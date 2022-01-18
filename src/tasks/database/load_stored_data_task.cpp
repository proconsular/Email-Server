//
// Created by Chris Luttio on 6/23/21.
//

#include "load_stored_data_task.h"

#define EXPAND_MY_SSQLS_STATICS
#include "models/account_model.h"
#include "models/email_model.h"
#include "models/label_model.h"
#include "models/receive_rule.h"
#include "models/receive_rule_labels.h"
#include "models/email_labels.h"

void LoadStoredDataTask::perform() {
    std::vector<sql::account> accounts;
    _state->database->query("select * from accounts")
        .storein(accounts);
    for (const auto& account : accounts) {
        UserAccount user;
        user.id = account.id;
        user.username = account.username;
        user.password = account.password;
        user.role = account.role;
        auto auth = Authorization();
        auth.access_token = account.access_token;
        auth.refresh_token = account.refresh_token;
        user.authorizations.push_back(auth);
        auto usr_account = std::make_shared<UserAccount>(user);
        _state->accounts.push_back(usr_account);
        if (user.username == "unknown")
            _state->unknown_account = usr_account;
    }
    sql::account::table("accounts");
    sql::email::table("emails");
    sql::label::table("labels");
    sql::receive_rule::table("receive_rules");
    sql::receive_rule_labels::table("receive_rule_labels");
    sql::email_labels::table("email_labels");
    _alive = false;
}