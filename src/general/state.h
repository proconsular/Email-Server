//
// Created by Chris Luttio on 3/25/21.
//

#ifndef P8_WEB_SERVER_STATE_H
#define P8_WEB_SERVER_STATE_H

#define MYSQLPP_MYSQL_HEADERS_BURIED

#include "task_scheduler.h"
#include "connection.h"
#include "client_request.h"
#include "http/http_request_envelope.h"
#include "http/http_response_envelope.h"
#include "configuration.h"
#include "http/http_request_carrier.h"

#include <vector>
#include <objects/general/authenicator.h>
#include "objects/general/route.h"
#include "objects/general/access_profile.h"
#include "objects/general/user_account.h"
#include "objects/general/web_socket_message.h"
#include "objects/general/session.h"
#include "objects/mail/email.h"
#include "objects/mail/dkim_signer.h"
#include "objects/general/authenicator.h"

#include <mysql++/mysql++.h>

#include "state/mail_state.h"

class State {
public:
    State() {
        scheduler = std::make_shared<TaskScheduler>();
        config = std::make_shared<Configuration>();
        ssl_enabled = false;

        AccessProfile public_access("public");
        public_access.allowed_methods = {"GET"};
        public_access.request_size_limit = 10 * 1024;

        access_level = "public";
        access_profiles["public"] = public_access;

        mail = std::make_unique<MailState>();
    }

    std::unique_ptr<Authenticator> authenticator;
    std::unique_ptr<mysqlpp::Connection> database;
    std::shared_ptr<UserAccount> unknown_account;

    const SSL_METHOD *ssl_method;
    SSL_CTX *ssl_context;

    Socket server_socket;
    Socket tls_socket;

    std::map<std::string, std::shared_ptr<Connection>> connections;
    std::map<std::string, std::shared_ptr<ClientRequest>> requests;

    std::map<std::string, std::shared_ptr<HTTPRequestCarrier>> active_requests;

    std::vector<HTTPRequestEnvelope> inbound_http_request_queue;
    std::vector<HTTPResponseEnvelope> outbound_http_response_queue;

    std::shared_ptr<TaskScheduler> scheduler;

    std::shared_ptr<Configuration> config;
    std::vector<Route> routes;
    std::map<std::string, AccessProfile> access_profiles;

    std::vector<std::shared_ptr<UserAccount>> accounts;

    std::string access_level;

    std::vector<WebSocketMessage> inbound_web_socket_messages;
    std::vector<WebSocketMessage> outbound_web_socket_messages;

    std::map<std::string, std::shared_ptr<Session>> sessions;

    std::unique_ptr<MailState> mail;

    bool ssl_enabled;

    AccessProfile get_access_profile() {
        return access_profiles[access_level];
    }

    std::shared_ptr<UserAccount> get_user_from_token(const std::string& token) {
        for (const auto& account : accounts) {
            for (const auto& auth : account->authorizations) {
                if (auth.access_token == token)
                    return account;
            }
        }
        return nullptr;
    }
};


#endif //P8_WEB_SERVER_STATE_H
