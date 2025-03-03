#pragma once

#ifndef TELEMETRY_MANAGER_H
#define TELEMETRY_MANAGER_H

#include <server_manager.h>

#include <mavsdk/plugins/telemetry/telemetry.h>
#include <string>

struct TelemetryData
{
    int takim_numarasi;
    double iha_enlem;
    double iha_boylam;
    float iha_irtifa;
    float iha_dikelme;
    float iha_yonelme;
    float iha_yatis;
    float iha_hiz;
    float iha_batarya;
    bool iha_otonom;
    bool iha_kilitlenme;
    float iha_hedef_merkez_X;
    float iha_hedef_merkez_Y;
    float hedef_genislik;
    float hedef_yukseklik;
    uint64_t gps_saati;
};

class TelemetryManager
{
public:
    TelemetryManager(int id, std::shared_ptr<mavsdk::Telemetry> telemetry, std::shared_ptr<ServerManager> server_manager);
    ~TelemetryManager();

    void start();
    void stop();

private:
    int m_id;
    bool m_running = true;

    std::shared_ptr<mavsdk::Telemetry> m_telemetry;
    std::shared_ptr<ServerManager> m_server_manager;
    TelemetryData m_telemetry_data;

    void sendTelemetry();
};

#endif // TELEMETRY_MANAGER_H