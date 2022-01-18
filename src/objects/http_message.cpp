//
// Created by Chris Luttio on 4/14/21.
//

#include "http_message.h"
#include "utils.h"

std::shared_ptr<std::string> HttpMessage::generate() const {
    auto output = std::make_shared<std::string>();

    std::vector<std::string> status_line;
    if (type == REQUEST) {
        status_line = {method, url.to_string(), version};
    } else {
        status_line = {version, std::to_string(code), status};
    }

    output->append(join(" ", status_line));
    output->append("\r\n");

    if (!headers.empty()) {
        std::vector<std::string> _headers;
        for (const auto& pair: headers) {
            _headers.push_back(join(": ", {pair.first, *pair.second}));
        }
        if (body != nullptr && !body->empty()) {
            _headers.push_back(join(": ", {"Content-Length", std::to_string(body->size())}));
        } else {
            _headers.push_back(join(": ", {"Content-Length", std::to_string(0)}));
        }
        output->append(join("\r\n", _headers));
        output->append("\r\n\r\n");
    }

    if (body != nullptr && !body->empty()) {
        output->append(*body);
    }

    return output;
}