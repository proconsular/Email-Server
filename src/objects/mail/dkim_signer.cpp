//
// Created by Chris Luttio on 6/8/21.
//

#include "dkim_signer.h"
#include <algorithm>

enum Canonicalization {
    Simple,
    Relaxed,
};

void DKIMSigner::load_key(const std::string &filename) {
    FILE *file = fopen(filename.c_str(), "rb");
    private_key = EVP_PKEY_new();
    PEM_read_RSAPrivateKey(file, &rsa, nullptr, nullptr);
    EVP_PKEY_assign_RSA(private_key, rsa);
    fclose(file);
}

std::string DKIMSigner::sign(const std::shared_ptr<Email> &email) {
    auto hc = Relaxed;
    auto bc = Relaxed;

    auto now = std::chrono::system_clock::now();
    auto tn = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    auto tx = tn + 60 * 60 * 24;

    std::string headers;
    for (const auto& h : email->header->fields) {
        headers.append(h->name + ":");
    }
    headers.resize(headers.size() - 1);
    std::string headers_lower;
    headers_lower.resize(headers.size());
    std::transform(headers.begin(), headers.end(), headers_lower.begin(), [](char c) {
       return std::tolower(c);
    });

    std::map<std::string, std::string> parameters;
    parameters["v"] = "1";
    parameters["a"] = "rsa-sha256";
    parameters["d"] = domain;
    parameters["s"] = selector;
    parameters["c"] = std::string(hc == Simple ? "simple" : "relaxed") + "/" + (bc == Simple ? "simple" : "relaxed");
    parameters["h"] = headers_lower;
    parameters["b"] = "";
    parameters["bh"] = "";

    std::string email_body = email->generate_body();
    std::string body_to_be_signed = bc == Simple ? _canonize_body_simple(email_body) : _canonize_body_relaxed(email_body);

    std::string bh64 = _hash(body_to_be_signed);

    parameters["bh"] = bh64;

    std::string headers_to_be_signed;
    if (hc == Simple) {
        std::vector<std::vector<std::string>> headers_list;
        for (const auto& h : email->header->fields) {
            headers_list.push_back({h->name, h->generate()});
        }
        headers_to_be_signed.append(_canonize_header_simple(headers_list));
        headers_to_be_signed.append("DKIM-Signature: " + _generate(parameters, false));
    } else {
        std::vector<std::vector<std::string>> headers_list;
        for (const auto& h : email->header->fields) {
            headers_list.push_back({h->name, h->generate()});
        }
        headers_list.push_back({"dkim-signature", _generate(parameters, false)});
        headers_to_be_signed = _canonize_header_relaxed(headers_list);
        headers_to_be_signed.resize(headers_to_be_signed.size() - 2);
    }

    parameters["b"] = _sign(headers_to_be_signed, private_key);

    return _generate(parameters, true);
}

std::string DKIMSigner::_canonize_header_simple(const std::vector<std::vector<std::string>>& headers) {
    std::string output;
    for (const auto& h : headers) {
        output.append(h[0] + ": " + h[1] + "\r\n");
    }
    return output;
}

std::string DKIMSigner::_canonize_body_simple(const std::string &body) {
    std::string output = body;
    std::string::size_type i = output.size() - 1;
    while (i > 0 && (output[i] == '\n' || output[i] == '\r')) i--;
    output.resize(i + 1);
    output.append("\r\n");
    return output;
}

std::string DKIMSigner::_hash(const std::string &data) {
    auto ctx = EVP_MD_CTX_create();
    EVP_DigestInit(ctx, EVP_sha256());
    EVP_DigestUpdate(ctx, data.c_str(), data.size());
    uint32_t len = 0;
    auto* buffer = new uint8_t[EVP_MD_size(EVP_sha256())];
    EVP_DigestFinal(ctx, buffer, &len);
    std::string output = encode_base_64(buffer, len);
    delete[] buffer;
    EVP_MD_CTX_destroy(ctx);
    return output;
}

std::string DKIMSigner::_sign(const std::string &data, evp_pkey_st* key) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_create();
    EVP_DigestSignInit(ctx, nullptr, EVP_sha256(), nullptr, key);
    EVP_DigestSignUpdate(ctx, data.c_str(), data.size());

    size_t msg_len;
    EVP_DigestSignFinal(ctx, nullptr, &msg_len);

    auto* bh = new uint8_t[msg_len];
    EVP_DigestSignFinal(ctx, bh, &msg_len);
    EVP_MD_CTX_destroy(ctx);

    std::string output = encode_base_64(bh, msg_len);
    delete[] bh;

    return output;
}

std::string DKIMSigner::_generate(const std::map<std::string, std::string> &parameters, bool fold) {
    std::string output;

    for (const auto& p : parameters) {
        if (p.first.empty())
            continue;
        output.append(p.first);
        output.append("=");
        if (p.first == "bh" || p.first == "b") {
            for (int i = 0; i < p.second.size(); i++) {
                if (fold)
                    if (i % 70 == 69)
                        output += "\r\n ";
                output += p.second[i];
            }
        } else {
            output.append(p.second);
        }
        output.append("; ");
        if (fold)
            output.append("\r\n ");
    }
    output.resize(output.size() - (fold ? 5 : 2));

    return output;
}

std::string DKIMSigner::_generate_list(const std::vector<std::vector<std::string>> &parameters) {
    std::string output;

    for (const auto& p : parameters) {
        if (p[0].empty())
            continue;
        output.append(p[0]);
        output.append("=");
        if (p[0] == "bh" || p[0] == "b") {
            for (int i = 0; i < p[1].size(); i++) {
                output += p[1][i];
            }
        } else {
            output.append(p[1]);
        }
        output.append("; ");
    }
    output.resize(output.size() - 2);

    return output;
}

std::string DKIMSigner::_canonize_body_relaxed(const std::string &body) {
    std::string output;

    int i = 0;
    while (i < body.size()) {
        if (body[i] == ' ' || body[i] == '\t') {
            while (i < body.size() && isspace(body[i]) && body[i] != '\r') i++;
            if (body[i] != '\r')
                output.append(" ");
        }
        if (i < body.size())
            output += body[i++];
    }

    return _canonize_body_simple(output);
}

std::string DKIMSigner::_canonize_header_relaxed(const std::vector<std::vector<std::string>>& headers) {
    std::string output;

    for (const auto& h : headers) {
        std::string name = h[0];
        std::string name_l;
        name_l.resize(name.size());
        std::transform(name.begin(), name.end(), name_l.begin(), [](char c) {
           return std::tolower(c);
        });
        output.append(name_l);
        output.append(":");

        std::string value = h[1];
        int i = 0;
        while (i < value.size()) {
            if (value[i] == ' ' || value[i] == '\t') {
                output += ' ';
                while (i < value.size() && isspace(value[i])) i++;
            }
            if (i < value.size())
                output += value[i++];
        }

        output.append("\r\n");
    }

    return output;
}