#include <iostream>
#include "agent.h"
#include "commander.h"
#include <thread>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <number_of_drones> <udp_connection_url>" << std::endl;
        std::cout << "Example: " << argv[0] << " 4 udpin://:14540" << std::endl;
        return 1;
    }

    int num_drones;
    try {
        num_drones = std::stoi(argv[1]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Invalid number of drones: " << argv[1] << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "Number of drones out of range: " << argv[1] << std::endl;
        return 1;
    }
    std::string connection_url = argv[2];

    Agent* agent = Agent::getInstance();
    agent->init(num_drones, connection_url);

    // waiting time to ensure all drones are connected
    while (agent->uavs().size() < num_drones) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    double north = 47.398321;
    double east = 8.556129;
    double south = 47.397400;
    double west = 8.546417;
    double maks_altitude = 25.0f;
    double min_altitude = 20.0f;

    std::vector<std::thread> threads;

    for (auto& uav : agent->uavs()) {
        threads.emplace_back([&uav, north, east, south, west, maks_altitude, min_altitude]() {
            while (uav.cmd == nullptr) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

            uav.cmd->setNavigationArea(north, east, south, west, maks_altitude, min_altitude);
            uav.cmd->takeoff();

            // Wait for the drone to be in Hold mode after takeoff
            auto start_time = std::chrono::steady_clock::now();
            while (uav.tlm->flight_mode() != mavsdk::Telemetry::FlightMode::Hold) {
                std::cout << "Waiting for drone to enter Hold mode..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                auto current_time = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
                if (duration > 30) { // Timeout after 30 seconds
                    std::cerr << "Timeout waiting for Hold mode." << std::endl;
                    break;
                }
            }

            uav.cmd->startMission();
        });
    }

    std::cout << "Press Enter to stop all missions..." << std::endl;
    std::cin.get();

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    for (auto& uav : agent->uavs()) {
        if (uav.cmd) {
            uav.cmd->stopMission();
        }
    }

    return 0;
}