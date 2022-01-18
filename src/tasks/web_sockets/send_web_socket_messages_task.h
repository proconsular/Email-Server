//
// Created by Chris Luttio on 5/10/21.
//

#ifndef P8_WEB_SERVER_SEND_WEB_SOCKET_MESSAGES_TASK_H
#define P8_WEB_SERVER_SEND_WEB_SOCKET_MESSAGES_TASK_H

#include "tasks/task.h"

#include <utility>
#include "general/state.h"
#include "controllers/controller.h"

class SendWebSocketMessagesTask: public Task {
public:
    explicit SendWebSocketMessagesTask(std::shared_ptr<State> state): _state(std::move(state)), _alive(true) {}

    void perform() override;

    bool alive() override {
        return _alive;
    }
private:
    bool _alive;

    std::shared_ptr<State> _state;
};


#endif //P8_WEB_SERVER_SEND_WEB_SOCKET_MESSAGES_TASK_H
