#include "telemetry_manager.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <thread>
#include <mutex>
#include <iostream>
#include <string>

using json = nlohmann::json;           

TelemetryManager::TelemetryManager(int id, std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<ServerManager> server_manager) 
    : m_id(id), m_telemetry(telemetry), m_server_manager(server_manager) {}

TelemetryManager::~TelemetryManager() {}

void TelemetryManager::start()
{
    m_running = true;
    m_server_manager->init();

    // Start a thread to send telemetry data to the server every second.
    std::thread([this]() {
        while (m_running) {
            m_telemetry_data.takim_numarasi = m_id;
            m_telemetry_data.iha_enlem = m_telemetry->position().latitude_deg;
            m_telemetry_data.iha_boylam = m_telemetry->position().longitude_deg;
            m_telemetry_data.iha_irtifa = m_telemetry->position().absolute_altitude_m;
            m_telemetry_data.iha_dikelme = m_telemetry->attitude_euler().pitch_deg;
            m_telemetry_data.iha_yonelme = m_telemetry->attitude_euler().yaw_deg;
            m_telemetry_data.iha_yatis = m_telemetry->attitude_euler().roll_deg;

            float vx, vy, vz;
            vx = m_telemetry->velocity_ned().north_m_s;
            vy = m_telemetry->velocity_ned().east_m_s;
            vz = m_telemetry->velocity_ned().down_m_s;

            m_telemetry_data.iha_hiz = sqrt(vx * vx + vy * vy + vz * vz);
            m_telemetry_data.iha_batarya = m_telemetry->battery().remaining_percent;
            m_telemetry_data.iha_otonom = m_telemetry->flight_mode() != mavsdk::Telemetry::FlightMode::Manual;
            m_telemetry_data.iha_kilitlenme = 0;
            m_telemetry_data.iha_hedef_merkez_X = 0;
            m_telemetry_data.iha_hedef_merkez_Y = 0;
            m_telemetry_data.hedef_genislik = 0;
            m_telemetry_data.hedef_yukseklik = 0;
            m_telemetry_data.gps_saati = m_telemetry->unix_epoch_time();

            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Sleep for 1 second.
            sendTelemetry(); 
        }
    }).detach();
}

void TelemetryManager::sendTelemetry() {
    json telemetry_data = {
        {"takim_numarasi", m_telemetry_data.takim_numarasi},
        {"iha_enlem", m_telemetry_data.iha_enlem},
        {"iha_boylam", m_telemetry_data.iha_boylam},
        {"iha_irtifa", m_telemetry_data.iha_irtifa},
        {"iha_dikelme", m_telemetry_data.iha_dikelme},
        {"iha_yonelme", m_telemetry_data.iha_yonelme},
        {"iha_yatis", m_telemetry_data.iha_yatis},
        {"iha_hiz", m_telemetry_data.iha_hiz},
        {"iha_batarya", m_telemetry_data.iha_batarya},
        {"iha_otonom", m_telemetry_data.iha_otonom},
        {"iha_kilitlenme", m_telemetry_data.iha_kilitlenme},
        {"iha_hedef_merkez_X", m_telemetry_data.iha_hedef_merkez_X},
        {"iha_hedef_merkez_Y", m_telemetry_data.iha_hedef_merkez_Y},
        {"hedef_genislik", m_telemetry_data.hedef_genislik},
        {"hedef_yukseklik", m_telemetry_data.hedef_yukseklik},
        {"gps_saati", m_telemetry_data.gps_saati}
    };

    m_server_manager->post(telemetry_data, "/api/telemetri_gonder");
}

void TelemetryManager::stop()
{
    m_running = false;
}