//
// Created by Chris Luttio on 6/13/21.
//

#ifndef P11_MAIL_SERVER_DKIM_VERIFIER_H
#define P11_MAIL_SERVER_DKIM_VERIFIER_H

#include "objects/mail/email.h"

class DKIMVerifier {
public:
    enum Result {
        FAIL,
        NEUTRAL,
        PASS,
    };

    static Result verify(const std::shared_ptr<Email>&);

    static std::map<std::string, std::string> parse_field(const std::string&);

    static evp_pkey_st* public_key_from_str(const std::string&);

    static bool verify_signature(const std::string&, const std::string&, evp_pkey_st*);
};


#endif //P11_MAIL_SERVER_DKIM_VERIFIER_H
