//
// Created by Chris Luttio on 4/25/21.
//

#include "route_resolver.h"
#include "utils.h"

void RouteResolver::resolve(const std::vector<Route> &routes, const std::string &request_url) {
    url = request_url;

    for (const auto& route: routes) {
        if (route_applies(route.url, request_url)) {
            switch (route.type) {
                case COMPLEX:
                case BASIC: {
                    for (const auto & attribute : route.attributes) {
                        attributes[attribute.first] = attribute.second;
                    }
                    if (!route.default_file.empty()) {
                        url = route.default_file;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}