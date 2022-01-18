//
// Created by Chris Luttio on 4/25/21.
//

#include "gtest/gtest.h"
#include "route_resolver.h"

TEST(route_resolver_tests, test1) {
    auto r1 = Route();
    r1.type = BASIC;
    r1.url = "/*";
    r1.attributes["Language"] = "fr";
    r1.attributes["Content-Type"] = "text/plain";
    r1.attributes["Cache-Control"] = "public, max-age=30";
    auto r2 = Route();
    r2.type = BASIC;
    r2.url = "/hello/*";
    r2.attributes["Language"] = "en";
    r2.attributes["Access"] = "private";

    RouteResolver resolver;
    resolver.resolve({r1, r2}, "/hello/world");

    ASSERT_EQ("/hello/world", resolver.url);
    ASSERT_EQ("en", resolver.attributes["Language"]);
    ASSERT_EQ("text/plain", resolver.attributes["Content-Type"]);
    ASSERT_EQ("private", resolver.attributes["Access"]);
}