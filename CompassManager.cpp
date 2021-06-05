/**
 * @file CompassManager.cpp
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

/*-----------------------------------*
 * INCLUDE FILES
 *-----------------------------------*/
#include "CompassManager.h"

/*-----------------------------------*
 * PUBLIC VARIABLE DEFINITIONS
 *-----------------------------------*/
/* None */

/*-----------------------------------*
 * PRIVATE DEFINES
 *-----------------------------------*/
float const magCalibrationMatrix[3][3] =
{
    {0.97, 0.15, 0.05},
    {0.018, 1.074, -0.042},
    {-0.002, 0.08, 1.154}
};
    
float const magBias[3] =
{
    -37.302,
    -60.436,
    -10.804
};

float const accCalibrationMatrix[3][3] =
{
{0.978, -0.009, 0.005},
{ 0.011, 1.004, -0.037},
{-0.052, -0.006, 1.02}
};

float accBias[3] =
{
-8.465,
-6.363,
-37.841
};


/*-----------------------------------*
 * PRIVATE MACROS
 *-----------------------------------*/
/* None */

/*-----------------------------------*
 * PRIVATE TYPEDEFS
 *-----------------------------------*/
/* None */

/*-----------------------------------*
 * PRIVATE FUNCTION PROTOTYPES
 *-----------------------------------*/

/**********************************
 * CONSTRUCTOR & DESTRUCTOR METHODS
***********************************/
/**
 * @name CompassManager
 * @brief CompassManager: constructor
 */
CompassManager::CompassManager()
{
    Wire.begin();
    lsm.begin();

    alpha = DEFAULT_ALPHA_VALUE;

    rawHeading          = 0;
    calibHeading        = 0;
    tiltCalibHeading    = 0;

    pitch   = 0;
    roll    = 0;

    MxCalibNormFiltered = 0;
    MyCalibNormFiltered = 0;
    MzCalibNormFiltered = 0;
    AxCalibNormFiltered = 0;
    AyCalibNormFiltered = 0;
    AzCalibNormFiltered = 0;

    offsetCompass = 0;

    delay(500);
}

/**
 * @name CompassManager
 * @brief CompassManager: get raw compass heading 
 * @param [in] float alpha: filter parameter 
 */
CompassManager::CompassManager(float alpha)
{
    CompassManager();
    this->alpha = alpha;
}

CompassManager::~CompassManager()
{
    // delete *lsm;
}

/**
 * @name getRawHeading
 * @brief getCalibratedHeading: get raw compass heading 
 * @retval compass heading value 0 to 360 deg [int]
 */
int CompassManager::getRawHeading()
{
    computeHeading();
    return (int)(rawHeading + offsetCompass)%360;
}

/**
 * @name getCalibratedHeading
 * @brief getCalibratedHeading: get calibrated compass heading tilt or not tilt compensated
 * @param [in] bool tiltCompensated: set return heading tilt or not tilt compensated with acelerometer
 * @retval compass heading value 0 to 360 deg [int]
 */
int CompassManager::getCalibratedHeading(bool tiltCompensated = true)
{
    computeHeading();
    if(tiltCompensated)
    {
        return (int)(tiltCalibHeading + offsetCompass)%360;
    }
    else
    {
        return (int)(calibHeading + offsetCompass)%360;
    }
}



/**
 * @name getPitch
 * @brief getPitch: get pitch
 * @retval float Pitch deg [0-360]
 */
float CompassManager::getPitch()
{
    return pitch * 180 / PI;
}

/**
 * @name getRoll
 * @brief getRoll: get roll
 * @retval float Roll deg [0-360]
 */
float CompassManager::getRoll()
{
    return roll * 180 / PI;
}

/**
 * @name setOffsetCompass
 * @brief setOffsetCompass: set compass offset to compensate fisical position instead real position
 * @retval None
 */
void CompassManager::setOffsetCompass(float offset)
{
    this->offsetCompass = offset;
}


/*******************
 * PUBLIC VARIABLES
*******************/
// None


/*******************
 * PRIVATE METHODS
*******************/
/**
 * @name getRawValuesMag
 * @brief getRawValuesMag: get raw magnetometer values using Adafruit lsm I2C interface
 * @param [out] float rawMagnetometerValues[3]: size 3 array for Mx, My, Mz value
 * @retval None
 */
void CompassManager::getRawValuesMag(float *rawMagnetometerValues)
{
    rawMagnetometerValues[0] = (float)lsm.magData.x;
    rawMagnetometerValues[1] = (float)lsm.magData.y;
    rawMagnetometerValues[2] = (float)lsm.magData.z;
}

/**
 * @name getRawValuesAcc
 * @brief getRawValuesAcc: get raw accelerometer values using Adafruit lsm I2C interface
 * @param [out] float rawAccelerometerValues[3]: size 3 array for Ax, Ay, Az value
 * @retval None
 */
void CompassManager::getRawValuesAcc(float *rawAccelerometerValues)
{
    rawAccelerometerValues[0] = (float)lsm.accelData.x;
    rawAccelerometerValues[1] = (float)lsm.accelData.y;
    rawAccelerometerValues[2] = (float)lsm.accelData.z;
}

/**
 * @name transformationMag
 * @brief transformationMag: get raw calibrated magnetometer using clib tranformation 
 * @param [in] float rawMagnetometerValues[3]: size 3 array for Mx, My, Mz value
 * @param [out] float calibMagnetometerValues[3]: size 3 array for MxCalib, MyCalib, MzCalib value
 * @retval None
 */
void CompassManager::calibrationTransform(float const calibrationMatrix[3][3], 
                                          float const biasArray[3], 
                                          float rawMeasurementValues[3], 
                                          float *calibratedValues)
{
    // Computing
    for (int i = 0; i < 3; ++i) rawMeasurementValues[i] = rawMeasurementValues[i] - biasArray[i];
    
    float result[3] = {0, 0, 0};
    
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result[i] += calibrationMatrix[i][j] * rawMeasurementValues[j];
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        calibratedValues[i] = result[i];
    }

}


/**
 * @name computeHeading
 * @brief computeHeading: compute and store the three kind of heading 
 * @param None
 * @retval None
 */
void CompassManager::computeHeading()
{
    lsm.read();

    float rawMagnetometerValues[3];
    float calibMagnetometerValues[3];
    getRawValuesMag(rawMagnetometerValues);
    calibrationTransform(magCalibrationMatrix, magBias, rawMagnetometerValues, calibMagnetometerValues);

    float rawAccelerometerValues[3];
    float calibAcceleronemeterValues[3];
    getRawValuesMag(rawAccelerometerValues);
    calibrationTransform(accCalibrationMatrix, accBias, rawAccelerometerValues, calibAcceleronemeterValues);

    float norm_m = sqrt(sq(calibMagnetometerValues[0]) + sq(calibMagnetometerValues[1]) + sq(calibMagnetometerValues[2])); //original code did not appear to normalize, and this seems to help
    float MxCalibratedNormalized = calibMagnetometerValues[0] / norm_m;
    float MyCalibratedNormalized = calibMagnetometerValues[1] / norm_m;
    float MzCalibratedNormalized = calibMagnetometerValues[2] / norm_m;

    float norm_a = sqrt(sq(calibAcceleronemeterValues[0]) + sq(calibAcceleronemeterValues[1]) + sq(calibAcceleronemeterValues[2])); //original code did not appear to normalize, and this seems to help
    float AxCalibratedNormalized = calibAcceleronemeterValues[0] / norm_a;
    float AyCalibratedNormalized = calibAcceleronemeterValues[1] / norm_a;
    float AzCalibratedNormalized = calibAcceleronemeterValues[2] / norm_a;

    // Low-Pass filter magnetometer
    MxCalibNormFiltered = MxCalibratedNormalized * alpha + (MxCalibNormFiltered * (1.0 - alpha));
    MyCalibNormFiltered = MyCalibratedNormalized * alpha + (MyCalibNormFiltered * (1.0 - alpha));
    MzCalibNormFiltered = MzCalibratedNormalized * alpha + (MzCalibNormFiltered * (1.0 - alpha));

    // Low-Pass filter accelerometer
    AxCalibNormFiltered = AxCalibratedNormalized * alpha + (AxCalibNormFiltered * (1.0 - alpha));
    AyCalibNormFiltered = AyCalibratedNormalized * alpha + (AyCalibNormFiltered * (1.0 - alpha));
    AzCalibNormFiltered = AzCalibratedNormalized * alpha + (AzCalibNormFiltered * (1.0 - alpha));

    // Calculating this->pitch and this->roll angles following Application Note
    this->pitch = (double) asin((double) - AxCalibNormFiltered);
    this->roll = (double) asin((double) AyCalibNormFiltered / cos((double) this->pitch));

    //  Calculating heading with raw measurements not tilt compensated
    this->rawHeading = (float)atan2(rawMagnetometerValues[1], rawMagnetometerValues[0]) * 180 / PI;

    // this->rawHeading += offsetCompass;
    if (this->rawHeading < 0) {
        this->rawHeading = 360 + this->rawHeading;
    }

    //  Calculating heading with calibrated measurements not tilt compensated
    this->calibHeading = (float)atan2(calibMagnetometerValues[1], calibMagnetometerValues[0]) * 180 / PI;
    // this->calibHeading += offsetCompass;
    if (this->calibHeading < 0) {
        this->calibHeading = 360 + this->calibHeading;
    }

    //  Calculating tilt compensated heading
    double Xh = MxCalibNormFiltered * cos((double)this->pitch) + MzCalibNormFiltered * sin((double)this->pitch);
    double Yh = MxCalibNormFiltered * sin((double)this->roll) * sin((double)this->pitch) + MyCalibNormFiltered * cos((double)this->roll) - MzCalibNormFiltered * sin((float)this->roll) * cos((float)this->pitch);
    this->tiltCalibHeading = (atan2(Yh, Xh)) * 180 / PI;

    // this->calibHeading += offsetCompass;
    if (this->tiltCalibHeading < 0) {
        this->tiltCalibHeading = 360 + this->tiltCalibHeading;
    }
    //Calculating Tilt angle in degrees
    this->tiltAngle = (float)atan2((float)fabs(AzCalibNormFiltered), (float)AxCalibNormFiltered) * 180 / PI;
}

/****************************************************************************
 ****************************************************************************/