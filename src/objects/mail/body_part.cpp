//
// Created by Chris Luttio on 7/3/21.
//

#include "body_part.h"

std::string BodyPart::generate() const {
    std::string output;

    auto content_type = headers->get_content_type_header();

    for (const auto& header : headers->fields) {
        output += header->name + ": " + header->generate() + "\r\n";
    }
    output += "\r\n";

    if (content_type == nullptr) {
        if (content != nullptr)
            return *content;
    } else {
        if (content_type->type != "multipart") {
            if (content != nullptr)
                output += *content;
        } else {
            auto boundary = content_type->attributes.at("boundary");
            for (int i = 0; i < body_parts.size(); i++) {
                const auto& part = body_parts[i];
                output += "\r\n--" + boundary + "\r\n";
                output += part->generate();
            }
            output += "\r\n--" + boundary + "--\r\n";
        }
    }

    return output;
}