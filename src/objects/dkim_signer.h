//
// Created by Chris Luttio on 6/8/21.
//

#ifndef P11_MAIL_SERVER_DKIM_SIGNER_H
#define P11_MAIL_SERVER_DKIM_SIGNER_H

#include <string>
#include <utility>
#include <openssl/evp.h>

#include "email.h"

class DKIMSigner {
public:
    explicit DKIMSigner(): private_key(nullptr) {}
    explicit DKIMSigner(std::string domain, std::string selector): domain(std::move(domain)), selector(std::move(selector)), private_key(nullptr), rsa(
            nullptr) {}

    void load_key(const std::string&);
    std::string sign(const std::shared_ptr<Email>&);

    static std::string _hash(const std::string&) ;
    static std::string _sign(const std::string&, evp_pkey_st*) ;
    static std::string _generate(const std::map<std::string, std::string>&, bool fold);
    static std::string _generate_list(const std::vector<std::vector<std::string>>&);

    static std::string _canonize_header_simple(const std::vector<std::vector<std::string>>&);
    static std::string _canonize_header_relaxed(const std::vector<std::vector<std::string>>&);

    static std::string _canonize_body_simple(const std::string&);
    static std::string _canonize_body_relaxed(const std::string&);
private:
    std::string selector;
    std::string domain;

    EVP_PKEY *private_key;
    RSA* rsa;
};


#endif //P11_MAIL_SERVER_DKIM_SIGNER_H
