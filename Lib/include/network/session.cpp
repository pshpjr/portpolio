//
// Created by pshpjr on 26. 3. 30..
//

#include "session.h"

#include "packet.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>

#include <cstring>
#include <thread>

namespace psh::lib::network
{

Session::Session(boost::asio::io_context& ioContext, boost::asio::ip::tcp::socket&& socket, IoHandler& ioHandler)
    : ioContext_(ioContext)
    , socket_(std::move(socket))
    , ioHandler_(ioHandler)
{
}

auto Session::GetIoContext() const -> boost::asio::io_context&
{
    return ioContext_;
}

auto Session::GetEndpoint() const -> boost::asio::ip::tcp::endpoint
{
    return socket_.remote_endpoint();
}

void Session::StartRecv()
{
    if (isClosed_.load() || needSendClose_.load())
    {
        return;
    }

    if (bufferEnd_ == buffer_.size())
    {
        if (bufferBegin_ == 0)
        {
            Close();
            return;
        }

        const size_t remainingBytes = bufferEnd_ - bufferBegin_;
        std::memmove(buffer_.data(), buffer_.data() + bufferBegin_, remainingBytes);
        bufferBegin_ = 0;
        bufferEnd_ = remainingBytes;
    }

    socket_.async_read_some(
        boost::asio::buffer(buffer_.data() + bufferEnd_, buffer_.size() - bufferEnd_),
        [this, self = shared_from_this()](boost::system::error_code ec, std::size_t recvBytes) mutable
        {
            if (ec || recvBytes == 0)
            {
                Close();
                return;
            }

            bufferEnd_ += recvBytes;

            while (true)
            {
                const size_t availableBytes = bufferEnd_ - bufferBegin_;
                if (availableBytes < sizeof(PacketHeader))
                {
                    break;
                }

                auto [isComplete, packetLength] = Packet::CheckLength(buffer_.data() + bufferBegin_, availableBytes);
                if (!isComplete)
                {
                    break;
                }

                bufferBegin_ += sizeof(PacketHeader);
                ioHandler_.NotifyRecv(buffer_.data() + bufferBegin_, static_cast<int>(packetLength));
                bufferBegin_ += packetLength;
            }

            if (bufferBegin_ == bufferEnd_)
            {
                bufferBegin_ = 0;
                bufferEnd_ = 0;
            }
            else if (bufferBegin_ > 0)
            {
                const size_t remainingBytes = bufferEnd_ - bufferBegin_;
                std::memmove(buffer_.data(), buffer_.data() + bufferBegin_, remainingBytes);
                bufferBegin_ = 0;
                bufferEnd_ = remainingBytes;
            }

            StartRecv();
        });
}

void Session::Send(const char* data, size_t length)
{
    if (isClosed_.load() || needSendClose_.load())
    {
        return;
    }

    auto sendBuffer = std::make_shared<std::vector<char>>(data, data + length);
    sendQueue_.enqueue(sendBuffer);
    pendingSendCount_.fetch_add(1);
    TryStartSend();
}

void Session::Close()
{
    if (isClosed_.exchange(true))
    {
        return;
    }

    FinishClose();
}

void Session::SendClose()
{
    if (isClosed_.load())
    {
        return;
    }

    needSendClose_.store(true);

    if (isSending_.load() || pendingSendCount_.load() > 0 || currentSendBuffer_)
    {
        return;
    }

    FinishClose();
}

void Session::FinishClose()
{
    if (!isClosed_.exchange(true))
    {
        currentSendBuffer_.reset();
    }
    else if (!socket_.is_open())
    {
        return;
    }

    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket_.close(ec);
    ioHandler_.OnSessionClosed(this);
    auto self = shared_from_this();
    ioHandler_.NotifyDisconnect(self);
}

void Session::TryStartSend()
{
    if (!isSending_.exchange(true))
    {
        StartSend();
    }
}

void Session::StartSend()
{
    if (isClosed_.load())
    {
        isSending_.store(false);
        return;
    }

    if (needSendClose_.load() && pendingSendCount_.load() == 0 && !currentSendBuffer_)
    {
        isSending_.store(false);
        FinishClose();
        return;
    }

    std::shared_ptr<std::vector<char>> nextBuffer;
    while (!sendQueue_.try_dequeue(nextBuffer))
    {
        if (pendingSendCount_.load() == 0)
        {
            isSending_.store(false);
            if (pendingSendCount_.load() == 0)
            {
                if (needSendClose_.load())
                {
                    FinishClose();
                }
                return;
            }

            bool expected = false;
            if (!isSending_.compare_exchange_strong(expected, true))
            {
                return;
            }
        }
        else
        {
            std::this_thread::yield();
        }
    }

    pendingSendCount_.fetch_sub(1);
    currentSendBuffer_ = std::move(nextBuffer);

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(*currentSendBuffer_),
        [this, self = shared_from_this()](boost::system::error_code ec, std::size_t /*sentBytes*/) mutable
        {
            if (ec)
            {
                FinishClose();
                return;
            }

            currentSendBuffer_.reset();
            StartSend();
        });
}

} // namespace psh::lib::network
