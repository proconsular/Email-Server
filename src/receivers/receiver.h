//
// Created by Chris Luttio on 4/5/21.
//

#ifndef P8_WEB_SERVER_RECEIVER_H
#define P8_WEB_SERVER_RECEIVER_H

#include "actions/action.h"

class Receiver {
public:
    virtual void receive(const Action&) = 0;
};

#endif //P8_WEB_SERVER_RECEIVER_H
