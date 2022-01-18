//
// Created by Chris Luttio on 4/30/21.
//

#include "gtest/gtest.h"
#include "applications/docker_interface_app.h"
#include "applications/http_client_app.h"

TEST(secure_http_server_tests, Basic_OK) {
    DockerInterfaceApp docker;

    std::string id;
    ASSERT_TRUE(docker.create_container(id, "p8_web_server_test", "443", "443"));
    ASSERT_TRUE(docker.start_container(id));
    ASSERT_TRUE(docker.wait_for_container_status(id, "running"));

    HttpClientApp http;
    auto request = HttpMessage::make_request("GET", "/hello.txt");
    std::shared_ptr<HttpMessage> response;
    bool received = http.send("https://localhost", request, response);

    docker.remove_container_forced(id);

    ASSERT_TRUE(received);

    ASSERT_EQ(200, response->code);
    ASSERT_EQ("OK", response->status);
    ASSERT_EQ("hello world", *response->body);
}

TEST(secure_http_server_tests, Reliability_Server_OK) {
    DockerInterfaceApp docker;

    const std::string host = string_format("https://localhost");

    std::string id;
    ASSERT_TRUE(docker.create_container(id, "p8_web_server_test", "443", "443"));
    ASSERT_TRUE(docker.start_container(id));
    ASSERT_TRUE(docker.wait_for_container_status(id, "running"));

    const int tries = 100;
    int passes = 0;
    int i = 0;

    while (i < tries) {
        HttpClientApp http;
        auto request = HttpMessage::make_request("GET", "/hello.txt");
        std::shared_ptr<HttpMessage> response;
        bool received = http.send(host, request, response);
        bool message_integrity = response->status == "OK" && response->code == 200 && *response->body == "hello world";
        if (received && message_integrity)
            passes++;
        i++;
    }

    docker.remove_container_forced(id);

    ASSERT_EQ(tries, passes);
}

TEST(secure_http_server_tests, Reliability_Server_Rounds_90KB_File) {
    FILE *big_file = fopen("../../web/big_file.txt", "r");
    char *buffer = new char[150000];
    size_t amount = fread(buffer, 1, 150000, big_file);
    fclose(big_file);

    std::string data = std::string(buffer, buffer + amount);
    delete[] buffer;

    ASSERT_EQ("Fusce feugiat eget odio ac mollis. Morbi vitae faucibus elit.", data.substr(0, 61));

    DockerInterfaceApp docker;

    const std::string host = string_format("https://localhost");

    std::string id;
    ASSERT_TRUE(docker.create_container(id, "p8_web_server_test", "443", "443"));
    ASSERT_TRUE(docker.start_container(id));
    ASSERT_TRUE(docker.wait_for_container_status(id, "running"));

    const int rounds = 100;
    int passes = 0;
    int round = 0;

    while (round < rounds) {
        HttpClientApp http;
        auto request = HttpMessage::make_request("GET", "/big_file.txt");
        std::shared_ptr<HttpMessage> response;
        bool received = http.send(host, request, response);
        if (received && response->body != nullptr) {
            if (data == *response->body) {
                passes++;
            }
        }
        round++;
    }

    docker.remove_container_forced(id);

    ASSERT_EQ(rounds, passes);
}