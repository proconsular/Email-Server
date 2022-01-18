//
// Created by Chris Luttio on 6/28/21.
//

#ifndef P11_MAIL_SERVER_BODY_PART_H
#define P11_MAIL_SERVER_BODY_PART_H

#include "objects/mail/email_header.h"

#include <string>
#include <memory>
#include <map>

class BodyPart {
public:

    std::unique_ptr<EmailHeader> headers;
    std::unique_ptr<std::string> content;

    std::vector<std::unique_ptr<BodyPart>> body_parts;

    std::string generate() const;
};

#endif //P11_MAIL_SERVER_BODY_PART_H
