//
// Created by Chris Luttio on 7/17/21.
//

#ifndef P11_MAIL_SERVER_RECEIVE_RULE_H
#define P11_MAIL_SERVER_RECEIVE_RULE_H

#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

namespace sql {
    sql_create_3(receive_rule, 1, 0,
                 mysqlpp::sql_int, id,
                 mysqlpp::sql_varchar, match,
                 mysqlpp::sql_int, account_id
    );
}

#endif //P11_MAIL_SERVER_RECEIVE_RULE_H
