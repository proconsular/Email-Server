//
// Created by Chris Luttio on 3/28/21.
//

#ifndef P8_WEB_SERVER_LOAD_CONFIGURATION_TASK_H
#define P8_WEB_SERVER_LOAD_CONFIGURATION_TASK_H

#include "controlled_task.h"

#include <utility>
#include "general/state.h"
#include "controllers/controller.h"

struct LoadConfigurationTask: ControlledTask {
    LoadConfigurationTask(std::shared_ptr<State> state, std::shared_ptr<Controller> controller): ControlledTask(std::move(state), std::move(controller)) {}

    void perform() override;
};


#endif //P8_WEB_SERVER_LOAD_CONFIGURATION_TASK_H
