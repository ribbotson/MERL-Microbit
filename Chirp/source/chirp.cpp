/*
 * Chirp.cpp
 * 
 * Copyright 2017 Richard <Richard@RICKYONTHEROAD>
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
 */
#include "MicroBit.h"
#include "I2CSoilMoistureSensor.h"


MicroBit uBit;
I2CSoilMoistureSensor sensor;


int main() {
	uBit.init();	
	uBit.serial.baud(115200);
	
	
	sensor.begin(); // reset sensor
	uBit.sleep(1000); // give some time to boot up
	uBit.serial.printf("I2C Soil Moisture Sensor Address: %x\r\n", sensor.getAddress());
	uBit.serial.printf("Software Firmware Version: %x\r\n", sensor.getVersion());


    while(1) {
		
		while (sensor.isBusy()) uBit.sleep(50); // available since FW 2.3
		uBit.serial.printf("Soil Moisture Capacitance: %d", sensor.getCapacitance());
		int temp = sensor.getTemperature();
		uBit.serial.printf(", Temperature: %d.%d", temp / 10, temp % 10);
		uBit.serial.printf(", Light: %d\r\n", sensor.getLight(true));

        uBit.sleep(2000);
    }
	release_fiber();
	return 0;	
}

