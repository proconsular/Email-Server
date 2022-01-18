//
// Created by Chris Luttio on 4/25/21.
//

#ifndef P8_WEB_SERVER_ROUTE_RESOLVER_H
#define P8_WEB_SERVER_ROUTE_RESOLVER_H

#include "objects/route.h"

class RouteResolver {
public:
    void resolve(const std::vector<Route>&, const std::string&);

    std::string url;
    std::map<std::string, std::string> attributes;
};


#endif //P8_WEB_SERVER_ROUTE_RESOLVER_H
