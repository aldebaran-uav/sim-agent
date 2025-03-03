#pragma once

#ifndef SERVER_MANAGER_H
#define SERVER_MANAGER_H

#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <string>

class ServerManager
{
public:
    ServerManager(std::string server_url);
    ~ServerManager();

    void init();
    void post(const nlohmann::json& data, const std::string& endpoint);
    void get(const std::string& endpoint);

private:
    CURL* m_curl;
    std::string m_server_url;
};

#endif // SERVER_MANAGER_H