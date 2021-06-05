/**
 * @file Connection_Manager.h
 * @author Emanuele Belia
 * @date 28 May 2021
 * @brief this class is for connection manager for compass configuration
 *   
 */


/*-----------------------------------*
 * INCLUDE FILES
 *-----------------------------------*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "index_html.h"
#include "SystemManager.h"


/*-----------------------------------*
 * PUBLIC DEFINES
 *-----------------------------------*/

/*-----------------------------------*
 * PUBLIC MACROS
 *-----------------------------------*/
/* None */

/*-----------------------------------*
 * PUBLIC TYPEDEFS
 *-----------------------------------*/
/* None */

/*-----------------------------------*
 * PUBLIC VARIABLE DECLARATIONS
 *-----------------------------------*/
AsyncWebServer server(80);

//* Put your SSID & Password */
const char* ssid        = "Bussola";  // Enter SSID here
const char* password    = "12345678";  //Enter Password here

/* Put IP Address details */
const IPAddress local_ip(192,168,1,1);
const IPAddress gateway (192,168,1,1);
const IPAddress subnet  (255,255,255,0);

const char* PARAM_INPUT_1 = "lat";
const char* PARAM_INPUT_2 = "lon";

double lat,lon;

int heading, distance;

bool is_connected = false;

/*-----------------------------------*
 * PUBLIC FUNCTION PROTOTYPES
 *-----------------------------------*/
void notFound(AsyncWebServerRequest *request);
void print_config();
bool init_fs(double &lat, double &lon);
void save_data(double lat, double lon);

/*******************
 * CONSTRUCTOR & DESTRUCTOR METHODS
*******************/


/*******************
 * PUBLIC METHODS
*******************/
String hostname = "bussola";
/**
 * @name init_server
 * @brief init_server: init server
 */
void init_server(SystemManager *SysMan)
{
    bool fs_ok = init_fs(lat, lon);

    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.hostname(hostname.c_str());
    delay(100);
#ifdef DEBUG
    Serial.println();
    Serial.print("IP Address: ");
    Serial.println(local_ip);
#endif
    heading  = -1;
    distance = -1;
    // Send web page with input fields to client
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send_P(200, "text/html", index_html);
    });
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) 
	{
        // double latitude, longitude;
		if (request->hasParam(PARAM_INPUT_1)) 
		{
			lat = request->getParam(PARAM_INPUT_1)->value().toDouble();
		}
		if (request->hasParam(PARAM_INPUT_2)) 
		{
			lon = request->getParam(PARAM_INPUT_2)->value().toDouble();
		}

		Serial.println(lat,7);
		Serial.println(lon,7);
        save_data(lat, lon);
        request->send_P(200, "text/html", index_html);
		// request->send(200, "text/html", "HTTP GET lat: "+ String(lat,7) + " lon: "+ String(lon,7) +"<br><a href=\"/\">Return to Home Page</a>");
    });

    server.on("/compass_status", HTTP_GET, [SysMan] (AsyncWebServerRequest *request) 
	{
		switch (SysMan->get_compass_status())
		{
            case compass_status_t::OFFLINE:
                request->send(200, "text/plain", "offline");
                break;
            case compass_status_t::CALIBRATING:
                request->send(200, "text/plain", "calibrating");
                break;
            case compass_status_t::OK:
                request->send(200, "text/plain", "ok");
                break;
            case compass_status_t::FAIL:
                request->send(200, "text/plain", "err");
                break;
            default:
                break;
        }
	});

	server.on("/servo_status", HTTP_GET, [SysMan] (AsyncWebServerRequest *request) 
	{
		switch (SysMan->get_servo_status())
		// switch (servo_status)
		{
            case servo_status_t::OFFLINE:
                request->send(200, "text/plain", "offline");
                break;
            case servo_status_t::OK:
                request->send(200, "text/plain", "ok");
                break;
            case servo_status_t::FAIL:
                request->send(200, "text/plain", "err");
                break;
            default:
                break;
		}
	});

	server.on("/gps_status", HTTP_GET, [SysMan] (AsyncWebServerRequest *request) 
	{
		switch (SysMan->get_gps_status())
		// switch (gps_status)
		{
            case gps_status_t::OFFLINE:
                request->send(200, "text/plain", "offline");
                break;
            case gps_status_t::OK:
                request->send(200, "text/plain", "ok");
                break;
            case gps_status_t::FAIL:
                request->send(200, "text/plain", "err");
                break;
            default:
                break;
		}
	});

	server.on("/sys_status", HTTP_GET, [SysMan] (AsyncWebServerRequest *request) 
	{
		switch (SysMan->get_system_status())
		// switch (system_status)
		{
            case system_status_t::OFFLINE:
                request->send(200, "text/plain", "offline");
                break;
            case system_status_t::OK:
                request->send(200, "text/plain", "ok");
                break;
            case system_status_t::FAIL:
                request->send(200, "text/plain", "err");
                break;
            default:
                break;
		}
	});

	server.on("/target", HTTP_GET, [] (AsyncWebServerRequest *request) 
	{
		request->send(200, "text/plain", String(lat,8) + ";" + String(lon,8));
	});

    server.on("/actualpose", HTTP_GET, [] (AsyncWebServerRequest *request) 
	{
        String heading_s = "-";
        String distance_s = "-";
        if(heading >= 0)
        {
            heading_s = String(heading);
        }
        if(distance >= 0)
        {
            distance_s = String(distance);
        }
		request->send(200, "text/plain", heading_s + ";" + distance_s);
	});

    

    server.onNotFound(notFound);
    server.begin();
    is_connected = true;
    is_connected += fs_ok;

}


/**
 * @name print_config
 * @brief print_config: print lat and lon
*/
void print_config()
{
    Serial.println(String("lat: ") + String(lat,7) + String(" lon: ") + String(lon,7));
}

void get_coordinates(double &latitude, double &longitude)
{
    latitude = lat;
    longitude = lon;
}

bool get_status()
{
    return is_connected;
}

/*******************
 * PUBLIC VARIABLES
*******************/
// None


/*******************
 * PRIVATE METHODS
*******************/
/**
 * @name init_fs
 * @brief init_fs: Init filesystem and if configuration file not exists then will be created otherwise data will be get
 * @param [out] double lat: latitude
 * @param [out] double lon: longitude
 */
bool init_fs(double &lat, double &lon)
{
    bool success = SPIFFS.begin();
    File configFile;
    if (!success) {
        lat = 0;
        lon = 0;
        return false;
    }
    if (SPIFFS.exists("/config.txt")) 
    {
        configFile = SPIFFS.open("/config.txt", "r");
        if (configFile) 
        {
            lat = configFile.readStringUntil('\n').toDouble();
            lon = configFile.readStringUntil('\n').toDouble();
            configFile.close();
#ifdef DEBUG
            Serial.print("lat: ");
            Serial.println(lat,8);
            Serial.print("lon: ");
            Serial.println(lon,8);
#endif
        }
        else
        {
            return false;
        }
    }
    else
    {
        configFile = SPIFFS.open("/config.txt", "w");
        if (configFile) 
        {
            lat = 43.027114;
            lon = 12.434206;
            configFile.println(lat, 8);
            configFile.println(lon, 8);
            configFile.close();
        }
        else
        {
            return false;
        }
    }
    SPIFFS.end();
    return true;
}

/**
 * @name save_data
 * @brief save_data: save config data in the eeprom
 * @param [in] double lat: latitude
 * @param [in] double lon: longitude
 */
void save_data(double lat, double lon)
{
    bool success = SPIFFS.begin();
    if (!success)
    {
        is_connected = false;
        return;
    }
    File configFile = SPIFFS.open("/config.txt", "w");
    if (configFile) 
    {
#ifdef DEBUG
        Serial.println("write_file");
#endif
        configFile.println(lat, 7);
        configFile.println(lon, 7);
        configFile.close();
    }
    SPIFFS.end();
}

/**
 * @name notFound
 * @brief notFound: page 404
 * @param [in] AsyncWebServerRequest request: request
 */
void notFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "Not found");
}

void set_actualpose(int h, int d)
{
    heading = h;
    distance = d;
}



    


/****************************************************************************
 ****************************************************************************/
