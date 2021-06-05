#include "MPU9250.h"
#include <FeedbackServo.h>

#define FEEDBACK_PIN 12 // NodeMCU D6
#define SERVO_PIN 14 //NodeMCU D5

FeedbackServo *servo;

MPU9250 mpu;


// const float Mag_x_offset = 366.695;
// const float Mag_y_offset = 376.02;
// const float Mag_z_offset = -397.455;
// const float Mag_x_scale = 1.0184643;
// const float Mag_y_scale = 0.97450244;
// const float Mag_z_scale = 1.0081002;

const float
Mag_x_offset = 296.175,
Mag_y_offset = 7.029999,
Mag_z_offset = -428.03,
Mag_x_scale = 1.0535749,
Mag_y_scale = 1.0375158,
Mag_z_scale = 0.919955;


int32_t timestamp = 0;

const int delta = 10;
int angle = 0;
void setup()
{
    Serial.begin(115200);
    Wire.begin();
	// Wire.setClock(400000);
    
    delay(500);

    servo = new FeedbackServo(FEEDBACK_PIN, 2.0, 0.1, 0.0, 0.0, -200, 200, 20.0);
    servo->setServoControl(SERVO_PIN);

    Serial.println("Init Compass");
    MPU9250Setting setting;
    setting.accel_fs_sel 		= ACCEL_FS_SEL::A2G;
    setting.gyro_fs_sel 		= GYRO_FS_SEL::G250DPS;
    setting.mag_output_bits 	= MAG_OUTPUT_BITS::M14BITS;
    setting.fifo_sample_rate 	= FIFO_SAMPLE_RATE::SMPL_200HZ;
    setting.gyro_fchoice 		= 0x00;
    setting.gyro_dlpf_cfg 		= GYRO_DLPF_CFG::DLPF_41HZ;
    setting.accel_fchoice 		= 0x00;
    setting.accel_dlpf_cfg 		= ACCEL_DLPF_CFG::DLPF_45HZ;

    bool status_compass = true;

    if (!mpu.setup(0x68, setting))
    {
        Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
        status_compass = false;
    }

    if(status_compass)
    {
        Serial.println("Calibrating Compass...");
        mpu.calibrateAccelGyro();
        mpu.setMagBias(Mag_x_offset, Mag_y_offset, Mag_z_offset);
        mpu.setMagScale(Mag_x_scale, Mag_y_scale, Mag_z_scale);
        mpu.selectFilter(QuatFilterSel::MAHONYEM);
        Serial.println("End calibration Compass");
    }
    else
    {
        Serial.println("ERROR ON COMPASS SETUP");
    }



}

void loop()
{
    int heading;
    if(mpu.update(micros()-timestamp))
    {
        heading = (int) mpu.getHeading();
        Serial.println("Angle: " + String(heading));
    }
    // servo->rotate_PID(angle, 0);
    angle += delta;
    delay(100);
    timestamp = micros(); // is for integration handling on quaternion compensation compass

}