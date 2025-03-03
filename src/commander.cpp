#include "commander.h"

#include <random>
#include <iostream>
#include <cmath>
#include <future>

Commander::Commander(std::shared_ptr<mavsdk::Action> action,
                     std::shared_ptr<mavsdk::Telemetry> telemetry)
    : m_action(action), m_telemetry(telemetry) {}

Commander::~Commander() 
{
    stopMission();
    if (m_missionThread.joinable()) {
        m_missionThread.join();
    }
}

void Commander::setNavigationArea(const navigation_area &nav_area) 
{
    m_max_altitude = nav_area.max_altitude;
    m_min_altitude = nav_area.min_altitude;
    
    m_north = nav_area.north;
    m_east = nav_area.east;
    m_south = nav_area.south;
    m_west = nav_area.west;

    m_lat_dist = std::uniform_real_distribution<>(m_south, m_north);
    m_lon_dist = std::uniform_real_distribution<>(m_west, m_east);
}

void Commander::takeoff()
{
    if (m_action) {
        std::cout << "Arming..." << std::endl;
        auto arm_result = m_action->arm();
        if (arm_result != mavsdk::Action::Result::Success) {
            std::cerr << "Arming failed: " << arm_result << std::endl;
            return;
        }
        std::cout << "Taking off..." << std::endl;
        const auto result = m_action->takeoff();
        if (result != mavsdk::Action::Result::Success) {
            std::cerr << "Takeoff failed: " << result << std::endl;
        }

        while(true) {
            bool in_air = m_telemetry->in_air();
            mavsdk::Telemetry::FlightMode flight_mode = m_telemetry->flight_mode();
            if (in_air && flight_mode == mavsdk::Telemetry::FlightMode::Hold) {
                std::cout << "Drone is in the air." << std::endl;
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    
    } else {
        std::cerr << "Action plugin not initialized." << std::endl;
    }
}

void Commander::startMission()
{
    if (m_missionRunning) {
        std::cout << "Mission already running." << std::endl;
        return;
    }

    m_missionRunning = true;

    while (m_telemetry->flight_mode() != mavsdk::Telemetry::FlightMode::Hold) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    m_missionThread = std::thread(&Commander::runMission, this);
}

void Commander::stopMission()
{
    if (m_missionRunning) {
        m_missionRunning = false;
        if (m_missionThread.joinable()) {
            m_missionThread.join();
        }
    } else {
        std::cout << "Mission not running." << std::endl;
    }
}

void Commander::runMission()
{
    std::uniform_real_distribution<> alt_dist(m_min_altitude, m_max_altitude);

    while (m_missionRunning) {
        // Generate random waypoint within the defined area
        double latitude = m_lat_dist(m_gen);
        double longitude = m_lon_dist(m_gen);
        
        // Avoid division by zero
        float altitude;
        if (m_max_altitude > m_min_altitude) {
            altitude = alt_dist(m_gen);
        } else {
            altitude = m_min_altitude;
            std::cerr << "Max altitude equals min altitude. Setting altitude to min altitude." << std::endl;
        }

        std::cout << "Navigating to waypoint: " << latitude << ", " << longitude << ", " << altitude << std::endl;
        navigateToWaypoint(latitude, longitude, altitude);

        if (m_telemetry->in_air() && m_telemetry->flight_mode() == mavsdk::Telemetry::FlightMode::Hold) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        } else {
            std::cerr << "Drone not in the air." << std::endl;
            break;
        }
    }
    std::cout << "Mission stopped." << std::endl;
}

void Commander::navigateToWaypoint(double latitude, double longitude, float altitude)
{
    if (m_action) {
        auto goto_result = m_action->goto_location(latitude, longitude, altitude, 0.0f);
        if (goto_result != mavsdk::Action::Result::Success) {
            std::cerr << "Failed to navigate to waypoint: " << goto_result << std::endl;
            return;
        }

        std::cout << "Navigating to waypoint: " << latitude << ", " << longitude << " alt : " << altitude << std::endl;

        // Define a tolerance for considering the waypoint reached (in meters)
        constexpr double tolerance = 5.0;

        while (m_missionRunning) {
            // Get current position from telemetry
            const auto position = m_telemetry->position();
            double current_latitude = position.latitude_deg;
            double current_longitude = position.longitude_deg;
            float current_altitude = position.absolute_altitude_m;

            double distance = std::sqrt(std::pow((latitude - current_latitude) * 111111, 2) + // Approximate meters per degree latitude
                                        std::pow((longitude - current_longitude) * 111111 * std::cos(latitude * M_PI / 180.0), 2) + // Approximate meters per degree longitude
                                        std::pow(altitude - current_altitude, 2));

            if (distance <= tolerance) {
                std::cout << "Reached waypoint." << std::endl;
                break;
            }

            if (!m_telemetry->in_air() || m_telemetry->flight_mode() != mavsdk::Telemetry::FlightMode::Hold) {
                std::cerr << "Drone not in the air or not in hold mode." << std::endl;
                return;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }    
    } else {
        std::cerr << "Action plugin not initialized." << std::endl;
    }
}
