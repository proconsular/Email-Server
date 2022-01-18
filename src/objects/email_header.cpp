//
// Created by Chris Luttio on 6/27/21.
//

#include "email_header.h"

#include <map>
#include <algorithm>

void EmailHeader::insert(const std::string &key, const std::string &value) {
    fields.push_back(std::make_unique<StringHeaderField>(key, value));
}

void EmailHeader::erase(const std::string &key) {
    for (int i = 0; i < fields.size();) {
        const auto& field = fields[i];
        if (field->name == key) {
            fields.erase(fields.begin() + i);
        } else {
            i++;
        }
    }
}

void EmailHeader::normalize() {
    for (auto& field : fields) {
        std::transform(field->name.begin(), field->name.end(), field->name.begin(), [](char c) {
           return std::tolower(c);
        });
    }
}

void EmailHeader::parse() {
    static std::map<std::string, Headers> field_interpretation = {
            {"from", Address},
            {"to", Address},
            {"dkim-signature", Tagged},
            {"content-type", ContentType},
    };

    for (int i = 0; i < fields.size(); i++) {
        auto field = fields[i].get();
        auto str_field = reinterpret_cast<StringHeaderField*>(field);
        if (str_field) {
            std::string value = str_field->value;
            Headers type = field_interpretation[field->name];
            switch (type) {
                case Address: {
                    std::vector<EmailAddress> addresses;
                    int n = 0, m;
                    while (n < value.size()) {
                        m = n;
                        while (n < value.size() && value[n] != ',') n++;
                        if (m < n)
                            addresses.push_back(EmailAddress::parse(value.substr(m, n - m)));
                        n++;
                    }
                    fields[i] = std::make_unique<EmailAddressHeaderField>(field->name, addresses);
                    break;
                }
                case Tagged: {
                    std::vector<TagField> tags = TaggedHeaderField::parse(value);
                    fields[i] = std::make_unique<TaggedHeaderField>(field->name, tags);
                    break;
                }
                case ContentType: {
                    fields[i] = std::make_unique<ContentTypeHeaderField>(ContentTypeHeaderField::parse(field->name, value));
                    break;
                }
                default:
                    break;
            }
        }
    }
}

std::vector<EmailAddress> EmailHeader::get_address_header(const std::string &name) const {
    auto headers = find_header(name);
    if (!headers.empty()) {
        auto* header = reinterpret_cast<const EmailAddressHeaderField*>(headers[0]);
        if (header)
            return header->value;
    }
    return {};
}

const TaggedHeaderField* EmailHeader::get_tagged_header(const std::string &name) const {
    auto headers = find_header(name);
    if (!headers.empty())
        return reinterpret_cast<const TaggedHeaderField*>(headers[0]);
    return nullptr;
}

const ContentTypeHeaderField* EmailHeader::get_content_type_header() const {
    auto headers = find_header("content-type");
    if (!headers.empty())
        return reinterpret_cast<const ContentTypeHeaderField*>(headers[0]);
    return nullptr;
}

std::vector<const HeaderField*> EmailHeader::find_header(const std::string &name) const {
    std::vector<const HeaderField*> headers;
    for (auto& field : fields) {
        if (field->name == name) {
            headers.push_back(field.get());
        }
    }
    return headers;
}