//
// Created by Chris Luttio on 6/27/21.
//

#ifndef P11_MAIL_SERVER_EMAIL_ADDRESS_H
#define P11_MAIL_SERVER_EMAIL_ADDRESS_H

#include <string>

class EmailAddress {
public:

    std::string name;
    std::string local;
    std::string domain;

    std::string get() const {
        return local + "@" + domain;
    }

    static EmailAddress parse(const std::string&);
};


#endif //P11_MAIL_SERVER_EMAIL_ADDRESS_H
