/**
 * @file GPSManager.h
 * @author Emanuele Belia
 * @date 11 April 2021
 * @brief This class is for managing GPS and add some tools for heading management 
 *
 * This class is developed and tested using ublox 6m GPS module
 * 
 * @dependecies: 
 *      TinyGPS++ http://arduiniana.org/libraries/tinygpsplus/
 *      SoftwareSerial
 * 
 */

#ifndef GPS_MANAGER_H
#define GPS_MANAGER_H


/*-----------------------------------*
 * INCLUDE FILES
 *-----------------------------------*/
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

/*-----------------------------------*
 * PUBLIC DEFINES
 *-----------------------------------*/
// #define GPS_RX 13 // GPS recieve D8
// #define GPS_TX 15 // GPS transmit D7

#define GPS_RX 13 // GPS recieve D8
#define GPS_TX 15 // GPS transmit D7
#define SERIAL_BAUD   9600
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
/* None */

/*-----------------------------------*
 * PUBLIC FUNCTION PROTOTYPES
 *-----------------------------------*/

class GPSManager
{
public:
    /*******************
     * CONSTRUCTOR & DESTRUCTOR METHODS
    *******************/
    /**
     * @name GPSManager
     * @brief GPSManager: constructor that follow the desired position
     * @param [in] int desiredDirection: set desired heading deg [0-360]
     * @param [in] int offsetServoCompass: set offset between zero compass and zero motor [0-360]
    */
    GPSManager();
    ~GPSManager();

    /*******************
     * PUBLIC METHODS
    *******************/
    /**
     * @name setTarget
     * @brief setTarget: Set latitude and longitude target
     * @param [in] double targetLatitude: set desired latitude target
     * @param [in] double targetLatitude: set desired logitude target
     * @retval None
     */
    void setTarget(double targetLatitude, double targetLongitude);

    /**
     * @name setSimulationMode
     * @brief setSimulationMode: Set simulation mode
     * @param [in] double gpsLatitude: set simulation GPS latitude
     * @param [in] double gpsLongitude:set simulation GPS logitude 
     * @retval None
     */
    void setSimulationMode(double gpsLatitude = 43.025985899999995, double gpsLongitude = 12.4339257);
    // void setSimulationMode(double gpsLatitude = , double gpsLongitude);

    /**
     * @name getTargetDistanceHeading
     * @brief getTargetDistanceHeading:  compute the heading to the targrt position
     * @param [in] int currentHeading: current heading from compass 
     * @param [out] int targetHeading: heding to target
     * @param [out] double distanceTarget: distance to target
     * @retval bool: return true if GPS is avaliable false otherwise
    */
    bool getTargetDistanceHeading(const int currentHeading, int &targetHeading, double &distanceTarget);



    /**
     * @name getGpsHeading
     * @brief getGpsHeading: return the heading between GPS coordinates and the target
     * @return double direction [deg 0-360]
    */
    double getTargetHeading();

    bool update();
    double getLatitude();
    double getLongitude();





    /*******************
     * PUBLIC VARIABLES
    *******************/
    // None


private:
    /*******************
     * PRIVATE METHODS
    *******************/

    /*******************
     * PRIVATE VARIABLES
    *******************/
    TinyGPSPlus gps;
    SoftwareSerial *GPSSerial;

    double targetLatitude;
    double targetLongitude;

    double gpsLatitude;
    double gpsLongitude;
    double directionTarget;

    // bool first_coordinate



    bool simulationMode;

};


#endif /* GPS_MANAGER_H */

/****************************************************************************
 ****************************************************************************/
