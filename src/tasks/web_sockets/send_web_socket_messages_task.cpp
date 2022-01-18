//
// Created by Chris Luttio on 5/10/21.
//

#include "send_web_socket_messages_task.h"
#include "general/web_socket_message_generator.h"

#include <thread>

void SendWebSocketMessagesTask::perform() {
    for (auto& message : _state->outbound_web_socket_messages) {
        if (message.connection->websocket_state == CONNECTING)
            continue;
        auto frames = WebSocketMessageGenerator::generate(message);
        for (auto frame : frames) {
            std::string str;
            std::copy(frame.begin(), frame.end(), std::back_inserter(str));
            message.connection->write(std::make_shared<std::string>(str));
        }
        message.sent = true;
    }
    for (auto i = _state->outbound_web_socket_messages.begin(); i != _state->outbound_web_socket_messages.end();) {
        if (i->sent) {
            _state->outbound_web_socket_messages.erase(i);
        } else {
            i++;
        }
    }
}