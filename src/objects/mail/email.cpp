//
// Created by Chris Luttio on 6/9/21.
//

#include "email.h"
#include <algorithm>

std::string Email::generate() const {
    std::string message;

    switch (encoding) {
        case base64:
            header->insert("Content-Transfer-Encoding", "base64");
            break;
        default:
            break;
    }

    header->insert("Message-ID", "<" + name + "@drade.io>");

    for (const auto& pair : header->fields) {
        std::string header_lower;
        header_lower.resize(pair->name.size());
        std::transform(pair->name.begin(), pair->name.end(), header_lower.begin(), [](char c) {
            return std::tolower(c);
        });
        message.append(header_lower + ": " + pair->generate() + "\r\n");
    }

    message.append("\r\n");
    message.append(prepare_for_send(generate_body()));

    return message;
}

std::string Email::generate_body() const {
    std::string output;

    auto content_type = header->get_content_type_header();

    if (content_type == nullptr) {
        if (!body->body_parts.empty()) {
            if (body->body_parts[0]->content != nullptr) {
                return *body->body_parts[0]->content;
            }
        }
    } else {
        if (content_type->type != "multipart") {
            if (!body->body_parts.empty()) {
                if (body->body_parts[0]->content != nullptr) {
                    output += *body->body_parts[0]->content;
                }
            }
        } else {
            auto boundary = content_type->attributes.at("boundary");
            for (int i = 0; i < body->body_parts.size(); i++) {
                const auto& part = body->body_parts[i];
                output += "\r\n--" + boundary + "\r\n";
                output += part->generate();
            }
            output += "\r\n--" + boundary + "--\r\n";
        }
    }

    return output;
}

std::string Email::get_encoded_body(bool fold) const {
    std::string bb = generate_body();
    switch (encoding) {
        case base64: {
            std::string b64 = encode_base_64((uint8_t*)bb.c_str(), bb.size());
            std::string output;
            for (int i = 0; i < b64.size(); i++) {
                if (fold)
                    if (i % 70 == 69)
                        output.append("\r\n");
                output.push_back(b64[i]);
            }
            return output;
        }
        default:
            break;
    }
    return bb;
}

std::string Email::prepare_for_send(const std::string &body) {
    std::string output;

    for (int i = 0; i < body.size(); i++) {
        if ((i == 0 || (i > 0 && body[i - 1] == '\n')) && body[i] == '.') {
            output.append("..");
        } else {
            output.push_back(body[i]);
        }
    }

    return output;
}