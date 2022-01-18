//
// Created by Chris Luttio on 4/24/21.
//

#ifndef P8_WEB_SERVER_ROUTE_H
#define P8_WEB_SERVER_ROUTE_H

#include <iostream>
#include <map>
#include <vector>

enum RouteType {
    BASIC,
    COMPLEX,
};

class Route {
public:
    RouteType type;

    std::string url;
    std::string default_file;

    std::map<std::string, std::string> attributes;
    std::vector<std::map<std::string, std::string>> formats;
};


#endif //P8_WEB_SERVER_ROUTE_H
