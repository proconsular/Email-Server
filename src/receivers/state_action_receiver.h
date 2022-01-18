//
// Created by Chris Luttio on 4/5/21.
//

#ifndef P8_WEB_SERVER_STATE_ACTION_RECEIVER_H
#define P8_WEB_SERVER_STATE_ACTION_RECEIVER_H

#include "receiver.h"

#include <utility>
#include "general/state.h"

class StateActionReceiver: public Receiver {
public:
    explicit StateActionReceiver(std::shared_ptr<State> state): _state(std::move(state)) {}

    void receive(const Action &) override;

private:
    std::shared_ptr<State> _state;
};


#endif //P8_WEB_SERVER_STATE_ACTION_RECEIVER_H
