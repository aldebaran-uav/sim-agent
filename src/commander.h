#pragma once

#ifndef COMMANDER_H
#define COMMANDER_H

#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/action/action.h>

#include <atomic>
#include <thread>
#include <memory>
#include <random>

class Commander {
public:
    Commander(std::shared_ptr<mavsdk::Action> action,
              std::shared_ptr<mavsdk::Telemetry> telemetry);
    ~Commander();

    void takeoff();
    void land(); // Leaving this for consistency, even if unimplemented
    void startMission();
    void stopMission();

    // Set the navigation area boundaries
    void setNavigationArea(double north, double east, double south, double west, double max_altitude, double min_altitude);

private:
    std::shared_ptr<mavsdk::Action> m_action;
    std::shared_ptr<mavsdk::Telemetry> m_telemetry;
    std::atomic<bool> m_missionRunning{false};
    std::thread m_missionThread;

    // Navigation area boundaries
    double m_north{0.0};
    double m_east{0.0};
    double m_south{0.0};
    double m_west{0.0};
    double m_max_altitude{0.0};
    double m_min_altitude{0.0};

    // Random number generation
    std::random_device m_rd;
    std::mt19937 m_gen{m_rd()};
    std::uniform_real_distribution<> m_lat_dist;
    std::uniform_real_distribution<> m_lon_dist;

    void runMission();
    void navigateToWaypoint(double latitude, double longitude, float altitude);
};

#endif // COMMANDER_H
