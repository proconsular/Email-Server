//
// Created by Chris Luttio on 6/24/21.
//

#ifndef P11_MAIL_SERVER_MAIL_STATE_H
#define P11_MAIL_SERVER_MAIL_STATE_H

#include <openssl/err.h>
#include "objects/mail/email.h"
#include "objects/mail/dkim_signer.h"

class MailState {
public:
    MailState() {
        email_signer = DKIMSigner("drade.io", "mail");
        email_signer.load_key("keys/dkim.pem");

        smtp_ssl_method = TLS_client_method();
        smtp_ssl_context = SSL_CTX_new(smtp_ssl_method);

        if (SSL_CTX_use_certificate_file(smtp_ssl_context, "keys/server.cert", SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stderr);
            exit(1);
        }

        if (SSL_CTX_use_PrivateKey_file(smtp_ssl_context, "keys/server.key", SSL_FILETYPE_PEM) <= 0) {
            ERR_print_errors_fp(stderr);
            exit(1);
        }

        SSL_CTX_set_mode(smtp_ssl_context, SSL_MODE_AUTO_RETRY);
    }

    void append_send_queue(const std::shared_ptr<Email>&);
    void append_receive_queue(const std::shared_ptr<Email>&);

    const SSL_METHOD *smtp_ssl_method;
    SSL_CTX *smtp_ssl_context;

    Socket smtp_socket;

    std::map<std::string, std::map<std::string, std::shared_ptr<Email>>> mailboxes;

    std::vector<std::shared_ptr<Email>> unsaved;

    std::map<std::string, std::shared_ptr<Email>> outbound_emails;
    std::map<std::string, std::shared_ptr<Email>> inbound_emails;

    std::map<std::string, std::vector<std::shared_ptr<Email>>> send_domains;
    std::map<std::string, std::shared_ptr<Connection>> send_domain_connections;

    std::vector<std::shared_ptr<Connection>> smtp_connections;

    DKIMSigner email_signer;
};


#endif //P11_MAIL_SERVER_MAIL_STATE_H
