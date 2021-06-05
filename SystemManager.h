#pragma once
#include <Arduino.h>
enum class compass_status_t
{
    OFFLINE,
    CALIBRATING,
    OK,
    FAIL
};

enum class gps_status_t
{
    OFFLINE,
    OK,
    FAIL
};

enum class servo_status_t
{
    OFFLINE,
    OK,
    FAIL
};

enum class system_status_t
{
    OFFLINE,
    OK,
    FAIL
};




class SystemManager
{
private:
    compass_status_t    compass_status;
    gps_status_t        gps_status;
    servo_status_t      servo_status;
    system_status_t     system_status;

    uint gps_offline_counter;
    uint gps_offline_th;
    const uint GPS_OFFLINE_TH_DEFAULT = 100;

    uint system_offline_counter;
    uint system_offline_th;
    const uint SYS_OFFLINE_TH_DEFAULT = 100;

public:
    SystemManager();
    ~SystemManager();

    void update_compass_status(compass_status_t cs);
    void update_gps_status(gps_status_t gs);
    void update_servo_status(servo_status_t ss);

    void set_gps_offline_th(uint th);


    system_status_t get_system_status();

    compass_status_t    get_compass_status();
    gps_status_t        get_gps_status();
    servo_status_t      get_servo_status();

    void get_status(system_status_t & sysS, compass_status_t & cs, gps_status_t &gs, servo_status_t &ss);




};

