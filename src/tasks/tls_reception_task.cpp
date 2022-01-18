//
// Created by Chris Luttio on 4/27/21.
//

#include "tls_reception_task.h"
#include <openssl/err.h>
#include <thread>

void TLSReceptionTask::perform() {
    Socket new_client;
    while (_state->tls_socket.accept(new_client)) {
        auto connection = std::make_shared<Connection>();
        connection->socket = new_client;
        fcntl(new_client.id, F_SETFL, O_NONBLOCK);

        SSL* ssl = SSL_new(_state->ssl_context);
        SSL_set_fd(ssl, new_client.id);

	int ret = -1;
        int tries = 5;
        while (ret < 0 && tries >= 0) {
	    ret = SSL_accept(ssl);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            tries--;
        }
	
	if (ret < 0) {
            _controller->apply(Action(ReportError, std::make_shared<std::string>("SSL Accept failure")));
            close(connection->socket.id);
            SSL_free(ssl);
            continue;
        }

	
        connection->last_read = get_time();

        connection->ssl = ssl;
        connection->security = SECURE;

        std::string id = new_client.get_socket();
        if (_state->connections.find(id) != _state->connections.end()) {
            id += "/" + generate_hash_id(4);
        }

        connection->_id = id;

        _controller->apply(Action(CreateClientConnection, connection));
    }
}
