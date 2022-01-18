//
// Created by Chris Luttio on 4/5/21.
//

#include "gtest/gtest.h"
#include "client_request.h"

TEST(CopyTests, CR1) {
    auto* r1 = new ClientRequest;
    r1->type = RetrieveFile;
    r1->uri = URL::parse("http://www.google.com");

    auto* r2 = new ClientRequest(*r1);

    r2->type = Unsupported;
    r2->data = std::make_shared<std::string>("hello world");
    r2->uri = URL::parse("/hello.txt");

    ASSERT_FALSE(r1->type == r2->type);
    ASSERT_FALSE(r1->data == r2->data);
    ASSERT_FALSE(r1->uri.to_string() == r2->uri.to_string());
}