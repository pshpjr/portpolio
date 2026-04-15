#include "db_executor.h"

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/mysql/error_code.hpp>

#include <coroutine>
#include <cstdlib>
#include <exception>
#include <utility>

namespace psh::database
{

DbExecutor::DbExecutor(boost::mysql::pool_params params, std::size_t workerCount)
    : io_{},
      workGuard_{io_.get_executor()},
      pool_{io_, std::move(params)},
      workerCount_{workerCount == 0 ? std::thread::hardware_concurrency() : workerCount}
{
}

DbExecutor::~DbExecutor()
{
    Stop();
}

void DbExecutor::Start()
{
    if (std::exchange(started_, true))
        return;

    pool_.async_run(boost::asio::detached);

    workers_.reserve(workerCount_);
    for (std::size_t i = 0; i < workerCount_; ++i)
    {
        workers_.emplace_back(
            [this]
            {
                io_.run();
            });
    }
}

void DbExecutor::Stop()
{
    if (!std::exchange(started_, false))
        return;

    pool_.cancel();
    io_.stop();
    workers_.clear(); // jthread dtor joins
}

DbStrand DbExecutor::MakeDbStrand()
{
    return boost::asio::make_strand(io_.get_executor());
}

void DbExecutor::Post(const DbStrand& userDbStrand, std::shared_ptr<IQuery> query, DbCallback onComplete)
{
    boost::asio::co_spawn(
        userDbStrand,
        // 성공
        [this, queryP = std::move(query), cb = std::move(onComplete)]() mutable -> boost::asio::awaitable<void>
        {
            auto [ec, conn] = co_await pool_.async_get_connection(
                boost::asio::as_tuple(boost::asio::use_awaitable));
            if (!ec)
            {
                std::error_code ecExec = co_await queryP->Execute(conn.get());
                cb(ecExec);
            }
        },
        // 실패
        [](std::exception_ptr ep)
        {
            // 예외 금지 컨벤션: 코루틴 경계에서 빠져나오는 예외는
            // 복구 불가능한 프로그래밍 오류로 간주하고 즉시 종료.
            if (ep)
            {
                std::abort();
            }
        });
}

} // namespace psh::database
