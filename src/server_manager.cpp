#include "server_manager.h"
#include <iostream>

ServerManager::ServerManager(std::string server_url) 
    : m_server_url(server_url) {}

ServerManager::~ServerManager() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
    }
    curl_global_cleanup();
}

void ServerManager::init()
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_curl = curl_easy_init();
}

void ServerManager::post(const nlohmann::json& data, const std::string& endpoint)
{
    if (!m_curl) {
        return;
    }

    std::string url = m_server_url + endpoint;
    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.dump().c_str());
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, data.dump().size());

    curl_easy_perform(m_curl);
}

void ServerManager::get(const std::string& endpoint)
{
    if (!m_curl) {
        return;
    }

    std::string url = m_server_url + endpoint;
    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1L);

    curl_easy_perform(m_curl);
}