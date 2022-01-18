//
// Created by Chris Luttio on 6/23/21.
//

#include "initialize_database_connection_task.h"
#include <mysql++/mysql++.h>

void InitializeDatabaseConnectionTask::perform() {
    mysqlpp::Connection connection;
    if (connection.connect("mail", "mail.drade.io", "manager", "&1Athena", 3306)) {
        _state->database = std::make_unique<mysqlpp::Connection>(connection);
        setup_tables();
        _controller->apply(Action(Print, std::make_shared<std::string>("INIT: MySQL Database Connection")));
    }
    _alive = false;
}

void InitializeDatabaseConnectionTask::setup_tables() {
    _state->database->query(
            "CREATE TABLE IF NOT EXISTS accounts ("
            "id INT NOT NULL AUTO_INCREMENT, "
            "username VARCHAR(255), "
            "password VARCHAR(255), "
            "role VARCHAR(255), "
            "access_token VARCHAR(255), "
            "refresh_token VARCHAR(255),"
            "PRIMARY KEY(id)"
            ")").exec();
    _state->database->query(
            "CREATE TABLE IF NOT EXISTS emails ("
            "id INT NOT NULL AUTO_INCREMENT,"
            "name VARCHAR(255),"
            "sender VARCHAR(255),"
            "status VARCHAR(255),"
            "created_at DATETIME,"
            "account_id INT,"
            "PRIMARY KEY(id),"
            "FOREIGN KEY (account_id) "
            "REFERENCES accounts(id) "
            "ON DELETE CASCADE"
            ")").exec();
    _state->database->query(
            "CREATE TABLE IF NOT EXISTS labels ("
            "id INT NOT NULL AUTO_INCREMENT,"
            "name VARCHAR(255),"
            "PRIMARY KEY(id)"
            ")").exec();
    _state->database->query(
            "CREATE TABLE IF NOT EXISTS receive_rules ("
            "id INT NOT NULL AUTO_INCREMENT, "
            "`match` VARCHAR(255), "
            "account_id INT, "
            "PRIMARY KEY(id), "
            "FOREIGN KEY (account_id) "
            "REFERENCES accounts(id) "
            "ON DELETE CASCADE"
            ")").exec();
    _state->database->query(
            "CREATE TABLE IF NOT EXISTS receive_rule_labels ("
            "id INT NOT NULL AUTO_INCREMENT,"
            "rule_id INT,"
            "label_id INT,"
            "PRIMARY KEY(id),"
            "FOREIGN KEY (rule_id) "
            "REFERENCES receive_rules(id) "
            "ON DELETE CASCADE, "
            "FOREIGN KEY (label_id) "
            "REFERENCES labels(id) "
            "ON DELETE CASCADE"
            ")").exec();
    _state->database->query(
            "CREATE TABLE IF NOT EXISTS email_labels ("
            "id INT NOT NULL AUTO_INCREMENT,"
            "email_id INT,"
            "label_id INT,"
            "PRIMARY KEY(id),"
            "FOREIGN KEY (email_id) "
            "REFERENCES emails(id) "
            "ON DELETE CASCADE, "
            "FOREIGN KEY (label_id) "
            "REFERENCES labels(id) "
            "ON DELETE CASCADE"
            ")").exec();
}