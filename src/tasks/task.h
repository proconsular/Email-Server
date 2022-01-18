//
// Created by Chris Luttio on 3/25/21.
//

#ifndef P8_WEB_SERVER_TASK_H
#define P8_WEB_SERVER_TASK_H

struct Task {
    virtual void perform() = 0;
    virtual bool alive() = 0;
};

#endif //P8_WEB_SERVER_TASK_H
