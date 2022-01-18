//
// Created by Chris Luttio on 4/24/21.
//

#include "gtest/gtest.h"

#include "tasks/load_routing_data_task.h"

TEST(load_routing_data_task_tests, test1) {
    std::shared_ptr<State> state = std::make_shared<State>();
    state->config->routing_directory = "../../routes/";

    auto task = LoadRoutingDataTask(state);
    task.perform();

//    auto route = state->routes["/hello"];
//
//    ASSERT_EQ("/hello", route.url);
//
//    auto first = route.content_type.begin();
//    ASSERT_EQ("text/html; charset=UTF-8", first->first);
//    ASSERT_EQ("/hello.html", first->second);
//
//    auto second = ++first;
//    ASSERT_EQ("text/plain; charset=UTF-8", second->first);
//    ASSERT_EQ("/hello.txt", second->second);
//
//    ASSERT_EQ("public, max-age=30", route.cache_control);
//    ASSERT_EQ("/hello.txt", route.default_file);
//    ASSERT_EQ("en", route.language.begin()->first);
}