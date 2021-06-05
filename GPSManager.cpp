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


/*-----------------------------------*
 * INCLUDE FILES
 *-----------------------------------*/
#include "GPSManager.h"

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
/* None */

/*-----------------------------------*
 * PUBLIC FUNCTION PROTOTYPES
 *-----------------------------------*/

/*******************
 * CONSTRUCTOR & DESTRUCTOR METHODS
*******************/
/**
 * @name GPSManager
 * @brief GPSManager: constructor that follow the desired position
 * @param [in] int desiredDirection: set desired heading deg [0-360]
 * @param [in] int offsetServoCompass: set offset between zero compass and zero motor [0-360]
*/
GPSManager::GPSManager()
{
	targetLatitude 	= -1.0;
	targetLongitude = -1.0;
	gpsLatitude		= -1.0;
	gpsLongitude	= -1.0;
	directionTarget = -1.0;

	GPSSerial = new SoftwareSerial(GPS_RX, GPS_TX);
	GPSSerial->begin(SERIAL_BAUD);
	simulationMode = false;
}
GPSManager::~GPSManager(){}

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
void GPSManager::setTarget(double targetLatitude, double targetLongitude)
{
	this->targetLatitude = targetLatitude;
	this->targetLongitude = targetLongitude;
}

/**
 * @name setSimulationMode
 * @brief setSimulationMode: Set simulation mode
 * @param [in] double gpsLatitude: set simulation GPS latitude
 * @param [in] double gpsLongitude:set simulation GPS logitude 
 * @retval None
 */
// void GPSManager::setSimulationMode(double gpsLatitude = 43.025985899999995, double gpsLongitude = 12.4339257)
void GPSManager::setSimulationMode(double gpsLatitude, double gpsLongitude)
{
	Serial.print("gpsLatitude: ");
	Serial.println(gpsLatitude);
	Serial.print("gpsLongitude: ");
	Serial.println(gpsLongitude);
	this->gpsLatitude = gpsLatitude;
	this->gpsLongitude = gpsLongitude;
	simulationMode = true;
}


bool GPSManager::update()
{
	if(!simulationMode)
	{
		while(GPSSerial->available() > 0)
		{
			if (gps.encode(GPSSerial->read()))
			{
				if (gps.location.isValid())
  				{
					this->gpsLatitude = gps.location.lat(); // degrees
					this->gpsLongitude = gps.location.lng(); // degrees
					return true;
				}
				else
				{
					return false;
				}
			}
			
		}
	}
}

double GPSManager::getLatitude()
{
	return this->gpsLatitude;
}
double GPSManager::getLongitude()
{
	return this->gpsLongitude;
}


/**
 * @name getTargetDistanceHeading
 * @brief getTargetDistanceHeading:  compute the heading to the targrt position
 * @param [in] const int currentHeading: set current heading
 * @param [out] int targetHeading: return target heading
 * @param [out] double distanceTarget: target distance
 * @retval bool: return true if GPS is avaliable false otherwise
 */
bool GPSManager::getTargetDistanceHeading(const int currentHeading, int &targetHeading, double &distanceTarget)
{
	// Serial.println("GET");

	bool ret = update();
	
	distanceTarget = gps.distanceBetween(gpsLatitude, gpsLongitude, targetLatitude, targetLongitude);
	directionTarget = gps.courseTo(gpsLatitude, gpsLongitude, targetLatitude, targetLongitude);
	targetHeading = 0;  // sweep

	if( currentHeading > directionTarget ) 
		targetHeading = -(currentHeading - directionTarget); // These two are equal. -(a-b) = -a+b
	else
		targetHeading = directionTarget - currentHeading;

	if( targetHeading < 0 ) 
		targetHeading += 360;	
	return ret;
}


/**
 * @name getGpsHeading
 * @brief getGpsHeading: return the heading between GPS coordinates and the target
 * @return double direction [deg 0-360]
*/
double GPSManager::getTargetHeading()
{
	return  gps.courseTo(gpsLatitude, gpsLongitude, targetLatitude, targetLongitude);
}


/****************************************************************************
 ****************************************************************************/
