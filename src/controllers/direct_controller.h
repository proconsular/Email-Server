//
// Created by Chris Luttio on 4/4/21.
//

#ifndef P8_WEB_SERVER_DIRECT_CONTROLLER_H
#define P8_WEB_SERVER_DIRECT_CONTROLLER_H

#include "controller.h"
#include "receivers/receiver.h"

class DirectController: public Controller {
public:
    explicit DirectController(std::shared_ptr<State> state);

    void apply(const Action&) override;

    void add_receiver(const std::shared_ptr<Receiver>& receiver) {
        _receivers.push_back(receiver);
    }
private:
    std::vector<std::shared_ptr<Receiver>> _receivers;
};


#endif //P8_WEB_SERVER_DIRECT_CONTROLLER_H
