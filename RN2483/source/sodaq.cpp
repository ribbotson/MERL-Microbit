/*
 * sodaq.cpp
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
* Copyright (c) 2015 SODAQ. All rights reserved.
*
* This file is part of Sodaq_RN2483.
*
* Sodaq_RN2483 is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as
* published by the Free Software Foundation, either version 3 of
* the License, or(at your option) any later version.
*
* Sodaq_RN2483 is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with Sodaq_RN2483.  If not, see
* <http://www.gnu.org/licenses/>.
*/

#include <Sodaq_RN2483.h>
#include "MicroBit.h"
#include "mBit_LoRaWAN.h"

MicroBit uBit;
Sodaq_RN2483 LoRaNode;


const uint8_t devEUI[8] =
{
	//0004A30B001A20B9
	0x00, 0x04, 0xA3, 0x0B,
	0x00, 0x1A, 0x20, 0xB9
};

// USE YOUR OWN KEYS!
const uint8_t appEUI[8] =
{
	//70B3D57EF000390B
	0x70, 0xB3, 0xD5, 0x7E,
	0xF0, 0x00, 0x39, 0x0B

};

// USE YOUR OWN KEYS!
const uint8_t appKey[16] =
{
	//FDE4DCCAEE7A62DC4986A6B895AC9338
	0xFD, 0xE4, 0xDC, 0xCA,
	0xEE, 0x7A, 0x62, 0xDC,
	0x49, 0x86, 0xA6, 0xB8,
	0x95, 0xAC, 0x93, 0x38,
};

uint8_t testPayload[] =
{
	0x30, 0x31, 0xFF, 0xDE, 0xAD
};

uint8_t cbuffer[16];
int main()
{
	uBit.init();
	
	uBit.io.powerOffPin.setDigitalValue(1); // hold power on from battery

	if (LoRaNode.initOTA(devEUI, appEUI, appKey, true))
	{
		uBit.serial.send("Connection to the network was successful.\r\n");
	}
	else
	{
		uBit.serial.send("Connection to the network failed!\r\n");
	}

	LoRaNode.getHWEUI(cbuffer,16);

	while(1)
	{
		uBit.serial.send("Sleeping for 5 seconds before starting sending out test packets.");
		for (uint8_t i = 5; i > 0; i--)
		{
			uBit.serial.send(".");
			uBit.sleep(1000);
		}
		uBit.serial.send("\r\n");
		// send 10 packets, with at least a 5 seconds delay after each transmission (more seconds if the device is busy)
		uint8_t i = 10;
		while (i > 0)
		{
			testPayload[0] = i; // change first byte

			switch (LoRaNode.sendReqAck(4, testPayload, 5, 2))
			{
			case NoError:
				uBit.serial.send("Successful transmission.\r\n");
				i--;
				
				break;
			case NoResponse:
				uBit.serial.send("There was no response from the device.\r\n");
				break;
			case RNTimeout:
				uBit.serial.send("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.\r\n");
				uBit.sleep(20000);
				break;
			case PayloadSizeError:
				uBit.serial.send("The size of the payload is greater than allowed. Transmission failed!\r\n");
				break;
			case InternalError:
				uBit.serial.send("Oh No! This shouldn't happen. Something is really wrong! Try restarting the device!\r\nThe program will now halt.\r\n");
				while (1) {};
				break;
			case Busy:
				uBit.serial.send("The device is busy. Sleeping for 10 extra seconds.\r\n");
				uBit.sleep(10000);
				break;
			case NetworkFatalError:
				uBit.serial.send("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe program will now halt.\r\n");
				while (1) {};
				break;
			case NotConnected:
				uBit.serial.send("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe program will now halt.\r\n");
				while (1) {};
				break;
			case NoAcknowledgment:
				uBit.serial.send("There was no acknowledgment sent back!\r\n");
				break;
			default:
				break;
			}
			uBit.sleep(5000);
		}
	}
	release_fiber();
	return 0;	

}
