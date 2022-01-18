//
// Created by Chris Luttio on 6/12/21.
//

#include "store_email_task.h"
#include "dirent.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <cmath>

#include "models/email_model.h"
#include "models/label_model.h"
#include "models/email_labels.h"

void StoreEmailTask::perform() {
    for (const auto& email : _state->mail->unsaved) {
        try {
            auto fetch_query = _state->database->query("SELECT * FROM emails WHERE name = %0q");
            fetch_query.parse();
            std::vector<sql::email> emails;
            fetch_query.storein(emails, email->name);
            if (emails.empty()) {
                auto from_header = email->header->get_address_header("from");
                auto from = from_header[0];
                auto local = from.local;
                auto domain = from.domain;

                auto rcpts = email->header->get_address_header("to");
                EmailAddress to = rcpts[0];
                for (const auto& rcpt : rcpts) {
                    if (rcpt.domain == _state->config->sender_domain) {
                        to = rcpt;
                        break;
                    }
                }

                sql::email record;
                record.id = 0;
                record.name = email->name;
                record.sender = from.get();
                record.to = to.get();
                record.subject = email->header->find_header("subject")[0]->generate();

                std::shared_ptr<UserAccount> account;
                std::string username;
                if (domain == _state->config->sender_domain) {
                    username = local;
                } else {
                    username = to.local;
                }
                for (const auto& a : _state->accounts) {
                    if (a->username == username) {
                        account = a;
                        break;
                    }
                }
                if (account != nullptr) {
                    record.account_id = account->id;
                } else {
                    record.account_id = _state->unknown_account->id;
                }
                record.created_at = mysqlpp::NOW();
                auto create_query = _state->database->query();
                create_query.insert(record);
                auto id = create_query.execute().insert_id();

                auto rules_query = _state->database->query("select * from rules where account_id = %0");
                rules_query.parse();
                auto rows = rules_query.store(account->id);
                for (const auto& row : rows) {
                    std::string match = std::string(row["match"].c_str());
                    bool is_match = false;
                    if (match[0] == '*') {
                        if (match.ends_with(domain)) {
                            is_match = true;
                        }
                    } else {
                        is_match = match == from.get();
                    }
                    if (is_match) {
                        auto query = _state->database->query("insert into email_labels (email_id, label_id) values (%0, %1)");
                        query.parse();
                        query.execute(id, row["label_id"]);
                    }
                }
            }
        } catch (const mysqlpp::Exception& er) {
            std::cerr << er.what() << std::endl;
        }
        if (email->stored)
            continue;
        try {
            auto path = "emails/" + email->name + ".eml";
            FILE *file = fopen(path.c_str(), "w");
            if (file) {
                auto data = email->generate();
                fwrite(data.c_str(), 1, data.size(), file);
                fclose(file);
                email->stored = true;
            }
        } catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }
    _state->mail->unsaved.clear();
}