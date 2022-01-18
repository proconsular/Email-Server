//
// Created by Chris Luttio on 3/25/21.
//

#include "reception_task.h"

#include <arpa/inet.h>
#include <iostream>

void ReceptionTask::perform() {
    Socket new_client;
    if (state->server_socket.accept(new_client)) {
        std::string id = new_client.get_socket();
        if (state->connections.find(id) != state->connections.end()) {
            id += "/" + generate_hash_id(4);
        }
        _controller->apply(Action(CreateClientConnection, std::make_shared<Connection>(Client, id, new_client)));
    }
}

