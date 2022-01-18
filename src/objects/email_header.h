//
// Created by Chris Luttio on 6/27/21.
//

#ifndef P11_MAIL_SERVER_EMAIL_HEADER_H
#define P11_MAIL_SERVER_EMAIL_HEADER_H

#include <iostream>
#include <vector>
#include <memory>

#include "header_field.h"

class EmailHeader {
public:

    enum Headers {
        String,
        Address,
        Tagged,
        ContentType,
    };

    std::vector<std::unique_ptr<HeaderField>> fields;

    std::vector<EmailAddress> get_address_header(const std::string&) const;
    const TaggedHeaderField* get_tagged_header(const std::string&) const;
    const ContentTypeHeaderField* get_content_type_header() const;

    std::vector<const HeaderField*> find_header(const std::string&) const;

    void insert(const std::string&, const std::string&);
    void erase(const std::string&);

    void normalize();
    void parse();
};


#endif //P11_MAIL_SERVER_EMAIL_HEADER_H
