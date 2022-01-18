//
// Created by Chris Luttio on 6/24/21.
//

#include "mail_state.h"

void MailState::append_receive_queue(const std::shared_ptr<Email> &email) {
    unsaved.push_back(email);
    std::string user = get_local_part(email->sender);
    mailboxes[user][email->name] = email;
}

void MailState::append_send_queue(const std::shared_ptr<Email> &email) {
    unsaved.push_back(email);
    outbound_emails[email->name] = email;
    std::string user = get_local_part(email->sender);
    mailboxes[user][email->name] = email;
}