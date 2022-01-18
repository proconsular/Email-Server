//
// Created by Chris Luttio on 6/12/21.
//

#ifndef P11_MAIL_SERVER_EMAIL_PARSER_H
#define P11_MAIL_SERVER_EMAIL_PARSER_H

#include <iostream>
#include "email.h"
#include "email_body.h"
#include "email_header.h"

class EmailParser {
public:
    static std::shared_ptr<Email> parse(const std::shared_ptr<std::string>&);
    static std::unique_ptr<EmailBody> parse_body(uint32_t, uint32_t, const EmailHeader*, const std::string*);

    static uint32_t parse_body_part(uint32_t, const EmailHeader*, const std::string*, std::vector<std::unique_ptr<BodyPart>>&);
    static uint32_t find_start_boundary(uint32_t, const std::string&, const std::string*);

    static std::string parse_header_value(uint32_t&, const std::string*);
};

#endif //P11_MAIL_SERVER_EMAIL_PARSER_H
