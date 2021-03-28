//
// Created by Chris Luttio on 3/23/21.
//

#include "http_request_parser.h"
#include <vector>
#include <iostream>

HTTPRequest * HTTPRequestParser::parse(const std::string &data) {
    HTTPRequest* request = new HTTPRequest;

    std::string::const_iterator cursor = data.begin();

    processRequestLine(request, data, cursor);
    processHeaders(request, data, cursor);

    if (request->headers.find("Content-Length") != request->headers.end()) {
        int length = atoi(request->headers["Content-Length"].c_str());
        request->body = std::string(cursor, cursor + length);
    }

    return request;
}

void HTTPRequestParser::processHeaders(HTTPRequest *request, const std::string &data, std::string::const_iterator &cursor) {
    std::string::const_iterator i = cursor;

    std::string key;
    for (; cursor < data.end(); cursor++) {
        if (*cursor == ':') {
            key = std::string(i, cursor);
            i = cursor + 1;
        } else if (*cursor == '\r') {
            while (isspace(*i)) i++;
            request->headers[key] = std::string(i, cursor);
            std::string::const_iterator end = cursor;
            next(cursor, data);
            if (cursor - end == 4) {
                break;
            }
            i = cursor;
        }
    }

    if (!key.empty() && cursor == data.end()) {
        while (isspace(*i)) i++;
        request->headers[key] = std::string(i, cursor);
    }
}

void HTTPRequestParser::processRequestLine(HTTPRequest* request, const std::string& data, std::string::const_iterator& i) {
    std::vector<std::string> words;
    std::string word;

    for (; i != data.end(); i++) {
        if (*i == '\r')
            break;
        if (!isspace(*i)) {
            word += *i;
        } else {
            words.push_back(word);
            word = "";
        }
    }

    if (words.size() == 2 && !word.empty()) {
        words.push_back(word);
    }

    request->method = words[0];
    request->uri = words[1];
    request->version = words[2];

    next(i, data);
}