//
// Created by Chris Luttio on 4/27/21.
//

#include "initialize_tls_server_socket_task.h"
#include "utils.h"
#include <thread>
#include <openssl/err.h>

void InitializeTLSServerSocketTask::perform() {
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();

    auto sock = std::make_shared<Socket>();

    int tries = 5;
    while (tries >= 0) {
        int id = socket(AF_INET, SOCK_STREAM, 0);
        if (id < 0) {
            break;
        }

        fcntl(id, F_SETFL, O_NONBLOCK);

        sock->ip_address.sin_family = AF_INET;
        sock->ip_address.sin_addr.s_addr = INADDR_ANY;
        sock->ip_address.sin_port = htons(443);
        sock->_port = 443;

        if (bind(id, (sockaddr *) &sock->ip_address, sizeof(sock->ip_address)) < 0) {
            close(id);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            tries--;
            continue;
        }

        if (listen(id, 3) < 0) {
            close(id);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            tries--;
            continue;
        }

        sock->id = id;

        if (id >= 0)
            break;

        tries--;
    }

    bool ok = true;

    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX* context = SSL_CTX_new(method);

    if (!context) {
        ok = false;
    }

    SSL_CTX_set_ecdh_auto(context, 1);

    if (SSL_CTX_use_certificate_file(context, "keys/server.cert", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        ok = false;
    }

    if (SSL_CTX_use_PrivateKey_file(context, "keys/server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        ok = false;
    }

    if (sock->id < 0 && !ok) {
        _controller->apply(Action(ReportError, std::make_shared<std::string>("TLS socket creation failed")));
    } else {
        _state->ssl_method = method;
        _state->ssl_context = context;
        _state->ssl_enabled = true;

        _controller->apply(Action(CreateTLSServerSocket, sock));
    }

    _alive = false;
}