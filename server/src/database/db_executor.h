#ifndef PORTPOLIO_DB_EXECUTOR_H
#define PORTPOLIO_DB_EXECUTOR_H

#include "database/i_query.h"

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/mysql/connection_pool.hpp>
#include <boost/mysql/pool_params.hpp>

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <system_error>
#include <thread>
#include <vector>

namespace psh::database
{

using DbStrand   = boost::asio::strand<boost::asio::io_context::executor_type>;
using DbCallback = std::function<void(std::error_code)>;

// DB IO 전용 io_context + 워커 스레드를 소유한다.
// Post 는 호출 스레드와 무관하게 항상 deferred 로 실행되며,
// 코루틴 본체와 onComplete 는 전달된 userDbStrand 위에서 실행된다.
class DbExecutor
{
public:
    explicit DbExecutor(boost::mysql::pool_params params,
                        std::size_t               workerCount = 1);
    ~DbExecutor();

    DbExecutor(const DbExecutor&)            = delete;
    DbExecutor& operator=(const DbExecutor&) = delete;
    DbExecutor(DbExecutor&&)                 = delete;
    DbExecutor& operator=(DbExecutor&&)      = delete;

    void Start();
    void Stop();

    // User 단위 DB 직렬화용 strand 를 생성한다. 반환된 strand 는 User 가 소유한다.
    DbStrand MakeDbStrand();

    // userDbStrand : DB 접근 직렬화를 담당하는 strand (User 소유).
    // query        : 바인드된 statement / 결과 저장소를 내부에 가진 쿼리.
    // onComplete   : userDbStrand 위에서 호출된다. 필드 strand 등 최종 타깃은
    //                호출자가 이 콜백 안에서 캡처로 넣어 직접 재-dispatch 한다.
    void Post(const DbStrand&         userDbStrand,
              std::shared_ptr<IQuery> query,
              DbCallback              onComplete);

private:
    boost::asio::io_context m_io;
    std::optional<boost::asio::executor_work_guard<
        boost::asio::io_context::executor_type>>
                                  m_workGuard;
    boost::mysql::connection_pool m_pool;
    std::vector<std::jthread>     m_workers;
    std::size_t                   m_workerCount;
    bool                          m_started{false};
};

} // namespace psh::database

#endif // PORTPOLIO_DB_EXECUTOR_H
