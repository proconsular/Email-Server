//
// Created by Chris Luttio on 6/27/21.
//

#ifndef P11_MAIL_SERVER_TAG_FIELD_H
#define P11_MAIL_SERVER_TAG_FIELD_H

#include <string>
#include <utility>

class TagField {
public:
    explicit TagField(std::string name, std::string value): name(std::move(name)), value(std::move(value)) {}

    std::string name;
    std::string value;
};

#endif //P11_MAIL_SERVER_TAG_FIELD_H
