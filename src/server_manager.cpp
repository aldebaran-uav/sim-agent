#include "server_manager.h"

ServerManager::ServerManager(std::string server_url) 
    : m_server_url(server_url) {}

ServerManager::~ServerManager() {}

void ServerManager::init()
{
    curl_global_init(CURL_GLOBAL_ALL);
    m_curl = curl_easy_init();
}

void ServerManager::post(const nlohmann::json& data, const std::string& endpoint)
{
    if (m_curl) {
        curl_easy_setopt(m_curl, CURLOPT_URL, (m_server_url + endpoint).c_str());
        curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.dump().c_str());
        curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, data.dump().size());

        CURLcode res = curl_easy_perform(m_curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
    }
}

void ServerManager::get(const std::string& endpoint)
{
    if (m_curl) {
        curl_easy_setopt(m_curl, CURLOPT_URL, (m_server_url + endpoint).c_str());
        curl_easy_setopt(m_curl, CURLOPT_HTTPGET, 1L);

        CURLcode res = curl_easy_perform(m_curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
    }
}