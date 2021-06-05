/**
 * @file ServoManager.h
 * @author Emanuele Belia
 * @date 10 April 2021
 * @brief This class is for managing servo direction pointing  a desired compass heading, passing actual heading and a desired heading
 *        considering the offstet between 0 angle of the compass and the 0 angle desired for the servo motor
 *        In other hands the servo mantain the same desiredPosition all the time also rotating the compass box.
 *        Knowing the geometry of the box and the rotation betweeen the zero of the compass and the zero of the servo
 *        pointing desired position is possible.
 *
 * This class is developed and tested using High Speed Parallax 360 servo motor
 * @note This class is tested with 1 MCUs:
 *          - NodeMCU 1.0 (ESP-12E)
 *       Actually the PID library for 360 degrees servo control is developed only for NodeMCU but it is not impossible the translation for ESP32 and AVR family
 *
 * @dependecies: Feedback_Control_Servo https://github.com/belyeng93/Feedback_Control_Servo
 *
 */

#include "ServoManager.h"

#define FEEDBACK_PIN 12 // NodeMCU D6
#define SERVO_PIN 14 //NodeMCU D5
/*******************
 * CONSTRUCTOR & DESTRUCTOR METHODS
*******************/
/**
 * @name ServoManager
 * @brief ServoManager: constructor that follow the desired position
 * @param [in] int desiredDirection: set desired heading deg [0-360]
 * @param [in] int offsetServoCompass: set offset between zero compass and zero motor [0-360]
*/
ServoManager::ServoManager()
{
    // servo = new FeedbackServo(FEEDBACK_PIN, 2.0, 0.1, 0.0, 0.0, -200, 200, 20.0);
    init = false;
    
    servo = new FeedbackServo(FEEDBACK_PIN, 5.0, 0.1, 0.0, 0.0, -200, 200, 20.0);
    servo->setServoControl(SERVO_PIN);

    this->previousHeading = 0;
    this->deltaForDesiredDirection = 0;
    this->offsetServoCompass = 0;
    this->n_turns = 0;
    servo->rotate_PID(deltaForDesiredDirection + offsetServoCompass, 1);
    init = true;
}
ServoManager::ServoManager(int desiredDirection, int offsetServoCompass)
{
    servo = new FeedbackServo(FEEDBACK_PIN, 2.0, 0.1, 0.0, 0.0, -200, 200, 20.0);
    servo->setServoControl(SERVO_PIN);

    this->previousHeading = 0;
    this->deltaForDesiredDirection = desiredDirection;
    this->offsetServoCompass = offsetServoCompass;
    this->n_turns = 0;
    servo->rotate_PID(deltaForDesiredDirection + offsetServoCompass, 1);
    init = true;

}

ServoManager::~ServoManager(){}

/*******************
 * PUBLIC METHODS
*******************/
/**
 * @name setFollowHeading
 * @brief setFollowHeading: Set heading that the servo will point
 * @param [in] int heading: set desired heading deg [0-360]
 * @retval None
 */
void ServoManager::setFollowHeading(int heading)
{
    //TODO: check old position
    desiredDirection = heading;
    deltaForDesiredDirection += desiredDirection;
}

/**
 * @name setOffsetServoCompass
 * @brief setOffsetServoCompass: offset between servo and compass 0 that's mean the zero of the compass could be not the same of the servo
 * @param [in] int offset: degrees between 0 angle of the servo and 0 angle of the compass deg [0-360]
 * @retval None
 */
void ServoManager::setOffsetServoCompass(int offset)
{
    offsetServoCompass = offset;
}


/**
 * @name updateServoPosition
 * @brief updateServoPosition: update the pointing position passing actual heading taked with compass
 * @param [in] int actualHeading: actual heading deg [0-360]
 * @retval None
 */
void ServoManager::updateServoPosition(int actualHeading)
{
    int delta = actualHeading - (this->previousHeading%360);

    if(delta > 180)
    {
        //ANTICLOCKWISE MOVEMENT PASSING ZERO HEADING ANGLE
        //avoiding servo turn around of the servo near zero when compass goes from 0 to 359
        // Serial.println("Anticlockwise zero");
        deltaForDesiredDirection += 360 - delta;
    }
    else if(delta < -180)
    {
        //CLOCKWISE MOVEMENT PASSING ZERO HEADING ANGLE
        //avoiding servo turn around of the servo near zero when compass goes from 359 to 0
        // Serial.println("Clockwise zero");
        deltaForDesiredDirection += 360 + delta;
    }
    else
    {
        deltaForDesiredDirection += delta;
    }
    if(delta != 0)
    {
        //Move servo only if delta is positive or negative
        servo->rotate_PID((-deltaForDesiredDirection) + offsetServoCompass, 0);
    }
    this->previousHeading = actualHeading;
}


/**
 * @name setServoPosition
 * @brief setServoPosition: set servo position checking at zero position if it is a clockwise or anticlockwise rotation to
 * avoid turn around of the needle
 * @param [in] int actualHeading: actual heading deg [0-360]
 * @retval None
 */
void ServoManager::setServoPosition(int position)
{
    // FOR CONTINUOUS ROTATION
    //i'm in 240 deg -> previous heading = 240  == actual position
    // go to 40 deg -> position = 40   == desired position
    // alpha is  desired - actual --> 40 - 240 = -200
    //then delta is 360 + alpha --> 360 - 200 = 160 --> this is the value to be add to the actual position to obtain a short turn to 40 degrees
    // init = false;

    int alpha = position - (this->previousHeading%360);
    int delta = 0;
    int servo_position = 0;

    if((alpha <-180) and init)
    {
        // i need to sum the alpha value
        delta = (360 + alpha);
    }
    else if((alpha > 180) and init)
    {
        delta = (alpha - 360);
    }
    else
    {
        delta = alpha;
    }

    servo_position = (previousHeading  + delta) % 360;

    // Serial.println(String("SP: ") + String(servo_position) + 
    //                 String("\nalpha: ") + String(alpha) +
    //                 String("\nDt: ") + String(delta) +
    //                 String(" \nSum: ") + String(servo_position + offsetServoCompass)
    // );

    servo->rotate_PID(servo_position + offsetServoCompass, 0);
    this->previousHeading = servo_position;
    init = true;
}

bool ServoManager::getStatus()
{
    return init;
}


/*******************
 * PUBLIC VARIABLES
*******************/
// None


/*******************
 * PRIVATE METHODS
*******************/
