//
// Created by Chris Luttio on 4/25/21.
//

#include "gtest/gtest.h"

#include "utils.h"

TEST(route_applies_tests, test1) {
    ASSERT_TRUE(route_applies("*", "/hello"));
    ASSERT_FALSE(route_applies("/", "/hello"));
    ASSERT_TRUE(route_applies("/*", "/hello"));
    ASSERT_FALSE(route_applies("/chees", "/hello"));
    ASSERT_FALSE(route_applies("/hello/world", "/hello"));
    ASSERT_FALSE(route_applies("/hello/", "/hello"));
    ASSERT_TRUE(route_applies("/hello/*", "/hello/world"));
    ASSERT_TRUE(route_applies("/hello/*", "/hello/world/chhese"));
    ASSERT_FALSE(route_applies("/heo/*", "/hello/world/chhese"));
    ASSERT_TRUE(route_applies("/hello.txt", "/hello.txt"));
}