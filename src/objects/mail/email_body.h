//
// Created by Chris Luttio on 6/28/21.
//

#ifndef P11_MAIL_SERVER_EMAIL_BODY_H
#define P11_MAIL_SERVER_EMAIL_BODY_H

#include "body_part.h"

#include <string>
#include <vector>

class EmailBody {
public:

    std::vector<std::unique_ptr<BodyPart>> body_parts;

    const BodyPart* find_alternative(const std::string& type) {
        for (const auto& part : body_parts) {
            std::string part_type = "text/plain";
            auto content_type = part->headers->get_content_type_header();
            if (content_type) {
                part_type = content_type->type + "/" + content_type->subtype;
            }
            if (part_type == type)
                return part.get();
        }
        return nullptr;
    }
};

#endif //P11_MAIL_SERVER_EMAIL_BODY_H
