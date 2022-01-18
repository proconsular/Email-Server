//
// Created by Chris Luttio on 6/27/21.
//

#ifndef P11_MAIL_SERVER_HEADER_FIELD_H
#define P11_MAIL_SERVER_HEADER_FIELD_H

#include <string>
#include <utility>
#include <vector>
#include <map>

#include "email_address.h"
#include "objects/general/tag_field.h"

class HeaderField {
public:

    std::string name;

    virtual std::string generate() const {
        return "";
    }
};

class StringHeaderField: public HeaderField {
public:
    explicit StringHeaderField(std::string name, std::string value): value(std::move(value)) {
        this->name = std::move(name);
    }

    std::string value;

    std::string generate() const {
        return value;
    };
};

class EmailAddressHeaderField: public HeaderField {
public:
    EmailAddressHeaderField() = default;
    explicit EmailAddressHeaderField(std::string name, std::vector<EmailAddress> addresses): value(std::move(addresses)) {
        this->name = std::move(name);
    }

    std::vector<EmailAddress> value;

    std::string generate() const;
};

class TaggedHeaderField: public HeaderField {
public:
    TaggedHeaderField(std::string name, std::vector<TagField> tags): value(std::move(tags)) {
        this->name = std::move(name);
    }

    std::vector<TagField> value;

    static std::vector<TagField> parse(const std::string&);

    std::string get(const std::string& key) const {
        return this->operator[](key);
    }

    std::string operator[](const std::string& key) const {
        for (const auto& tag : value) {
            if (tag.name == key)
                return tag.value;
        }
        return "";
    }

    void set(const std::string& key, const std::string& val) {
        for (auto& tag : value) {
            if (tag.name == key) {
                tag.value = val;
                break;
            }
        }
    }

    std::string generate() const;
};

class ContentTypeHeaderField: public HeaderField {
public:

    std::string type;
    std::string subtype;
    std::map<std::string, std::string> attributes;

    const std::string& get_attribute(const std::string& key) const {
        return attributes.at(key);
    }

    std::string generate() const;

    static ContentTypeHeaderField parse(std::string, const std::string&);
};

#endif //P11_MAIL_SERVER_HEADER_FIELD_H
