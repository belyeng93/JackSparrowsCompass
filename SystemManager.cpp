#include "SystemManager.h"
SystemManager::SystemManager(/* args */)
{
    compass_status  = compass_status_t::OFFLINE;
    gps_status      = gps_status_t::OFFLINE;
    servo_status    = servo_status_t::OFFLINE;
    system_status   = system_status_t::OFFLINE;
    
    gps_offline_counter = 0;
    gps_offline_th = GPS_OFFLINE_TH_DEFAULT;

    system_offline_counter = 0;
    system_offline_th = SYS_OFFLINE_TH_DEFAULT;

}

SystemManager::~SystemManager(){}
void SystemManager::update_compass_status(compass_status_t cs)
{
    this->compass_status = cs;
}
void SystemManager::update_gps_status(gps_status_t gs)
{
    if(gs == gps_status_t::OFFLINE)
    {
        gps_offline_counter++;
        // Serial.println(gps_offline_counter);
        if(gps_offline_counter >= gps_offline_th)
        {
            this->gps_offline_counter = 0;
            // Serial.println("OFFLINE");
            this->gps_status = gps_status_t::OFFLINE;
        }
    }
    else
    {
        // Serial.println("OK");
        this->gps_offline_counter = 0;
        this->gps_status = gs;
    }

}
void SystemManager::update_servo_status(servo_status_t ss)
{
    this->servo_status = ss;
}
system_status_t SystemManager::get_system_status()
{
    if( compass_status == compass_status_t::OK && 
        gps_status == gps_status_t::OK &&
        servo_status == servo_status_t::OK)
    {
        this->system_offline_counter = 0;
        this->system_status = system_status_t::OK;
    }
    else
    {
        if( 
            compass_status == compass_status_t::FAIL || 
            gps_status == gps_status_t::FAIL ||
            servo_status == servo_status_t::FAIL)
        {
            this->system_status = system_status_t::FAIL;
        }
        else
        {
            system_offline_counter++;
            if(system_offline_counter >= system_offline_th)
            {
                this->system_offline_counter = 0;
                this->system_status = system_status_t::OFFLINE;
            }
        }
    }
    return system_status;
}
compass_status_t SystemManager::get_compass_status()
{
    return compass_status;
}
gps_status_t SystemManager::get_gps_status()
{
    return gps_status;
}
servo_status_t SystemManager::get_servo_status()
{
    return servo_status;
}
void SystemManager::get_status(system_status_t & sysS, compass_status_t & cs, gps_status_t &gs, servo_status_t &ss)
{
    sysS    = this->system_status;
    cs      = this->compass_status;
    gs      = this->gps_status;
    ss      = this->servo_status;
}
void SystemManager::set_gps_offline_th(uint th)
{
    this->gps_offline_th = th;
}


