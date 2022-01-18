//
// Created by Chris Luttio on 6/22/21.
//

#ifndef P11_MAIL_SERVER_EMAIL_MODEL_H
#define P11_MAIL_SERVER_EMAIL_MODEL_H

#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

namespace sql {
    sql_create_8(email, 1, 0,
     mysqlpp::sql_int, id,
     mysqlpp::sql_varchar, name,
     mysqlpp::sql_varchar, sender,
     mysqlpp::sql_varchar, subject,
     mysqlpp::sql_varchar, to,
     mysqlpp::sql_varchar, status,
     mysqlpp::sql_datetime, created_at,
     mysqlpp::sql_int, account_id
    );
}

#endif //P11_MAIL_SERVER_EMAIL_MODEL_H
