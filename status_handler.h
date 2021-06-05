#include <Arduino.h>
#define PIN_LED_STATUS_OK     10 //sd3
#define PIN_LED_STATUS_FAIL   0  //D3
#define PIN_LED_CALIBRATION   16 //D0


bool status_server   = false;
bool status_servo    = false;
bool status_compass  = false;
bool status_gps      = false;
bool status_calibration = false;

bool led_ok = false;
bool led_calib = false;

bool status_handler = false;

void update_led_status();
void toggle_led();
void init_status_handler();

void init_status_handler()
{
    status_handler = true;
    pinMode(PIN_LED_STATUS_OK,  OUTPUT);
    pinMode(PIN_LED_STATUS_FAIL, OUTPUT);
    pinMode(PIN_LED_CALIBRATION, OUTPUT);

    digitalWrite(PIN_LED_STATUS_OK,     HIGH);
    digitalWrite(PIN_LED_STATUS_FAIL,   HIGH);
    digitalWrite(PIN_LED_CALIBRATION,   HIGH);

    delay(100);

    digitalWrite(PIN_LED_STATUS_OK,     LOW);
    digitalWrite(PIN_LED_STATUS_FAIL,   LOW);
    digitalWrite(PIN_LED_CALIBRATION,   LOW);

}


void update_led_status()
{   
    if(not status_handler)
    {
        init_status_handler();
    }
    digitalWrite(PIN_LED_STATUS_OK,     LOW);
    digitalWrite(PIN_LED_STATUS_FAIL,   HIGH);
  
    if(status_server and status_servo and status_compass and status_gps)
    {
        if(not led_ok)
        {
            led_ok = true;
            toggle_led();
        }
    }
    else
    {
        
        if(status_calibration) 
        {
            //Init Calibration
            digitalWrite(PIN_LED_CALIBRATION,     HIGH);
        }
        led_ok = false;
        if(led_ok)
        {
            toggle_led();
        }
    }
}

void toggle_led()
{
    if(led_ok)
    {
        digitalWrite(PIN_LED_STATUS_OK,     HIGH);
        digitalWrite(PIN_LED_STATUS_FAIL,   LOW);
        digitalWrite(PIN_LED_CALIBRATION,   LOW);
    }
    else
    {
        digitalWrite(PIN_LED_STATUS_OK,     LOW);
        digitalWrite(PIN_LED_STATUS_FAIL,   HIGH);
        digitalWrite(PIN_LED_CALIBRATION,   LOW);
    }
    
}

