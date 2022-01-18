//
// Created by Chris Luttio on 3/25/21.
//

#include <general/constants.h>
#include "receive_requests_task.h"
#include "http/http_message_parser.h"
#include "http/http_request_reader.h"

void ReceiveRequestsTask::perform() {
    for (const auto& pair : state->connections) {
        auto connection = std::make_shared<Connection>(*pair.second);
        if (!connection->alive || connection->protocol != Http)
            continue;
        int error = connection->socket.get_error();
        if (error == 0) {
            auto profile = state->get_access_profile();
            HttpRequestReader reader(profile.request_size_limit);
            int result = reader.read(connection);
            if (result != 0) {
                connection->active_requests++;
                connection->last_read = std::chrono::high_resolution_clock::now();
                if (connection->security == UNSECURE && state->ssl_enabled) {
                    auto client_request = std::make_shared<ClientRequest>();
                    client_request->connection = connection;
                    client_request->type = RedirectSSL;
                    client_request->status = Complete;
                    if (result == 1) {
                        client_request->uri = reader.get_message()->url;
                    }
                    connection->persistence = Connection::CLOSE;

                    _controller->apply(Action(CreateClientRequest, client_request));
                } else {
                    if (result == 1) {
                        _controller->apply(Action(CreateHttpRequest, std::make_shared<HTTPRequestEnvelope>(connection, reader.get_message())));
                    } else {
                        auto client_request = std::make_shared<ClientRequest>();
                        client_request->connection = connection;
                        client_request->type = BadRequest;
                        client_request->status = Complete;
                        connection->persistence = Connection::CLOSE;

                        _controller->apply(Action(CreateClientRequest, client_request));
                    }
                }
                _controller->apply(Action(ModifyClientConnection, connection));
            }
        } else {
            connection->terminate();
        }
    }
}