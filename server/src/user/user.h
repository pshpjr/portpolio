//
// Created by pshpjr on 26. 3. 29..
//

#ifndef PORTPOLIO_USER_H
#define PORTPOLIO_USER_H

#include "common/type_def.h"
#include "database/db_executor.h"

class User
{
public:
    User(UserID uid, psh::database::DbExecutor& dbExecutor);

    UserID                                Uid() const noexcept { return m_uid; }
    const psh::database::DbStrand&        DbStrand() const noexcept { return m_dbStrand; }

private:
    UserID                  m_uid;
    psh::database::DbStrand m_dbStrand;
};

#endif //PORTPOLIO_USER_H
