//
// Created by Chris Luttio on 4/1/21.
//

#include "initialize_http_request_connections_task.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <thread>

void InitializeHTTPRequestConnectionsTask::perform() {
    for (const auto& pair: _state->active_requests) {
        auto carrier = pair.second;
        if (carrier->status == NEW) {
            auto _carrier = std::make_shared<HTTPRequestCarrier>(*carrier);

            std::shared_ptr<Connection> connection;

            std::string socket_name;

            if (carrier->url.protocol.substr(0, 4) == "http") {
                int port = 80;
                if (carrier->url.protocol == "https") {
                    port = 443;
                }
                if (carrier->url.port != 0) {
                    port = carrier->url.port;
                }
                socket_name = std::string(carrier->url.domain_to_cstr()) + ":" + std::to_string(port);
            } else if (carrier->url.protocol == "unix") {
                socket_name = carrier->url.to_string();
            }

            if (_state->connections.find(socket_name) != _state->connections.end()) {
                auto conn = _state->connections[socket_name];
                if (conn->alive) {
                    connection = conn;
                }
            }

            if (connection == nullptr) {
                Socket socket;
                if (carrier->url.protocol.substr(0, 4) == "http") {
                    socket = create_ip_socket(_carrier);
                } else if (carrier->url.protocol == "unix") {
                    socket = create_unix_socket(_carrier);
                }
                if (socket.id >= 0) {
                    connection = std::make_shared<Connection>(Server, socket_name, socket);
                }
            }

            bool ok = false;

            if (connection != nullptr) {
                if (carrier->url.protocol == "https") {
                    if (initialize_tls(connection)) {
                        ok = true;
                    }
                } else {
                    ok = true;
                }
            }

            if (ok) {
                _state->connections[connection->id()] = connection;
                _carrier->connection = connection;
                _carrier->status = CONNECTED;
            } else {
                _carrier->status = FAILED;
                if (connection != nullptr && connection->socket.id >= 0) {
                    close(connection->socket.id);
                }
            }

            _controller->apply(Action(InitializeHttpRequestConnection, _carrier));
        }
    }
}

Socket InitializeHTTPRequestConnectionsTask::create_ip_socket(const std::shared_ptr<HTTPRequestCarrier> &carrier) {
    sockaddr_in server_address{};

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(80);
    if (carrier->url.port != 0) {
        server_address.sin_port = htons(carrier->url.port);
    } else if (carrier->url.protocol == "https") {
        server_address.sin_port = htons(443);
    }

    struct addrinfo hints{}, *res, *res0;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int socket_id = -1;

    const char* domain = carrier->url.domain_to_cstr();
    if (strcmp("localhost", domain) == 0) {
        socket_id = socket(AF_INET, SOCK_STREAM, 0);

        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(socket_id, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
            close(socket_id);
            socket_id = -1;
        }
    } else {
        int error = getaddrinfo(domain, carrier->url.protocol.c_str(), &hints, &res0);
        delete domain;

        if (error) {
            return -1;
        }

        for (res = res0; res; res = res->ai_next) {
            socket_id = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
            if (socket_id < 0) {
                continue;
            }
            if (connect(socket_id, res->ai_addr, res->ai_addrlen) == -1) {
                close(socket_id);
                socket_id = -1;
                continue;
            }
            break;
        }
        sockaddr* addr = res0->ai_addr;
        auto* in = reinterpret_cast<sockaddr_in*>(addr);
        server_address.sin_addr = in->sin_addr;

        freeaddrinfo(res0);
    }

    Socket socket;
    socket.id = socket_id;
    socket.ip_address = server_address;

    return socket;
}

Socket InitializeHTTPRequestConnectionsTask::create_unix_socket(const std::shared_ptr<HTTPRequestCarrier> &carrier) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    auto str = carrier->url.to_string().substr(7);

    struct sockaddr_un addr{};
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, str.c_str(), str.size());

    int error = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (error < 0) {
        return -1;
    }

    Socket socket;
    socket.id = sock;
    socket.unix_address = addr;

    return socket;
}

bool InitializeHTTPRequestConnectionsTask::initialize_tls(std::shared_ptr<Connection> &connection) {
    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* context = SSL_CTX_new(method);
    SSL_CTX_set_mode(context, SSL_MODE_AUTO_RETRY);
    SSL* ssl = SSL_new(context);

    if (!ssl) {
        _controller->apply(Action(ReportError, std::make_shared<std::string>("SSL connection creation error.")));
        return false;
    }
    int err;

    err = SSL_set_fd(ssl, connection->socket.id);
    if (err <= 0) {
        _controller->apply(Action(ReportError, std::make_shared<std::string>(string_format("SSL connection error: SSL_set_fd: %d", err))));
        return false;
    }

    auto start = get_time();
    while (get_ms_to_now(start) <= 500 && err <= 0) {
        err = SSL_connect(ssl);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    if (err <= 0) {
        _controller->apply(Action(ReportError, std::make_shared<std::string>(string_format("SSL connection error: %d", err))));
        return false;
    }

    SSL_set_connect_state(ssl);

    connection->ssl_method = method;
    connection->ssl_context = context;
    connection->ssl = ssl;

    connection->security = SECURE;

    return true;
}