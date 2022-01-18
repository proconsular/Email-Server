//
// Created by Chris Luttio on 6/3/21.
//

#ifndef P11_MAIL_SERVER_EMAIL_H
#define P11_MAIL_SERVER_EMAIL_H

#include <string>
#include <vector>
#include <chrono>
#include <map>

#include "general/connection.h"
#include "mail_identifier.h"
#include "email_header.h"
#include "email_body.h"

class Email {
public:
    explicit Email(): encoding(_7bit), status(NEW), stored(false), id(0) {
        name = MailIdentifier::generate_id();
        header = std::make_unique<EmailHeader>();
        body = std::make_unique<EmailBody>();
        raw_message = nullptr;
    }

    enum Status {
        NEW,
        PENDING,
        CONNECTED,
        SENT,
        FAILED,
        RECEIVED,
    };

    enum TransferEncoding {
        _7bit,
        _8bit,
        base64,
    };

    static std::shared_ptr<Email> create(const std::string& from, const std::vector<std::string>& to, const std::string& subject, const std::shared_ptr<std::string>& body) {
        auto message = std::make_shared<Email>();
        message->sender = from;
        message->recipients = to;
        message->subject = subject;
        auto body_part = std::make_unique<BodyPart>();
        body_part->content = std::make_unique<std::string>(*body);
        message->body->body_parts.push_back(std::move(body_part));
        message->header->insert("from", from);
        std::string rcpts;
        for (const auto& s : to)
            rcpts.append("<" + s + ">, ");
        rcpts.resize(rcpts.size() - 2);
        message->header->insert("to", rcpts);
        message->header->insert("subject", subject);
        message->header->insert("date", get_time_str(std::chrono::system_clock::now()));
        message->header->insert("mime-version", "1.0");
//        message->headers["list-unsubscribe"] = "<mailto:unsubscribe@drade.io>";
        message->header->insert("content-type", "text/plain; charset=utf-8");
        message->header->normalize();
        message->header->parse();
        return message;
    }

    uint32_t id;

    std::string name;

    bool stored;

    std::chrono::system_clock::time_point received;
    std::chrono::system_clock::time_point sent;

    std::string sender;
    std::vector<std::string> recipients;
    std::string subject;
    TransferEncoding encoding;

    std::unique_ptr<EmailHeader> header;
    std::unique_ptr<EmailBody> body;

    std::unique_ptr<std::string> raw_message;

    Status status;

    std::string generate() const;
    std::string get_encoded_body(bool fold) const;
    static std::string prepare_for_send(const std::string&);

    std::string generate_body() const;
};

#endif //P11_MAIL_SERVER_EMAIL_H
