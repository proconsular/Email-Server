//
// Created by Chris Luttio on 3/27/21.
//

#ifndef P8_WEB_SERVER_SEND_RESPONSES_TASK_H
#define P8_WEB_SERVER_SEND_RESPONSES_TASK_H

#include "task.h"
#include "state.h"

class SendResponsesTask: public Task {
public:
    SendResponsesTask(State* state): state(state), _alive(true) {}

    void perform() override;
    bool alive() override {
        return _alive;
    }
private:
    State* state;
    bool _alive;
};


#endif //P8_WEB_SERVER_SEND_RESPONSES_TASK_H
