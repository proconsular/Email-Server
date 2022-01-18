//
// Created by Chris Luttio on 4/24/21.
//

#include "load_routing_data_task.h"
#include "objects/route.h"
#include "json.hpp"

#include <dirent.h>
#include <cstdio>
#include <fstream>
#include <algorithm>

void LoadRoutingDataTask::perform() {
    DIR *d;
    struct dirent *dir;
    const char* dir_name = _state->config->routing_directory.c_str();
    d = opendir(dir_name);
    if (d) {
        while ((dir = readdir(d)) != nullptr) {
            size_t e = strlen(dir->d_name);
            size_t n = e;
            while (dir->d_name[n] != '.') n--;
            if (strcmp("json", dir->d_name + n + 1) != 0)
                continue;

            auto path = std::string(dir_name) + dir->d_name;
            std::fstream file(path);

            nlohmann::json data;
            file >> data;

            file.close();

            Route route;

            route.url = data["Route"];
            if (data.find("Default") != data.end()) {
                route.default_file = data["Default"];
            }

            if (data.find("Formats") != data.end()) {
                route.type = COMPLEX;
                auto formats = data["Formats"];
                if (formats.is_array()) {
                    for (const auto& format : formats) {
                        std::map<std::string, std::string> parameters;
                        for (auto iter = format.cbegin(); iter != format.cend(); iter++) {
                            parameters[iter.key()] = iter.value();
                        }
                        route.formats.push_back(parameters);
                    }
                }
            } else {
                route.type = BASIC;
            }

            for (auto iter = data.cbegin(); iter != data.cend(); iter++) {
                bool inherited = true;
                for (const auto& key : {"Route", "Default", "Formats"}) {
                    if (iter.key() == key) {
                        inherited = false;
                        break;
                    }
                }
                if (inherited) {
                    route.attributes[iter.key()] = iter.value();
                }
            }

            _state->routes.push_back(route);
        }
        closedir(d);
    }
    std::sort(_state->routes.begin(), _state->routes.end(), [](const Route& a, const Route& b) {
        return compare_routes(a.url, b.url);
    });
    _alive = false;
}