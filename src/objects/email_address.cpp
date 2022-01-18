//
// Created by Chris Luttio on 6/27/21.
//

#include "email_address.h"

/*
 * Three forms:
 *  1. jon@mail.com
 *  2. <jon@mail.com>
 *  3. "Jon" <jon@mail.com>
 *  4. Jon <jon@mail.com>
 */

EmailAddress EmailAddress::parse(const std::string &value) {
    EmailAddress address;

    int i = 0;

    while (i < value.size()) {
        while (i < value.size() && value[i] == ' ') i++;
        if (value[i] == '<') {
            i++;
            int s = i;
            while (i < value.size() && value[i] != '@') i++;
            address.local = value.substr(s, i - s);
            i++;
            s = i;
            while (i < value.size() && value[i] != '>' && value[i] != ' ') i++;
            address.domain = value.substr(s, i - s);
            i++;
        } else if (value[i] == '"') {
            i++;
            int s = i;
            while (i < value.size() && value[i] != '"') i++;
            address.name = value.substr(s, i - s);
            i++;
        } else {
            int s = i;
            int a = i;
            while (i < value.size() && value[i] != '<') {
                if (value[i] == '@')
                    a = i;
                i++;
            }
            if (i < value.size() && value[i] == '<') {
                int n = i - 1;
                while (n > 0 && value[n] == ' ') n--;
                address.name = value.substr(s, n - s + 1);
            } else {
                address.local = value.substr(s, a - s);
                address.domain = value.substr(a + 1);
            }
        }
    }

    return address;
}