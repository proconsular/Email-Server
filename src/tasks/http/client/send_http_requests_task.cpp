//
// Created by Chris Luttio on 3/31/21.
//

#include "send_http_requests_task.h"

void SendHTTPRequestsTask::perform() {
    for (const auto& pair: _state->active_requests) {
        auto envelope = std::make_shared<HTTPRequestCarrier>(*pair.second);
        if (envelope->status == CONNECTED) {
            auto data = envelope->http_request->generate();
            if (envelope->connection->write(data)) {
                envelope->status = SENT;
                _controller->apply(Action(SendHttpRequest, envelope));
            } else {
                auto err = envelope->connection->socket.get_error();
                if (err != 0) {
                    if (_state->config->auto_reconnect_on_request_failure && envelope->connection_attempts < 3) {
                        envelope->status = NEW;
                        envelope->connection_attempts++;
                    } else {
                        envelope->status = FAILED;
                    }
                    envelope->connection->terminate();
                    _controller->apply(Action(ModifyHttpCarrier, envelope));
                }
            }
        }
    }
}