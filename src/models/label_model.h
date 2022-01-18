//
// Created by Chris Luttio on 7/17/21.
//

#ifndef P11_MAIL_SERVER_LABEL_MODEL_H
#define P11_MAIL_SERVER_LABEL_MODEL_H

#if !defined(EXPAND_MY_SSQLS_STATICS)
#   define MYSQLPP_SSQLS_NO_STATICS
#endif

#include <mysql++/mysql++.h>
#include <mysql++/ssqls.h>

namespace sql {
    sql_create_2(label, 1, 0,
                 mysqlpp::sql_int, id,
                 mysqlpp::sql_varchar, name
    );
}

#endif //P11_MAIL_SERVER_LABEL_MODEL_H
