#include <iostream>
#include "agent.h"
#include "commander.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <number_of_drones> <udp_connection_url>" << std::endl;
        std::cout << "Example: " << argv[0] << " 3 udpin://:14540" << std::endl;
        return 1;
    }

    int num_drones = std::stoi(argv[1]);
    std::string connection_url = argv[2];

    Agent* agent = Agent::getInstance();
    agent->init(num_drones, connection_url);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    double north = 47.3980;
    double east = 8.5350;
    double south = 47.3870;
    double west = 8.5440;
    double maks_altitude = 20.0f;
    double min_altitude = 10.0f;

    for (auto& uav : agent->uavs()) {
        if (uav.cmd) {
            uav.cmd->setNavigationArea(north, east, south, west, maks_altitude, min_altitude);
            uav.cmd->takeoff();

            // Wait for the drone to be in Hold mode after takeoff
            while (uav.tlm->flight_mode() != mavsdk::Telemetry::FlightMode::Hold) {
                std::cout << "Waiting for drone to enter Hold mode..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            uav.cmd->startMission();
        }
    }

    std::cout << "Press Enter to stop all missions..." << std::endl;
    std::cin.get();

    for (auto& uav : agent->uavs()) {
        if (uav.cmd) {
            uav.cmd->stopMission();
        }
    }

    return 0;
}