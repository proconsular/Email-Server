//
// Created by Chris Luttio on 4/5/21.
//

#include "state_action_receiver.h"

void StateActionReceiver::receive(const Action &action) {
    switch (action.type) {
        case SetConfiguration: {
            auto config = std::static_pointer_cast<Configuration>(action.data);
            _state->config = config;
            break;
        }
        case CreateServerSocket: {
            auto socket = std::static_pointer_cast<Socket>(action.data);
            _state->server_socket = *socket;
            break;
        }
        case CreateTLSServerSocket: {
            auto socket = std::static_pointer_cast<Socket>(action.data);
            _state->tls_socket = *socket;
            break;
        }
        case CreateHttpRequest: {
            auto envelope = std::static_pointer_cast<HTTPRequestEnvelope>(action.data);
            _state->inbound_http_request_queue.push_back(*envelope);
            break;
        }
        case CreateClientConnection:
        case ModifyClientConnection: {
            auto connection = std::static_pointer_cast<Connection>(action.data);
            _state->connections[connection->id()] = connection;
            break;
        }
        case RemoveClientConnection: {
            auto connection = std::static_pointer_cast<Connection>(action.data);
            _state->connections.erase(connection->id());
            close(connection->socket.id);
            break;
        }
        case CreateClientRequest:
        case ModifyClientRequest: {
            auto request = std::static_pointer_cast<ClientRequest>(action.data);
            _state->requests[request->id()] = request;
            break;
        }
        case RemoveClientRequest: {
            auto request = std::static_pointer_cast<ClientRequest>(action.data);
            _state->requests.erase(request->id());
            break;
        }
        case CreateHttpResponse: {
            auto response = std::static_pointer_cast<HTTPResponseEnvelope>(action.data);
            _state->outbound_http_response_queue.push_back(*response);
            break;
        }
        case ClearHttpResponses: {
            for (const auto& response: _state->outbound_http_response_queue) {
                response.connection->active_requests--;
            }
            _state->outbound_http_response_queue.clear();
            break;
        }
        case ClearHttpRequests: {
            _state->inbound_http_request_queue.clear();
            break;
        }
        case CreateOutboundHttpRequest:
        case InitializeHttpRequestConnection:
        case SendHttpRequest:
        case ReceiveHttpResponse:
        case ModifyHttpCarrier: {
            auto carrier = std::static_pointer_cast<HTTPRequestCarrier>(action.data);
            _state->active_requests[carrier->id()] = carrier;
            break;
        }
        default:
            break;
    }
}