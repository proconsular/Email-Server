//
// Created by Chris Luttio on 6/12/21.
//

#include "smtp_reception_task.h"

void SmtpReceptionTask::perform() {
    Socket new_client;
    while (_state->mail->smtp_socket.accept(new_client)) {
        auto connection = std::make_shared<Connection>();
        connection->type = Client;
        connection->_id = new_client.ip() + ":" + std::to_string(new_client.port()) + "/" + generate_hash_id(3);
        connection->socket = new_client;
        connection->protocol = Smtp;
        connection->alive = true;
        connection->security = UNSECURE;
        fcntl(new_client.id, F_SETFL, O_NONBLOCK);
        _state->mail->smtp_connections.push_back(connection);
        _controller->apply(Action(Print, std::make_shared<std::string>("CONNECTED: SMTP client: " + connection->id())));
    }
}