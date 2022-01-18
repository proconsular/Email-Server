//
// Created by Chris Luttio on 6/5/21.
//

#include "categorize_outbound_emails_task.h"
#include <unordered_set>

void CategorizeOutboundEmailsTask::perform() {
    for (auto& entry : _state->mail->outbound_emails) {
        auto email = entry.second;
        if (email->status == Email::NEW) {
            std::unordered_set<std::string> domains;
            for (auto& recipient : email->recipients) {
                int offset = 0;
                while (recipient[offset++] != '@');
                std::string domain = recipient.substr(offset);
                domains.insert(domain);
            }
            for (auto& domain : domains) {
                _state->mail->send_domains[domain].push_back(email);
            }
            email->status = Email::PENDING;
        }
    }
}