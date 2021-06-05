// #include "CompassManager.h"
#include "MPU9250.h"
#include "ServoManager.h"
#include "GPSManager.h"
#include "Server.h"
#include "SystemManager.h"

// CompassManager cm;
MPU9250 mpu;

ServoManager sm;
GPSManager gpsm;
SystemManager *systemManager;
//casa
// 43.025932,12.433962

// LONGITUDE=12.433962
// LATITUDE=43.025932
// OVEST
// double lat_target = 43.02610605808224;
// double lon_target = 12.432494759559633;

// EST TORGIANO
// double lat_target = 43.025734;
// double lon_target = 12.435561;

// SUD TORGIANO
// double lat_target = 43.024667;
// double lon_target = 12.434512;

//NORD TORGIANO
// double lat_target = 43.027114;
// double lon_target = 12.434206;

//TORRE BAGLIONI
// 43.025567,12.438006
double lat_target = 43.025567;
double lon_target = 12.438006;


int previousTargetHeading = 0;

// const float Mag_x_offset = 366.695;
// const float Mag_y_offset = 376.02;
// const float Mag_z_offset = -397.455;
// const float Mag_x_scale = 1.0184643;
// const float Mag_y_scale = 0.97450244;
// const float Mag_z_scale = 1.0081002;

const float Mag_x_offset = 236.23502;
const float Mag_y_offset = 267.08;
const float Mag_z_offset = -383.865;
const float Mag_x_scale = 0.8924237;
const float Mag_y_scale = 0.9554539;
const float Mag_z_scale = 1.2007211;

//Calib proto A 05/06/2021
// const float Mag_x_offset = 169.245;
// const float Mag_y_offset = -186.25002;
// const float Mag_z_offset = -415.2000;
// const float Mag_x_scale = 1.0341263;
// const float Mag_y_scale = 1.0941668;
// const float Mag_z_scale = 0.893605;



int32_t timestamp = 0;

const int size_sample_filter = 30;
int heading_filter[size_sample_filter];
int sample_counter = 0;
void setup()
{
	systemManager = new SystemManager();
	Wire.begin();
	Wire.setClock(400000);
	Serial.begin(115200);

	// Init Server and get coordinates from eeprom
	Serial.println("Init Server and EEPROM");
	init_server(systemManager);
	get_coordinates(lat_target, lon_target);
	
	if(get_status())
	{
		// Init Servo manager
		Serial.println("Init Servo");
		sm = ServoManager(0, 270);

		if(sm.getStatus())
		{
			systemManager->update_servo_status(servo_status_t::OK);
		}
		else
		{
			systemManager->update_servo_status(servo_status_t::FAIL);
		}
		// update_led_status();
		delay(1000);

		if(sm.getStatus())
		{
			// Init MPU9250 9-DoF Compass
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
				systemManager->update_compass_status(compass_status_t::FAIL);
			}

			if(status_compass)
			{
				Serial.println("Calibrating Compass...");
				systemManager->update_compass_status(compass_status_t::CALIBRATING);
				mpu.calibrateAccelGyro();
				mpu.setMagBias(Mag_x_offset, Mag_y_offset, Mag_z_offset);
				mpu.setMagScale(Mag_x_scale, Mag_y_scale, Mag_z_scale);
				mpu.selectFilter(QuatFilterSel::MAHONYEM);
				
				Serial.println("End calibration Compass");
				systemManager->update_compass_status(compass_status_t::OK);

				// Init GPS Manager
				Serial.println("Init GPS Manager");

				systemManager->update_gps_status(gps_status_t::FAIL);
				gpsm = GPSManager();
				gpsm.setTarget(lat_target, lon_target);
				// gpsm.setSimulationMode();

				Serial.println("Init Completed");
				Serial.println("System Started");
				systemManager->update_gps_status(gps_status_t::OK);
			}
			else
			{
				Serial.println("ERROR ON COMPASS SETUP");
			}
		}
		else
		{
			Serial.println("ERROR ON SERVO SETUP");
		}
	}
	else
	{
		Serial.println("ERROR ON SERVER SETUP");
	}
	for(int i = 0; i < size_sample_filter; i++)
	{
		heading_filter[i] = 0;
	}

	delay(500);
}

void loop()
{
	if(systemManager->get_system_status() != system_status_t::FAIL)
	{
		// Get heading from quaternion compensation compass
		int heading;
		int start = millis();
		if(mpu.update(micros()-timestamp))
		{
			heading = (int) mpu.getHeading();
			heading_filter[sample_counter] = heading;
			sample_counter ++;
			int sum = 0;
			for(int i = 0; i < sample_counter; i++)
			{
				sum += heading_filter[i];
			}
			
			heading = sum/sample_counter;
			sample_counter = sample_counter%size_sample_filter;
		}

		int targetHeading = 0;
		double distanceTarget = 0;

		timestamp = micros(); // is for integration handling on quaternion compensation compass

		// Get Lat and Lon from Server Manager
		get_coordinates(lat_target, lon_target);
		gpsm.setTarget(lat_target, lon_target);

		bool ret_gps = gpsm.getTargetDistanceHeading(heading, targetHeading, distanceTarget);

		systemManager->update_gps_status(gps_status_t::OK);

		int difference = abs(targetHeading - previousTargetHeading);

		if((targetHeading != previousTargetHeading) and (difference >= 5) and distanceTarget < 10000)
		{
			// Serial.print("H: ");Serial.print(heading);
			// Serial.print(", TH: ");Serial.print(targetHeading);
			// Serial.print(", GTH: ");Serial.print((int)gpsm.getTargetHeading());
			// Serial.print(", D: "); Serial.println(distanceTarget);

			sm.setServoPosition(targetHeading);
			
			previousTargetHeading = targetHeading;
		}
		set_actualpose(heading, distanceTarget);


		if(not ret_gps)
		{
			systemManager->update_gps_status(gps_status_t::OFFLINE);
		}
		
		delay(10);
	}
	else
	{
		Serial.println("ERROR ON SYSTEM");
		delay(1000);
	}
	
}
