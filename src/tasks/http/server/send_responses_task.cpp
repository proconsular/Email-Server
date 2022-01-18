//
// Created by Chris Luttio on 3/27/21.
//

#include "send_responses_task.h"

void SendResponsesTask::perform() {
    for (const auto& envelope: state->outbound_http_response_queue) {
        envelope.response->headers["Access-Control-Allow-Origin"] = std::make_shared<std::string>("*");
        auto response = envelope.response->generate();
        envelope.connection->write(response);
        if (envelope.connection->persistence == Connection::CLOSE) {
            envelope.connection->terminate();
        }
    }
    if (!state->outbound_http_response_queue.empty()) {
        _controller->apply(Action(ClearHttpResponses));
    }
}