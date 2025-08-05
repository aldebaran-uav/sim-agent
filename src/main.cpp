#include "agent.h"

#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <thread>

using json = nlohmann::json;

json readConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return json();
    }
    json config;
    file >> config;
    return config;
}

int main(int argc, char* argv[]) {
    json config;
    config = readConfig("config.json");
    if (config.empty()) {
        return 1;
    }

    int num_drones = config["uav_count"];
    std::string connection_url = config["mavlink_ip"];
    int start_port = config["uav_start_port"];
    std::string server_url = std::string(config["server_ip"]) + ":" 
                           + std::to_string(static_cast<int>(config["server_port"]));
    std::string full_connection_url = connection_url + ":" 
                                    + std::to_string(static_cast<int>(start_port));
    navigation_area nav_area;

    nav_area.north = 40.206761;
    nav_area.east = 25.882657;
    nav_area.south = 40.202909;
    nav_area.west = 25.885234;
    nav_area.max_altitude = 25.0;
    nav_area.min_altitude = 20.0;

    Agent* agent = Agent::getInstance();
    agent->init(num_drones, full_connection_url, server_url, nav_area);

    std::cin.get();

    for (auto& uav : agent->uavs()) {
        if (uav.cmd) {
            uav.cmd->stopMission();
        }
    }

    return 0;
}