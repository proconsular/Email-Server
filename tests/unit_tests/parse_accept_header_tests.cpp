//
// Created by Chris Luttio on 4/24/21.
//

#include "gtest/gtest.h"

#include "utils.h"

TEST(parse_accept_header_tests, test1) {
    auto list = parse_accept_header("text/html, */*;q=0.7");

    ASSERT_EQ("text/html", list[0]);
    ASSERT_EQ("*/*", list[1]);
}

TEST(parse_accept_header_tests, test2) {
    auto list = parse_accept_header("text/html, text/*, */*;q=0.7");

    ASSERT_EQ("text/html", list[0]);
    ASSERT_EQ("text/*", list[1]);
    ASSERT_EQ("*/*", list[2]);
}

TEST(parse_accept_header_tests, test3) {
    auto list = parse_accept_header("text/*, text/html, */*;q=0.7");

    ASSERT_EQ("text/html", list[0]);
    ASSERT_EQ("text/*", list[1]);
    ASSERT_EQ("*/*", list[2]);
}

TEST(parse_accept_header_tests, test4) {
    auto list = parse_accept_header("text/html;q=0.8, text/*;q=0.8, */*;q=0.8");

    ASSERT_EQ("text/html", list[0]);
    ASSERT_EQ("text/*", list[1]);
    ASSERT_EQ("*/*", list[2]);
}

TEST(parse_accept_header_tests, test5) {
    auto list = parse_accept_header("text/html;q=0.8, text/*;q=0.9, */*;q=0.8");

    ASSERT_EQ("text/*", list[0]);
    ASSERT_EQ("text/html", list[1]);
    ASSERT_EQ("*/*", list[2]);
}

TEST(parse_accept_header_tests, test6) {
    auto list = parse_accept_header("text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");

    ASSERT_EQ("text/html", list[0]);
    ASSERT_EQ("application/xhtml+xml", list[1]);
    ASSERT_EQ("application/xml", list[2]);
    ASSERT_EQ("*/*", list[3]);
}

TEST(parse_accept_header_tests, test7) {
    auto list = parse_accept_header("text/html");

    ASSERT_EQ("text/html", list[0]);
}
