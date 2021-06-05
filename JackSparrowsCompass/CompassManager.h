/**
 * @file CompassManager.h
 * @author Emanuele Belia
 * @date 10 April 2021
 * @brief This class is for managing calibrated tilt compensates compass using 6 DOF accelerometer and magnetometer
 *
 * This class is developed and tested using LSM303DLHC mounted in a cheepest breakout board called GY-511
 * 
 * @note For the calibration, after some different tips and tool, has been perfomed with MagMaster
 * https://sourceforge.net/projects/msgviewer/
 * Using apposite MCU sketch a calibration first of the accelerometer then of the magnetometer 
 * or viceversa has been performed.
 * 
 * @note actually accelerometer and magnetometer calibration (matrix and bias) parameters are hardcoded 
 * 
 * @note This class is tested with 2 MCUs:
 *          - Arduno UNO
 *          - NodeMCU 1.0 (ESP-12E)
 * and then i supposed it works well with AVR and ESP family
 * 
 * @dependecies: Adafruit_Sensor and Adafruit_LSM303 lib has been used for I2C comunication 
 *              with the LSM303DLHC sensor
 *   
 */

#ifndef COMPASS_MANAGER_H
#define COMPASS_MANAGER_H


/*-----------------------------------*
 * INCLUDE FILES
 *-----------------------------------*/
#include "Wire.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303.h>

/*-----------------------------------*
 * PUBLIC DEFINES
 *-----------------------------------*/
const float DEFAULT_ALPHA_VALUE = 0.25;

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

class CompassManager
{
public:
    /*******************
     * CONSTRUCTOR & DESTRUCTOR METHODS
    *******************/
    /**
     * @name CompassManager
     * @brief CompassManager: constructor
     */
    CompassManager();
    
    /**
     * @name CompassManager
     * @brief CompassManager: get raw compass heading 
     * @param [in] float alpha: filter parameter 
     */
    CompassManager(float alpha);
    ~CompassManager();

    /*******************
     * PUBLIC METHODS
    *******************/
    /**
     * @name getRawHeading
     * @brief getCalibratedHeading: get raw compass heading 
     * @retval compass heading value 0 to 360 deg [int]
     */
    int getRawHeading();

    /**
     * @name getCalibratedHeading
     * @brief getCalibratedHeading: get calibrated compass heading tilt or not tilt compensated
     * @param [in] bool tiltCompensated: set return heading tilt or not tilt compensated with acelerometer
     * @retval compass heading value 0 to 360 deg [int]
     */
    int getCalibratedHeading(bool tiltCompensated);


    /**
     * @name getPitch
     * @brief getPitch: get pitch
     * @retval float Pitch deg [0-360]
     */
    float getPitch();

    /**
     * @name getRoll
     * @brief getRoll: get roll
     * @retval float Roll deg [0-360]
     */
    float getRoll();

    /**
     * @name setOffsetCompass
     * @brief setOffsetCompass: set compass offset to compensate fisical position instead real position deg 
     * @param [in] bool offset: set offset compass deg [0-360]
     * @retval None
     */
    void setOffsetCompass(float offset);

    /*******************
     * PUBLIC VARIABLES
    *******************/
    // None


private:
    /*******************
     * PRIVATE METHODS
    *******************/
    /**
     * @name getRawValuesMag
     * @brief getRawValuesMag: get raw magnetometer values using Adafruit lsm I2C interface
     * @param [out] float rawMagnetometerValues[3]: size 3 array for Mx, My, Mz value
     * @retval None
     */
    void getRawValuesMag(float *rawMagnetometerValues);

    /**
     * @name getRawValuesAcc
     * @brief getRawValuesAcc: get raw accelerometer values using Adafruit lsm I2C interface
     * @param [out] float rawAccelerometerValues[3]: size 3 array for Ax, Ay, Az value
     * @retval None
     */
    void getRawValuesAcc(float *rawAccelerometerValues);
    
    /**
     * @name calibrationTransform
     * @brief calibrationTransform: get calibrate raw meauserments with proper calibration matrix and bias array
     * @param [in] float calibrationMatrix[3][3]: calibration parameters
     * @param [in] float biasArray[3]: bias array
     * @param [in] float rawMeasurementValues[3]: raw measurement
     * @param [out] float calibMagnetometerValues[3]: calibrated measurement value
     * @retval None
     */
    void calibrationTransform(  float const calibrationMatrix[3][3], 
                                float const biasArray[3], 
                                float rawMeasurementValues[3], 
                                float *calibratedValues);


    /**
     * @name computeHeading
     * @brief computeHeading: compute and store the three kind of heading 
     * @param None
     * @retval None
     */
    void computeHeading();

    /*******************
     * PRIVATE VARIABLES
    *******************/
    Adafruit_LSM303 lsm;

    // Filter value
    float alpha;

    // All headings values
    int rawHeading;
    int calibHeading;
    int tiltCalibHeading;
    
    float tiltAngle;

    float roll;
    float pitch;

    float MxCalibNormFiltered;
    float MyCalibNormFiltered;
    float MzCalibNormFiltered;
    float AxCalibNormFiltered;
    float AyCalibNormFiltered;
    float AzCalibNormFiltered;

    float offsetCompass; //degrees




};


#endif /* COMPASS_MANAGER_H */

/****************************************************************************
 ****************************************************************************/
