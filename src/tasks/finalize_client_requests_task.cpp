//
// Created by Chris Luttio on 3/27/21.
//

#include "finalize_client_requests_task.h"
#include "client_request.h"
#include "objects/http_message.h"

#include <memory>

void FinalizeClientRequestsTask::perform() {
    for (const auto& pair: state->requests) {
        auto request = std::make_shared<ClientRequest>(*pair.second);
        switch (request->status) {
            case Complete: {
                auto response = std::make_shared<HttpMessage>(RESPONSE);
                response->code = 200;
                response->version = "HTTP/1.1";
                response->status = "OK";
                switch (request->type) {
                    case RetrieveFile: {
                        response->body = request->data;
                        response->headers["Content-Type"] = std::make_shared<std::string>(getContentType(request->path.extension));
                        for (const auto & response_header : request->response_headers) {
                            response->headers[response_header.first] = std::make_shared<std::string>(response_header.second);
                        }
                        break;
                    }
                    case NotFound: {
                        response->body = std::make_shared<std::string>("");
                        response->code = 404;
                        response->status = "Not Found";
//                        response->headers["Content-Length"] = std::make_shared<std::string>("0");
                        break;
                    }
                    case BadRequest: {
                        response->body = std::make_shared<std::string>("");
                        response->code = 400;
                        response->status = "Bad Request";
//                        response->headers["Content-Length"] = std::make_shared<std::string>("0");
                        break;
                    }
                    case RedirectSSL: {
                        response->body = std::make_shared<std::string>("");
                        response->code = 301;
                        response->status = "Moved Permanently";
                        auto ext = request->uri.to_string();
                        auto url = string_format("https://%s%s", state->config->domain.c_str(), ext.c_str());
                        response->headers["Location"] = std::make_shared<std::string>(url);
                        break;
                    }
                    case Refresh:
                    case Authorize: {
                        response->body = request->data;
                        response->code = 200;
                        response->status = "OK";
                        response->headers["Content-Type"] = std::make_shared<std::string>("application/json");
                        break;
                    }
                    case WebSocketUpgrade: {
                        response->body = std::make_shared<std::string>("");
                        response->code = 101;
                        response->status = "Switching Protocols";
                        response->headers["Connection"] = std::make_shared<std::string>("Upgrade");
                        response->headers["Upgrade"] = std::make_shared<std::string>("websocket");
                        response->headers["Sec-WebSocket-Accept"] = std::make_shared<std::string>(request->response_headers["Sec-WebSocket-Accept"]);
                        request->connection->websocket_state = OPEN;
                        break;
                    }
                    case Created: {
                        response->body = request->data;
                        response->code = 201;
                        response->status = "Created";
                        break;
                    }
                    case SendMail: {
                        response->code = 201;
                        response->status = "Created";
                        response->body = std::make_shared<std::string>("");
//                        response->headers["Content-Length"] = std::make_shared<std::string>("0");
                        break;
                    }
                    case OK: {
                        response->code = 200;
                        response->status = "OK";
                        response->body = request->data;
                        for (const auto & response_header : request->response_headers) {
                            response->headers[response_header.first] = std::make_shared<std::string>(response_header.second);
                        }
                        break;
                    }
                    default:
                        response->status = "Not Implemented";
                        response->code = 501;
                        break;
                }
                _controller->apply(Action(CreateHttpResponse, std::make_shared<HTTPResponseEnvelope>(request->connection, response)));
                continue;
            }
            case Failed: {
                auto response = std::make_shared<HttpMessage>(RESPONSE);
                response->code = 500;
                response->version = "HTTP/1.1";
                response->status = "Internal Server Error";
                switch (request->type) {
                    case RetrieveFile: {
                        response->code = 404;
                        response->status = "Not Found";
                        response->body = std::make_shared<std::string>("");
//                        response->headers["Content-Length"] = std::make_shared<std::string>("0");
                        break;
                    }
                    case Authorize: {
                        response->code = 401;
                        response->status = "Unauthorized";
                        response->body = std::make_shared<std::string>("");
//                        response->headers["Content-Length"] = std::make_shared<std::string>("0");
                    }
                    case Forbidden: {
                        response->code = 403;
                        response->status = "Forbidden";
                        response->body = std::make_shared<std::string>("");
//                        response->headers["Content-Length"] = std::make_shared<std::string>("0");
                    }
                    default:
                        break;
                }
                _controller->apply(Action(CreateHttpResponse, std::make_shared<HTTPResponseEnvelope>(request->connection, response)));
                continue;
            }
            default:
                break;
        }
    }
    for (auto iter = state->requests.cbegin(), next_iter = iter; iter != state->requests.cend(); iter = next_iter) {
        auto request = iter->second;
        next_iter++;
        if (request->status == Complete || request->status == Failed || (request->status == New && request->type == Unsupported)) {
            _controller->apply(Action(RemoveClientRequest, request));
        }
    }
}

std::string FinalizeClientRequestsTask::getContentType(const std::string &ext) {
    std::map<std::string, std::string> ext_table = {
            {"html", "text/html"},
            {"js", "text/javascript"},
            {"jpg", "image/jpeg"},
            {"jpeg", "image/jpeg"},
            {"png", "image/png"},
            {"json", "application/json"},
            {"ttf", "font/ttf"},
            {"txt", "text/plain"},
            {"wav", "audio/wav"},
            {"xml", "application/xml"},
            {"bin", "application/octet-stream"},
            {"css", "text/css"},
    };
    if (ext_table.find(ext) != ext_table.end()) {
        return ext_table[ext];
    }
    return "text/plain";
}