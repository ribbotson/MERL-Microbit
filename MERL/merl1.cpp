/*
 * merl1.cpp
 * Firmware for MERL Beet Bed Project
 * 
 * Copyright 2017 Richard Ibbotson <richard.ibbotson@btinternet.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 * */

#include "rtc_MCP7940N.h"
#include "MicroBit.h"
#include "mBit_LoRaWAN.h"
#include "Sodaq_RN2483.h"
#include "DS1820.h"
#include "I2CSoilMoistureSensor.h"
#include "BME280.h"
#include "TCS34275.h"


#define sleepTime (60*30L)  // 30 minutes = sleep time in secs
#define MAX_PROBES      4
 
MicroBit uBit; // Microbit Object
rtc_MCP7940 rtc; // Real Time clock object
Sodaq_RN2483 LoRaNode; // LoRaWAN node object
DS1820 sensor; // One wire soil temperature object
I2CSoilMoistureSensor msensor; // Soil moisture sensor object
BME280 asensor; // Air tempretaure, humidity, pressure sensor object
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
// Hue sensor object
const uint8_t devEUI[8] =
{
	//0004A30B001A20B9
	0x00, 0x04, 0xA3, 0x0B,
	0x00, 0x1A, 0x20, 0xB9
};


const uint8_t appEUI[8] =
{
	//70B3D57EF000390B
	0x70, 0xB3, 0xD5, 0x7E,
	0xF0, 0x00, 0x39, 0x0B

};


const uint8_t appKey[16] =
{
	//FDE4DCCAEE7A62DC4986A6B895AC9338
	0xFD, 0xE4, 0xDC, 0xCA,
	0xEE, 0x7A, 0x62, 0xDC,
	0x49, 0x86, 0xA6, 0xB8,
	0x95, 0xAC, 0x93, 0x38,
};

uint8_t Payload[] =
{
	0x00, 0x00, //  White soil temperature signed integer in 0.1 degree increments
	0x00, 0x00, //  Black soil temperature -100 = failed read
	0x00, 0x00, //	Blue soil temperature
	0x00, 0x00, //	Red soil temperature
	0x00, 0x00, // soil moisture
	0x00, 0x00, // soil probe temperature
	0x00, 0x00, // soil probe light
	0x00, 0x00, // air temperature
	0x00, 0x00, // air humidity
	0x00, 0x00, // air pressure
	0x00, 0x00, // light intensity
	0x00, 0x00, // light colour
	0x00, 0x00  // battery voltage
	
};

bool debug;
int8_t stage = 0, state = 0;

void ShowProgress(void){
	uBit.display.image.setPixelValue(stage, (4-state), 128);
}


int main()
{
	bool debug;
	int16_t tempnow;
	uint16_t r, g, b, c, utempnow;
	
	// Stage 1 Do all the startup stuff including resetting the alarm for wakeup
	
	uBit.init(); // Inititialise the uBit environment	
	uBit.io.powerOffPin.setDigitalValue(1); //Hold the power on
	ShowProgress();
	
	// Use the buttons for initialisation and debug
	// Check what buttons are pressed at startup. Both buttons would be pairing
	if (uBit.buttonA.isPressed()){ // Button A pressed = Debug Mode
		debug = true;
		state = 1;
		ShowProgress();
		
	}
	else 
	if (uBit.buttonB.isPressed()){ // Button B Pressed = Initialisation Mode
		state = 2;
		ShowProgress();
	} 
	else{
		state = 3;
		ShowProgress(); 
	}
	
	DateTime dt(0); // make a zero DateTime Object
	rtc.adjust(dt); // Start the rtc
	unsigned long settime = rtc.get() + sleepTime; // get current time as long and add sleep time
	DateTime at(settime);  // Make DateTime object for alarm
	rtc.setalarm(0, 7, at); // Set alarm 0 mask all
	state = 4;
	ShowProgress();
	
	// Stage 2 Gather all the sensor data for transmission
	
	// Read soil temperatures

	sensor.set_ROM( 0, 0x5903165563DAFF28); //White Sensor South Bottom
	sensor.set_ROM( 1, 0x4303165579E0FF28); //Black Sensor North Bottom
	sensor.set_ROM( 2, 0x110416589CB6FF28); //Blue Sensor South Top
	sensor.set_ROM( 3, 0x820316557FACFF28); //Red Sensor North Top
	
	for (uint8_t probe = 0; probe < MAX_PROBES; probe++){
		for(uint8_t tries = 0; tries < 5; tries++){
			sensor.convertTemperature(true);
			uBit.sleep(200);
			tempnow = sensor.temperature(probe);
			if(tempnow != -100){
				Payload[probe * 2] = tempnow >> 8;
				Payload[(probe * 2) + 1] = tempnow & 0xff;
				break;	
			}
			uBit.sleep(200);
		}
	}	
	stage = 1;
	state = 0;
	ShowProgress();
	
	// Read the soil moisture and probe temperature
	
	msensor.begin(); // reset sensor
	uBit.sleep(1000); // give some time to boot up
	
	while (msensor.isBusy()) uBit.sleep(50); // available since FW 2.3
	tempnow = msensor.getCapacitance();
	Payload[8] = tempnow >> 8;
	Payload[9] = tempnow & 0xff;
	tempnow = msensor.getTemperature();
	Payload[10] = tempnow >> 8;
	Payload[11] = tempnow & 0xff;		
	tempnow = msensor.getLight(true);
	Payload[12] = tempnow >> 8;
	Payload[13] = tempnow & 0xff;
	
	stage = 1;
	state = 1;
	ShowProgress();	
	
	// Read the air temperature, humidity and pressure
	
	tempnow = asensor.getTemperature();
	Payload[14] = tempnow >> 8;
	Payload[15] = tempnow & 0xff;
	
	tempnow = asensor.getHumidity();
	Payload[16] = tempnow >> 8;
	Payload[17] = tempnow & 0xff;
	
	tempnow = asensor.getPressure();
	Payload[18] = tempnow >> 8;
	Payload[19] = tempnow & 0xff;
	
	stage = 1;
	state = 2;
	ShowProgress();	
	
	// Read the light intensity and colour tempereature
	
	if (tcs.begin()) {
		tcs.getRawData(&r, &g, &b, &c);
		uBit.sleep(1000);
		tcs.getRawData(&r, &g, &b, &c);
		
		utempnow = tcs.calculateLux(r, g, b);
		Payload[20] = utempnow >> 8;
		Payload[21] = utempnow & 0xff;
		utempnow = tcs.calculateColorTemperature(r, g, b);
		Payload[22] = utempnow >> 8;
		Payload[23] = utempnow & 0xff;
	  
	}
	stage = 1;
	state = 3;
	ShowProgress();	
	
	// Read the battery voltage from the RN2483
	tempnow = LoRaNode.getBatVolts();
	Payload[24] = tempnow >> 8;
	Payload[25] = tempnow & 0xff;
	
	stage = 1;
	state = 4;
	ShowProgress();	
			
	// Stage 3 Join to the LoRaWAN network
	
	stage = 2;
	state = 0;
	ShowProgress();
	
	if (LoRaNode.initOTA(devEUI, appEUI, appKey, true))
	{
		state = 1;
		ShowProgress();
	}
	else
	{
		state = 2;
		ShowProgress();
	}
	
	
	// Stage 4 Send the sensor data
	
	stage = 3;
	state = 0;
	ShowProgress();
	
	switch (LoRaNode.sendReqAck(4, Payload, sizeof(Payload), 2)) {
		case NoError:
			stage = 4;
			state = 0;
			ShowProgress();
			uBit.sleep(1000);
			// All done success turn off the lights and sleep
			rtc.poweroff();
			break;	
					
			case NoResponse:	// Need to add some retry code
				state = 1;		
				ShowProgress();				
				break;
			case RNTimeout:		// Need to add some retry code
				state = 1;
				ShowProgress();
				break;
			case PayloadSizeError: //// Need to add some retry code
				state = 2;
				ShowProgress();
				break;
			case InternalError: // Need to add some retry code
				state = 2;
				ShowProgress();
				break;
			case Busy:  // Need to add some retry code
				state = 3;
				ShowProgress();
				break;
			case NetworkFatalError:  // Need to add some retry code
				state = 3;
				ShowProgress();
				break;
			case NotConnected:  // Need to add some retry code
				state = 4;
				ShowProgress();
				break;
			case NoAcknowledgment:  // Need to add some retry code
				state = 4;
				ShowProgress();
				break;
			default:
				break;
			}
	
	
	
	// All done anyway turn off the lights and sleep
	
	rtc.poweroff();


	release_fiber();
	return 0;	
}

