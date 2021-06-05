/**
 * @file ServoManager.h
 * @author Emanuele Belia
 * @date 10 April 2021
 * @brief This class is for managing servo direction pointing  a desired compass heading, passing actual heading and a desired heading
 *        considering the offstet between 0 angle of the compass and the 0 angle desired for the servo motor
 *
 * This class is developed and tested using High Speed Parallax 360 servo motor
 * @note This class is tested with 1 MCUs:
 *          - NodeMCU 1.0 (ESP-12E)
 *       Actually the PID library for 360 degrees servo control is developed only for NodeMCU but it is not impossible the translation for ESP32 and AVR family
 * 
 * @dependecies: Feedback_Control_Servo https://github.com/belyeng93/Feedback_Control_Servo
 *   
 */

#ifndef SERVO_MANAGER_H
#define SERVO_MANAGER_H


/*-----------------------------------*
 * INCLUDE FILES
 *-----------------------------------*/
#include <FeedbackServo.h>

/*-----------------------------------*
 * PUBLIC DEFINES
 *-----------------------------------*/

/*-----------------------------------*
 * PUBLIC MACROS
 *-----------------------------------*/
template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}
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

class ServoManager
{
public:
    /*******************
     * CONSTRUCTOR & DESTRUCTOR METHODS
    *******************/
    /**
     * @name ServoManager
     * @brief ServoManager: constructor that follow the desired position
     * @param [in] int desiredDirection: set desired heading deg [0-360]
     * @param [in] int offsetServoCompass: set offset between zero compass and zero motor [0-360]
    */
    ServoManager();
    ServoManager(int desiredDirection, int offsetServoCompass);
    ~ServoManager();

    /*******************
     * PUBLIC METHODS
    *******************/
    /**
     * @name setFollowHeading
     * @brief setFollowHeading: Set heading that the servo will point
     * @param [in] int heading: set desired heading deg [0-360]
     * @retval None
     */
    void setFollowHeading(int heading);

    /**
     * @name setOffsetServoCompass
     * @brief setOffsetServoCompass: offset between servo and compass 0 that's mean the zero of the compass could be not the same of the servo
     * @param [in] int offset: degrees between 0 angle of the servo and 0 angle of the compass deg [0-360]
     * @retval None
     */
    void setOffsetServoCompass(int offset);


    /**
     * @name updateServoPosition
     * @brief updateServoPosition: update the pointing position passing actual heading taked with compass
     * @param [in] int actualHeading: actual heading deg [0-360]
     * @retval None
     */
    void updateServoPosition(int actualHeading);

    /**
     * @name setServoPosition
     * @brief setServoPosition: set servo position
     * @param [in] int position: deg [0-360]
     * @retval None
     */
    void setServoPosition(int position);

    /**
     * @name getStatus
     * @brief getStatus: get status of servo
     * @retval bool status
     */
    bool getStatus();







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
    FeedbackServo *servo;

    int offsetServoCompass;
    int desiredDirection;   


    int previousHeading;
    int deltaForDesiredDirection;
    bool init;

    int n_turns; // numebers of rotated turns

};


#endif /* SERVO_MANAGER_H */

/****************************************************************************
 ****************************************************************************/
