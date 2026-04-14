//
// Created by pshpjr on 26. 3. 29..
//

#include "user.h"

User::User(UserID uid, psh::database::DbExecutor& dbExecutor)
    : m_uid{uid}, m_dbStrand{dbExecutor.MakeDbStrand()}
{
}
