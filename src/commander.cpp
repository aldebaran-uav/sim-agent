#include "commander.h"

#include <random>
#include <iostream>
#include <cmath>
#include <future>

Commander::Commander(std::shared_ptr<mavsdk::Mission> mission,
                     std::shared_ptr<mavsdk::Action> action,
                     std::shared_ptr<mavsdk::Telemetry> telemetry)
    : m_mission(mission), m_action(action), m_telemetry(telemetry) {}

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

void Commander::createMission()
{   
    if (!m_mission) {
        return;
        std::cerr << "Mission plugin not initialized." << std::endl;
    }

    m_mission->set_return_to_launch_after_mission(true);
    int num_waypoints = 5;

    for (int i = 0; i < num_waypoints; ++i) {
        mavsdk::Mission::MissionItem item;
        item.latitude_deg = m_lat_dist(m_gen);
        item.longitude_deg = m_lon_dist(m_gen);
        item.relative_altitude_m = m_min_altitude;
        item.speed_m_s = 5.0f;
        item.is_fly_through = true;
        item.acceptance_radius_m = 1.0f;
        item.yaw_deg = 0.0f;
        item.vehicle_action = mavsdk::Mission::MissionItem::VehicleAction::None;
        mission_plan.mission_items.push_back(item);
    }

    mavsdk::Mission::MissionItem landingPoint;
    landingPoint.latitude_deg = m_lat_dist(m_gen);
    landingPoint.longitude_deg = m_lon_dist(m_gen);
    landingPoint.relative_altitude_m = m_min_altitude;
    landingPoint.speed_m_s = 5.0f;
    landingPoint.is_fly_through = false;
    landingPoint.loiter_time_s = NAN;
    landingPoint.acceptance_radius_m = 1.0f;
    landingPoint.yaw_deg = 0.0f;
    landingPoint.vehicle_action = mavsdk::Mission::MissionItem::VehicleAction::Land;

    mission_plan.mission_items.push_back(landingPoint);
}

void Commander::uploadMission()
{
    if(!m_mission) {
        std::cerr << "Mission plugin not initialized." << std::endl;
        return;
    }

    if (m_mission->clear_mission() != mavsdk::Mission::Result::Success) {
        std::cerr << "Failed to clear mission." << std::endl;
        return;
    }

    if (m_mission->upload_mission(mission_plan) != mavsdk::Mission::Result::Success) {
        std::cerr << "Failed to upload mission." << std::endl;
        return;
    }

    std::cout << "Mission uploaded." << std::endl;
}

void Commander::startMission() {
    if (!m_mission) {
        std::cerr << "Mission plugin not initialized." << std::endl;
        return;
    }

    this->createMission();
    this->uploadMission();
    this->takeoff();
    
    while(!m_telemetry->in_air() && m_telemetry->flight_mode() != mavsdk::Telemetry::FlightMode::Hold) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // if (m_mission->start_mission() != mavsdk::Mission::Result::Success) {
    //     std::cerr << "Failed to start mission." << std::endl;
    //     std::cerr << "Current flight mode: " << m_telemetry->flight_mode() << std::endl;
    //     std::cerr << "Is armed: " << m_telemetry->armed() << std::endl;
    //     std::cerr << "Is in air: " << m_telemetry->in_air() << std::endl;
    //     return;
    // }

    m_mission->start_mission_async([this](mavsdk::Mission::Result result) {
        if(result == mavsdk::Mission::Result::Success) {
            std::cout << "Görev başlatıldı";
            return;
        }
        std::cout << "Görev başlatılamadı : " << result;
    });
}

void Commander::stopMission() {
    if (!m_mission) {
        std::cerr << "Mission plugin not initialized." << std::endl;
        return;
    }

    if (m_mission->pause_mission() != mavsdk::Mission::Result::Success) {
        std::cerr << "Failed to pause mission." << std::endl;
        return;
    }

    if (m_mission->clear_mission() != mavsdk::Mission::Result::Success) {
        std::cerr << "Failed to clear mission." << std::endl;
        return;
    }

    std::cout << "Mission stopped." << std::endl;
}
