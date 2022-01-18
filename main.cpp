
#define MYSQLPP_MYSQL_HEADERS_BURIED

#include <string>

#include "tasks/http/server/reception_task.h"
#include "tasks/http/server/initialize_server_task.h"
#include "tasks/http/server/receive_requests_task.h"
#include "tasks/http/server/prune_connections_task.h"
#include "tasks/http/server/process_http_requests_task.h"
#include "tasks/http/server/initialize_client_requests_task.h"
#include "tasks/http/server/send_responses_task.h"
#include "tasks/http/server/finalize_client_requests_task.h"
#include "tasks/load_configuration_task.h"
#include "tasks/http/client/initialize_http_request_connections_task.h"
#include "tasks/http/client/receive_http_responses_task.h"
#include "tasks/http/client/send_http_requests_task.h"
#include "tasks/http/server/load_routing_data_task.h"
#include "tasks/http/server/tls_reception_task.h"
#include "tasks/http/server/initialize_tls_server_socket_task.h"
#include "tasks/web_sockets/send_web_socket_messages_task.h"
#include "tasks/web_sockets/recieve_web_socket_messages_task.h"

#include "controllers/direct_controller.h"
#include "receivers/log_action_receiver.h"

#include "general/state.h"
#include "udns.h"

#include "tasks/mail/initialize_smtp_server_task.h"
#include "tasks/mail/smtp_reception_task.h"
#include "tasks/mail/receive_email_task.h"
#include "tasks/mail/store_email_task.h"
#include "tasks/database/initialize_database_connection_task.h"
#include "tasks/database/load_stored_data_task.h"

#include "tasks/mail/categorize_outbound_emails_task.h"
#include "tasks/mail/resolve_mail_connection_task.h"
#include "tasks/mail/send_email_task.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sys/stat.h>
#include "objects/mail/dkim_signer.h"

#include <mysql++/mysql++.h>

#include <csignal>

int main() {
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();

    signal(SIGPIPE, SIG_IGN);

    mkdir("emails", 777);

    dns_init(nullptr, 0);

    auto state = std::make_shared<State>();
    auto controller = std::make_shared<DirectController>(state);
    controller->add_receiver(std::make_shared<LogActionReceiver>("log.txt"));

    state->authenticator = std::make_unique<Authenticator>(state);

    controller->apply(Action(StartApp, std::make_shared<std::string>("Mail Server")));

    srand(get_time().time_since_epoch().count());

    state->scheduler->add(std::make_shared<LoadConfigurationTask>(state, controller));
    state->scheduler->add(std::make_shared<LoadRoutingDataTask>(state));
    state->scheduler->add(std::make_shared<InitializeDatabaseConnectionTask>(state, controller));
    state->scheduler->add(std::make_shared<LoadStoredDataTask>(state));
    state->scheduler->add(std::make_shared<InitializeServerTask>(state, controller));
    state->scheduler->add(std::make_shared<InitializeTLSServerSocketTask>(state, controller));
    state->scheduler->add(std::make_shared<InitializeSmtpServerTask>(state, controller));
    state->scheduler->add(std::make_shared<ReceptionTask>(state, controller));
    state->scheduler->add(std::make_shared<TLSReceptionTask>(state, controller));
    state->scheduler->add(std::make_shared<SmtpReceptionTask>(state, controller));
    state->scheduler->add(std::make_shared<ReceiveEmailTask>(state, controller));
    state->scheduler->add(std::make_shared<PruneConnectionsTask>(state, controller));
    state->scheduler->add(std::make_shared<ReceiveRequestsTask>(state, controller));
    state->scheduler->add(std::make_shared<ProcessHTTPRequestsTask>(state, controller));
    state->scheduler->add(std::make_shared<InitializeClientRequestsTask>(state, controller));
    state->scheduler->add(std::make_shared<FinalizeClientRequestsTask>(state, controller));
    state->scheduler->add(std::make_shared<SendResponsesTask>(state, controller));
    state->scheduler->add(std::make_shared<InitializeHTTPRequestConnectionsTask>(state, controller));
    state->scheduler->add(std::make_shared<SendHTTPRequestsTask>(state, controller));
    state->scheduler->add(std::make_shared<ReceiveHTTPResponsesTask>(state, controller));
    state->scheduler->add(std::make_shared<SendWebSocketMessagesTask>(state));
    state->scheduler->add(std::make_shared<RecieveWebSocketMessagesTask>(state));
    state->scheduler->add(std::make_shared<CategorizeOutboundEmailsTask>(state));
    state->scheduler->add(std::make_shared<ResolveMailConnectionTask>(state));
    state->scheduler->add(std::make_shared<SendEmailTask>(state, controller));
    state->scheduler->add(std::make_shared<StoreEmailTask>(state, controller));

    state->scheduler->run();

    return 0;
}
