//
// Created by Chris Luttio on 7/17/21.
//

#ifndef P11_MAIL_SERVER_RECEIVE_RULE_LABELS_H
#define P11_MAIL_SERVER_RECEIVE_RULE_LABELS_H

#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

namespace sql {
    sql_create_3(receive_rule_labels, 1, 0,
                 mysqlpp::sql_int, id,
                 mysqlpp::sql_int, rule_id,
                 mysqlpp::sql_int, label_id
    );
}

#endif //P11_MAIL_SERVER_RECEIVE_RULE_LABELS_H
