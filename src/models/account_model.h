//
// Created by Chris Luttio on 6/22/21.
//

#ifndef P11_MAIL_SERVER_ACCOUNT_MODEL_H
#define P11_MAIL_SERVER_ACCOUNT_MODEL_H

#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

namespace sql {
    sql_create_6(account, 1, 0,
     mysqlpp::sql_int, id,
     mysqlpp::sql_varchar, username,
     mysqlpp::sql_varchar, password,
     mysqlpp::sql_varchar, role,
     mysqlpp::sql_varchar, access_token,
     mysqlpp::sql_varchar, refresh_token
    );
}

#endif //P11_MAIL_SERVER_ACCOUNT_MODEL_H
