//
// Created by Chris Luttio on 4/5/21.
//

#ifndef P8_WEB_SERVER_LOG_ACTION_RECEIVER_H
#define P8_WEB_SERVER_LOG_ACTION_RECEIVER_H

#include "receiver.h"
#include <string>

#include <utility>

class LogActionReceiver: public Receiver {
public:
    explicit LogActionReceiver(std::string);

    void receive(const Action &) override;
private:
    std::string _filename;
    FILE* _file;
};


#endif //P8_WEB_SERVER_LOG_ACTION_RECEIVER_H
