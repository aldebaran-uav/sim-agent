#pragma once

#ifndef AGENT_H
#define AGENT_H

#include "commander.h"
#include "telemetry_manager.h"
#include "server_manager.h"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/action/action.h>

#include <iostream>
#include <memory>
#include <vector>
#include <thread>

using namespace mavsdk;

typedef struct {
    int team_number;
    std::shared_ptr<Mavsdk> mav;
    std::shared_ptr<System> sys;
    std::shared_ptr<Telemetry> tlm;
    std::shared_ptr<Action> act;
    std::unique_ptr<Commander> cmd;
    std::unique_ptr<TelemetryManager> tlm_mgr;
    std::shared_ptr<ServerManager> srv_mgr;
} uav;

class Agent 
{
public:
    static Agent* getInstance();
    void init(int uav_count, const std::string& start_url, const std::string& server_url);
    Agent();
    ~Agent();

    std::vector<uav>& uavs() { return m_uavs; }

private:
    static Agent* instance;
    int m_uav_count;
    std::vector<uav> m_uavs;
    std::vector<std::thread> m_threads;
};

#endif // AGENT_H