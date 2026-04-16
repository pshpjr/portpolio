#ifndef PORTPOLIO_I_QUERY_H
#define PORTPOLIO_I_QUERY_H

#include <boost/asio/awaitable.hpp>
#include <boost/mysql/any_connection.hpp>

#include <system_error>

namespace psh::database
{

class IQuery
{
public:
    virtual ~IQuery() = default;

    IQuery(const IQuery&) = delete;
    IQuery& operator=(const IQuery&) = delete;
    IQuery(IQuery&&) = delete;
    IQuery& operator=(IQuery&&) = delete;

    virtual boost::asio::awaitable<std::error_code> Execute(boost::mysql::any_connection& conn) = 0;

protected:
    IQuery() = default;
};

} // namespace psh::database

#endif // PORTPOLIO_I_QUERY_H
