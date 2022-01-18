//
// Created by Chris Luttio on 3/25/21.
//

#include "initialize_server_task.h"

#include <cstdlib>
#include <fcntl.h>
#include <memory>
#include "utils.h"
#include <openssl/ssl.h>

void InitializeServerTask::perform() {
    auto sock = std::make_shared<Socket>();
    int port = _state->config->port;

    while (true) {
        if (sock->init() == 0) {
            perror("Socket failed");
            exit(EXIT_FAILURE);
        }

        fcntl(sock->id, F_SETFL, O_NONBLOCK);

        sock->setup(port);

        if (sock->bind() < 0) {
            _controller->apply(Action(ReportError, std::make_shared<std::string>(string_format("Socket failed to bind: %i", port))));
            if (!_state->config->port_fixed) {
                port++;
                continue;
            }
        }

        break;
    }

    if (sock->listen(3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    _controller->apply(Action(CreateServerSocket, sock));

    _alive = false;
}