//
// Created by Chris Luttio on 3/28/21.
//

#include "gtest/gtest.h"
#include "url.h"

TEST(URLTests, Test1) {
    auto url = URL::parse("hello");
    ASSERT_EQ("hello", url.components[0]);
}

TEST(URLTests, Test2) {
    auto url = URL::parse("src/picture.png");
    ASSERT_FALSE(url.is_root);
    ASSERT_EQ("src", url.components[0]);
    ASSERT_EQ("picture", url.components[1]);
    ASSERT_EQ("png", url.extension);
}

TEST(URLTests, Test3) {
    auto url = URL::parse("/src/picture.png");
    ASSERT_TRUE(url.is_root);
    ASSERT_EQ("src", url.components[0]);
    ASSERT_EQ("picture", url.components[1]);
    ASSERT_EQ("png", url.extension);
}

TEST(URLTests, Test4) {
    auto url = URL::parse("/src/picture.png");
    ASSERT_TRUE(url.is_root);
    ASSERT_EQ("src", url.components[0]);
    ASSERT_EQ("picture", url.components[1]);
    ASSERT_EQ("png", url.extension);
}

TEST(URLTests, Test5) {
    auto url = URL::parse("hello.txt");
    ASSERT_EQ("hello", url.components[0]);
    ASSERT_EQ(1, url.components.size());
    ASSERT_EQ("txt", url.extension);
}

TEST(URLTests, web) {
    auto url = URL::parse("http://www.google.com");
    ASSERT_EQ("http", url.protocol);
    ASSERT_EQ(0, url.components.size());
    ASSERT_EQ("", url.extension);
    ASSERT_EQ("com", url.domain[0]);
    ASSERT_EQ("google", url.domain[1]);
    ASSERT_EQ("www", url.domain[2]);
}

TEST(URLTests, web2) {
    auto url = URL::parse("http://www.google.com/");
    ASSERT_EQ("http", url.protocol);
    ASSERT_EQ(0, url.components.size());
    ASSERT_EQ("", url.extension);
    ASSERT_EQ("com", url.domain[0]);
    ASSERT_EQ("google", url.domain[1]);
    ASSERT_EQ("www", url.domain[2]);
}

TEST(URLTests, web3) {
    auto url = URL::parse("https://www.storiel.com/games/epic-game.html");
    ASSERT_EQ("https", url.protocol);
    ASSERT_EQ(2, url.components.size());
    ASSERT_EQ("games", url.components[0]);
    ASSERT_EQ("epic-game", url.components[1]);
    ASSERT_EQ("html", url.extension);
    ASSERT_EQ("com", url.domain[0]);
    ASSERT_EQ("storiel", url.domain[1]);
    ASSERT_EQ("www", url.domain[2]);
}

//TEST(URLTests, web3) {
//    auto url = URL::parse("https://www.storiel.com/games?name=odysseus");
//    ASSERT_EQ("https", url.protocol);
//    ASSERT_EQ(1, url.components.size());
//    ASSERT_EQ("games", url.components[0]);
//    ASSERT_EQ(1, url.query.size());
//    ASSERT_EQ("odysseus", url.query["name"]);
//    ASSERT_EQ("", url.extension);
//    ASSERT_EQ("com", url.domain[0]);
//    ASSERT_EQ("storiel", url.domain[1]);
//    ASSERT_EQ("www", url.domain[2]);
//}

TEST(URLTests, to_string1) {
    auto url = URL();
    url.components.push_back("hello");
    url.extension = "txt";
    ASSERT_EQ("hello.txt", url.to_string());
}

TEST(URLTests, to_string2) {
    auto url = URL();
    url.components.push_back("hello");
    url.components.push_back("world");
    url.extension = "txt";
    ASSERT_EQ("hello/world.txt", url.to_string());
}

TEST(URLTests, to_string3) {
    auto url = URL();
    url.components.push_back("src");
    url.components.push_back("picture");
    url.extension = "png";
    url.is_root = true;
    ASSERT_EQ("/src/picture.png", url.to_string());
}

TEST(URLTests, to_string4) {
    auto url = URL();
    url.components.push_back("src");
    url.components.push_back("picture");
    url.extension = "png";
    url.protocol = "http";
    url.domain.push_back("com");
    url.domain.push_back("hello");
    ASSERT_EQ("http://hello.com/src/picture.png", url.to_string());
}

TEST(URLTests, to_string5) {
    auto str = "https://www.storiel.com/games/epic.html";
    auto url = URL::parse(str);
    ASSERT_EQ(str, url.to_string());
}

TEST(URLTests, to_string6) {
    auto str = "/main.dart.js";
    auto url = URL::parse(str);
    ASSERT_EQ(str, url.to_string());
}

TEST(URLTests, prepend1) {
    auto url = URL::parse("/hello.html");
    url.prepend("web");
    ASSERT_EQ("/web/hello.html", url.to_string());
}

TEST(URLTests, prepend2) {
    auto url = URL::parse("/");
    url.append("index.html");
    ASSERT_EQ("/index.html", url.to_string());
}

TEST(URLTests, combine1) {
    auto url = URL::append(URL::parse("/"), URL::parse("index.html"));
    ASSERT_EQ("/index.html", url.to_string());
}

TEST(URLTests, combine2) {
    auto url = URL::append(URL::parse("/src"), URL::parse("index.html"));
    ASSERT_EQ("/src/index.html", url.to_string());
}

TEST(URLTests, combine3) {
    auto url = URL::append(URL::parse("/src"), URL::parse("resource/index.html"));
    ASSERT_EQ("/src/resource/index.html", url.to_string());
}

TEST(URLTests, unix) {
    auto url = URL::parse("unix:///var/run/docker.sock");
    ASSERT_EQ("unix", url.protocol);
    ASSERT_EQ("unix:///var/run/docker.sock", url.to_string());
}

TEST(URLTests, port) {
    auto url = URL::parse("http://localhost:8000");
    ASSERT_EQ("http", url.protocol);
    ASSERT_EQ("localhost", std::string(url.domain_to_cstr()));
    ASSERT_EQ(8000, url.port);
}

TEST(URLTests, port_to) {
    auto url = URL::parse("http://localhost:8000");
    ASSERT_EQ("http://localhost:8000/", url.to_string());
}