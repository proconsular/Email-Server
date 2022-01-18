//
// Created by Chris Luttio on 4/25/21.
//

#include "gtest/gtest.h"

#include "utils.h"

TEST(route_sorting_tests, test1) {
    std::vector<std::string> routes = {
        "*",
        "/hello",
        "/*",
        "/hello/world",
        "/hello/*",
    };
    std::sort(routes.begin(), routes.end(), compare_routes);
    ASSERT_EQ("*", routes[0]);
    ASSERT_EQ("/*", routes[1]);
    ASSERT_EQ("/hello", routes[2]);
    ASSERT_EQ("/hello/*", routes[3]);
    ASSERT_EQ("/hello/world", routes[4]);
}

TEST(route_sorting_tests, test2) {
    std::vector<std::string> routes = {
            "/*",
            "*",
            "/hello",
            "/*",
            "/hello/world",
            "/hello/*",
    };
    std::sort(routes.begin(), routes.end(), compare_routes);
    ASSERT_EQ("*", routes[0]);
    ASSERT_EQ("/*", routes[1]);
    ASSERT_EQ("/*", routes[2]);
    ASSERT_EQ("/hello", routes[3]);
    ASSERT_EQ("/hello/*", routes[4]);
    ASSERT_EQ("/hello/world", routes[5]);
}

TEST(route_sorting_tests, test3) {
    std::vector<std::string> routes = {
            "/*",
            "*",
            "/yes/man/*",
            "/hello",
            "/*",
            "/yes/man",
            "/hello/world",
            "/hello/*",
    };
    std::sort(routes.begin(), routes.end(), compare_routes);
    ASSERT_EQ("*", routes[0]);
    ASSERT_EQ("/*", routes[1]);
    ASSERT_EQ("/*", routes[2]);
    ASSERT_EQ("/hello", routes[3]);
    ASSERT_EQ("/hello/*", routes[4]);
    ASSERT_EQ("/yes/man", routes[5]);
    ASSERT_EQ("/hello/world", routes[6]);
    ASSERT_EQ("/yes/man/*", routes[7]);
}