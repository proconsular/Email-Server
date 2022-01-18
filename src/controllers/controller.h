//
// Created by Chris Luttio on 4/4/21.
//

#ifndef P8_WEB_SERVER_CONTROLLER_H
#define P8_WEB_SERVER_CONTROLLER_H

#include <utility>

#include "general/state.h"
#include "actions/action.h"

class Controller {
public:
    explicit Controller(std::shared_ptr<State> state): _state(std::move(state)) {}

    virtual void apply(const Action&) = 0;
protected:
    std::shared_ptr<State> _state;
};

#endif //P8_WEB_SERVER_CONTROLLER_H
