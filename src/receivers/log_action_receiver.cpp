//
// Created by Chris Luttio on 4/5/21.
//

#include "log_action_receiver.h"
#include "general/socket.h"
#include "general/utils.h"
#include "general/configuration.h"
#include "http/http_request_envelope.h"
#include "http/http_response_envelope.h"
#include "general/client_request.h"
#include "http/http_request_carrier.h"

#include <iostream>

LogActionReceiver::LogActionReceiver(std::string filename): _filename(std::move(filename)) {
    _file = fopen(_filename.c_str(), "a");
}

void LogActionReceiver::receive(const Action &action) {
    std::string output;
    switch (action.type) {
        case StartProgram: {
            output.append("******** PROGRAM INITIALIZED ********");
            break;
        }
        case StartApp: {
            auto app = std::static_pointer_cast<std::string>(action.data);
            output.append(string_format("*********** APPLICATION: %s ***********", app->c_str()));
            break;
        }
        case SetConfiguration: {
            auto config = std::static_pointer_cast<Configuration>(action.data);
            output.append(string_format("LOADED: config: %s; path: %s, port: %i", config->from_file.c_str(), config->base_url.to_string().c_str(), config->port));
            break;
        }
        case CreateServerSocket: {
            auto socket = std::static_pointer_cast<Socket>(action.data);
            output.append(string_format("STARTED: server: %i", socket->port()));
            break;
        }case CreateTLSServerSocket: {
            auto socket = std::static_pointer_cast<Socket>(action.data);
            output.append(string_format("STARTED: TLS server: %i", socket->port()));
            break;
        }
        case CreateHttpRequest: {
            auto envelope = std::static_pointer_cast<HTTPRequestEnvelope>(action.data);
            output.append(
                    string_format("REQUEST: %s %s %s from %s",
                                  envelope->request->method.c_str(),
                                  envelope->request->url.to_string().c_str(),
                                  envelope->request->version.c_str(),
                                  envelope->connection->id().c_str()
                                  ));
            break;
        }
        case CreateClientConnection: {
            auto connection = std::static_pointer_cast<Connection>(action.data);
            output.append(string_format("ESTABLISHED: client connection: %s, secure: %s", connection->id().c_str(), connection->security == SECURE ? "yes" : "no"));
            break;
        }
        case RemoveClientConnection: {
            auto connection = std::static_pointer_cast<Connection>(action.data);
            output.append(string_format("DISCONNECTED: client connection: %s", connection->id().c_str()));
            break;
        }
        case CreateClientRequest: {
            auto request = std::static_pointer_cast<ClientRequest>(action.data);
            std::map<Requests, std::string> table = {
                    {Unsupported, "Unsupported"},
                    {RetrieveFile, "Retrieve File"},
                    {BadRequest, "Bad Request"},
            };
            output.append(string_format("CREATED: request: %s for %s: %s", request->id().c_str(), request->connection->id().c_str(), table[request->type].c_str()));
            break;
        }
        case ModifyClientRequest: {
            auto request = std::static_pointer_cast<ClientRequest>(action.data);
            std::map<Requests, std::string> table = {
                    {Unsupported, "Unsupported"},
                    {RetrieveFile, "Retrieve File"}
            };
            std::map<RequestStatus, std::string> status = {
                    {New, "New"},
                    {Working, "Working"},
                    {Complete, "Complete"},
                    {Failed, "Failed"},
            };
            output.append(string_format("UPDATED: request: %s for %s: %s, %s", request->id().c_str(), request->connection->id().c_str(), table[request->type].c_str(), status[request->status].c_str()));
            break;
        }
        case CreateHttpResponse: {
            auto response = std::static_pointer_cast<HTTPResponseEnvelope>(action.data);
            output.append(string_format("RESPONSE: %i %s %s to %s", response->response->code, response->response->status.c_str(), response->response->version.c_str(), response->connection->id().c_str()));
            break;
        }
        case CreateOutboundHttpRequest: {
            auto carrier = std::static_pointer_cast<HTTPRequestCarrier>(action.data);
            output.append(string_format("QUEUED OUTBOUND: REQUEST: (%s) %s %s %s to %s", carrier->id().c_str(), carrier->http_request->method.c_str(), carrier->http_request->url.to_string().c_str(), carrier->http_request->version.c_str(), carrier->url.domain_to_cstr()));
            break;
        }
        case InitializeHttpRequestConnection: {
            auto carrier = std::static_pointer_cast<HTTPRequestCarrier>(action.data);
            if (carrier->status == CONNECTED) {
                output.append(string_format("ESTABLISHED: with %s", carrier->url.to_string().c_str()));
            } else {
                output.append(string_format("FAILED: with %s", carrier->url.to_string().c_str()));
            }
            break;
        }
        case SendHttpRequest: {
            auto carrier = std::static_pointer_cast<HTTPRequestCarrier>(action.data);
            output.append(string_format("SENT: REQUEST: %s", carrier->id().c_str()));
            break;
        }
        case ReceiveHttpResponse: {
            auto carrier = std::static_pointer_cast<HTTPRequestCarrier>(action.data);
            if (carrier->status == FULFILLED) {
                output.append(string_format("RECEIVED: RESPONSE: %s, %i %s", carrier->id().c_str(), carrier->http_response->code, carrier->http_response->status.c_str()));
            } else {
                if (carrier->http_response != nullptr) {
                    output.append(string_format("FAILED: RESPONSE: %s, %i %s", carrier->id().c_str(), carrier->http_response->code, carrier->http_response->status.c_str()));
                } else {
                    output.append(string_format("FAILED: RESPONSE: %s", carrier->id().c_str()));
                }
            }
            break;
        }
        case ReportError: {
            auto error = std::static_pointer_cast<std::string>(action.data);
            output.append(string_format("ERROR: %s", error->c_str()));
            break;
        }
        case ReportLog: {
            auto error = std::static_pointer_cast<std::string>(action.data);
            output.append(string_format("LOG: \n%s", error->c_str()));
            break;
        }
        case Print: {
            auto error = std::static_pointer_cast<std::string>(action.data);
            output.append(string_format("%s", error->c_str()));
            break;
        }
        default:
            break;
    }
    if (output.empty()) {
        return;
    }
    auto timenow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto tm = std::ctime(&timenow);
    tm[strlen(tm) - 1] = '\0';
    output.insert(0, string_format("%s:  ", tm));
    output.append("\n");
    fwrite(output.c_str(), sizeof(char), output.size(), _file);
    fflush(_file);
    fwrite(output.c_str(), sizeof(char), output.size(), stdout);
    fflush(stdout);
}