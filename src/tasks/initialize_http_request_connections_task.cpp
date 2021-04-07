//
// Created by Chris Luttio on 4/1/21.
//

#include "initialize_http_request_connections_task.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void InitializeHTTPRequestConnectionsTask::perform() {
    for (auto carrier: _state->outbound_http_request_queue) {
        if (carrier->status == NEW) {
            sockaddr_in server_address{};

            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(80);

            struct addrinfo hints, *res, *res0;
            memset(&hints, 0, sizeof hints);
            hints.ai_family = PF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            const char* domain = carrier->url.domain_to_cstr();
            int error = getaddrinfo(domain, carrier->url.protocol.c_str(), &hints, &res0);
            delete domain;

            if (error) {
                carrier->status = FAILED;
                continue;
            }

            int socket_id = -1;
            for (res = res0; res; res = res->ai_next) {
                socket_id = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
                if (socket_id < 0) {
                    continue;
                }
                if (connect(socket_id, res->ai_addr, res->ai_addrlen) < 0) {
                    close(socket_id);
                    socket_id = -1;
                    continue;
                }
                break;
            }
            if (socket_id < 0) {
                carrier->status = FAILED;
            }
            freeaddrinfo(res0);

            if (socket_id >= 0) {
                auto connection = new Connection(Socket(socket_id));
                carrier->connection = connection;
                carrier->status = CONNECTED;
            }
        }
    }
}