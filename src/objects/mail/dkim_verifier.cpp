//
// Created by Chris Luttio on 6/13/21.
//

#include "dkim_verifier.h"
#include "dkim_signer.h"
#include "udns.h"
#include "general/utils.h"
#include <openssl/err.h>

DKIMVerifier::Result DKIMVerifier::verify(const std::shared_ptr<Email> &email) {
    TaggedHeaderField* field = const_cast<TaggedHeaderField *>(email->header->get_tagged_header("dkim-signature"));

//    std::map<std::string, std::string> parameters = parse_field(field);

    auto ctx = dns_new(nullptr);
    dns_open(ctx);

    auto domain = field->get("s") + "._domainkey." + field->get("d");
    auto txt_records =  dns_resolve_txt(ctx, domain.c_str(), 1, 0);

    if (!txt_records)
        return NEUTRAL;

    auto dkim_record = std::string(txt_records->dnstxt_txt->txt, txt_records->dnstxt_txt->txt + txt_records->dnstxt_txt->len);
    auto dkim_tags = parse_field(dkim_record);

    if (dkim_tags.find("p") == dkim_tags.end())
        return NEUTRAL;

    bool header_simple = true;
    bool body_simple = true;

    if (!field->get("c").empty()) {
        std::string canonization = field->get("c");
        int i = 0;
        while (i < canonization.size() && canonization[i] != '/') i++;
        header_simple = canonization.substr(0, i) == "simple";
        int s = i;
        i++;
        while (i < canonization.size()) i++;
        if (s < i) {
            body_simple = canonization.substr(s, i - s) == "simple";
        }
    }

    std::string body = "";// *email->body;
//    if (!email->header->find_header("content-transfer-encoding").empty()) {
//        auto value = email->headers["content-transfer-encoding"];
//        if (value == "base64") {
//            body = decode_base64(body);
//        }
//    }

    std::string canonized_body = body_simple ? DKIMSigner::_canonize_body_simple(body) : DKIMSigner::_canonize_body_relaxed(body);
    std::string body_hash = DKIMSigner::_hash(canonized_body);

    if (body_hash != field->get("bh"))
        return FAIL;

    std::vector<std::string> header_list;
    std::string headers_tag = field->get("h");
    int i = 0;
    while (i < headers_tag.size()) {
        int s = i;
        while (i < headers_tag.size() && headers_tag[i] != ':') i++;
        header_list.push_back(headers_tag.substr(s, i - s));
        i++;
    }

    std::vector<std::string> tag_names;
    int n = 0;
    for (const auto& tag : field->value) {
        tag_names.push_back(tag.name);
    }
//    tag_names.resize(tag_names.size() - 1);

    std::string b = field->get("b");
    field->set("b", "");

    std::vector<std::vector<std::string>> d_tag_list;
    d_tag_list.reserve(tag_names.size());
    for (const auto& t : tag_names) {
        d_tag_list.push_back({t, field->get(t)});
    }

    std::vector<std::vector<std::string>> signable_header_list;
    signable_header_list.reserve(header_list.size());
    for (const auto& h : header_list) {
        signable_header_list.push_back({h, email->header->find_header(h)[0]->generate()});
    }
    signable_header_list.push_back({"dkim-signature", DKIMSigner::_generate_list(d_tag_list)});

    std::string canonized_header;

    if (header_simple) {

    } else {
        canonized_header = DKIMSigner::_canonize_header_relaxed(signable_header_list);
        canonized_header.resize(canonized_header.size() - 2);
    }

    auto signature = decode_base64(b);

    auto public_key = public_key_from_str(dkim_tags["p"]);
    bool pass = verify_signature(canonized_header, signature, public_key);
    EVP_PKEY_free(public_key);

    if (!pass)
        return FAIL;

    return PASS;
}

evp_pkey_st* DKIMVerifier::public_key_from_str(const std::string &str) {
    auto p = "-----BEGIN PUBLIC KEY-----\r\n" + str + "\r\n-----END PUBLIC KEY-----";
    auto public_key = EVP_PKEY_new();
    RSA* rsa = nullptr;
    BIO *bio = BIO_new_mem_buf((void*)p.c_str(), p.size());
    rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, nullptr, nullptr);
    EVP_PKEY_assign_RSA(public_key, rsa);
    BIO_free(bio);
    return public_key;
}

std::map<std::string, std::string> DKIMVerifier::parse_field(const std::string &field) {
    std::map<std::string, std::string> parameters;

    int i = 0;
    while (i < field.size()) {
        int s = i;
        while (i < field.size() && !isspace(field[i]) && field[i] != '=') i++;
        std::string key = field.substr(s, i - s);
        while (i < field.size() && field[i] != '=') i++;
        i++;
        while (i < field.size() && isspace(field[i])) i++;
        s = i;
        while (i < field.size() && !isspace(field[i]) && field[i] != ';') i++;
        parameters[key] = field.substr(s, i - s);
        while (i < field.size() && field[i] != ';') i++;
        i++;
        while (i < field.size() && isspace(field[i])) i++;
    }

    return parameters;
}

bool DKIMVerifier::verify_signature(const std::string &data, const std::string& signature, evp_pkey_st *key) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_create();
    int err = 0;
    err = EVP_DigestVerifyInit(ctx, nullptr, EVP_sha256(), nullptr, key);
    err = EVP_DigestVerifyUpdate(ctx, data.c_str(), data.size());
    int ok = EVP_DigestVerifyFinal(ctx, (unsigned char*)signature.c_str(), signature.size());
    EVP_MD_CTX_destroy(ctx);
    return ok == 1;
}