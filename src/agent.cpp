#include "agent.h"
#include <sstream>
#include <chrono>
#include <thread>

using namespace mavsdk;

Agent* Agent::instance = nullptr;

Agent::Agent() {}

Agent::~Agent() 
{
    for (auto& thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

Agent* Agent::getInstance()
{
    if (instance == nullptr) {
        instance = new Agent();
    }
    return instance;
}

void Agent::init(int uav_count, const std::string& start_url, 
                 const std::string& server_url, navigation_area& nav_area) 
{
    if (instance == nullptr) {
        instance = new Agent();
    }

    instance->m_uav_count = uav_count;
    instance->m_uavs.resize(uav_count);
    instance->m_nav_area = nav_area;

    m_threads.clear();
    for (int i = 0; i < uav_count; ++i) {
        m_threads.emplace_back([this, i, start_url, server_url]() {
            m_uavs[i].mav = std::make_shared<Mavsdk>(Mavsdk::Configuration(ComponentType::GroundStation));

            std::string connection_str = start_url;
            if (connection_str.find("udp://") == 0) {
                connection_str = connection_str.replace(0, 6, "udpin://");
            }

            std::ostringstream url;
            url << connection_str.substr(0, connection_str.find_last_of(':') + 1) << 14540 + i;
            std::cout << "Connecting to " << url.str() << std::endl;

            ConnectionResult connection_result = m_uavs[i].mav->add_any_connection(url.str());
            if (connection_result != ConnectionResult::Success) {
                std::cerr << "Failed to connect to " << url.str() << std::endl;
                return;
            }

            // Wait for a system to be available.
            std::shared_ptr<System> system = nullptr;
            constexpr int max_attempts = 15;
            for (int j = 0; j < max_attempts; ++j) {
                auto systems = m_uavs[i].mav->systems();
                if (!systems.empty()) {
                    system = systems.at(0);
                    std::cout << "System found for UAV " << i << std::endl;
                    break;
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            if (!system) {
                std::cerr << "No system found for UAV " << i << std::endl;
                return;
            }
            
            m_uavs[i].team_number = i; 
            m_uavs[i].sys = system;
            m_uavs[i].tlm = std::make_shared<Telemetry>(m_uavs[i].sys);
            m_uavs[i].act = std::make_shared<Action>(m_uavs[i].sys);
            m_uavs[i].cmd = std::make_unique<Commander>(m_uavs[i].act, m_uavs[i].tlm);
            m_uavs[i].srv_mgr = std::make_unique<ServerManager>(server_url);
            m_uavs[i].tlm_mgr = std::make_unique<TelemetryManager>(i, m_uavs[i].tlm, m_uavs[i].srv_mgr);

            m_uavs[i].tlm_mgr->start();

            m_uavs[i].cmd->setNavigationArea(instance->m_nav_area);
            m_uavs[i].cmd->startMission();
        });
    }
}