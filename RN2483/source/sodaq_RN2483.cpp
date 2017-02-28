/*
 * sodaq_rn2483.cpp
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


#include "Sodaq_RN2483.h"
#include "StringLiterals.h"
#include "Utils.h"
#include "mBit_LoRaWAN.h"

extern MicroBit uBit;


// Structure for mapping error response strings and error codes.
typedef struct StringEnumPair
{
    const char* stringValue;
    uint8_t enumValue;
} StringEnumPair_t;



// Creates a new Sodaq_RN2483 instance.
Sodaq_RN2483::Sodaq_RN2483() :
    inputBufferSize(DEFAULT_INPUT_BUFFER_SIZE),
    receivedPayloadBufferSize(DEFAULT_RECEIVED_PAYLOAD_BUFFER_SIZE),
    packetReceived(false),
    isRN2903(false)
{
	uBit.io.RN2483ResetPin.setDigitalValue(1); // reset RN is high
	uBit.io.RN2483TxPin.setDigitalValue(1);
	uBit.io.RN2483RxPin.setPull(PullUp);
	uBit.serial.baud(115200);
	uBit.serial.setRxBufferSize(DEFAULT_INPUT_BUFFER_SIZE);
	uBit.serial.send("[initRN2483]\r\n", SYNC_SPINWAIT);
	uBit.serial.clearRxBuffer();
	expectString(STR_RESULT_OK, DEFAULT_TIMEOUT);
}


// Directs the serial port transmit and recieve to RN2483
// Sets the baud rate to 57600
void Sodaq_RN2483::serialToRN2483(void){
	NRF_UART0->PSELTXD = 20; //Pin 12
	NRF_UART0->PSELRXD = 18; // Pin 8
	NRF_UART0->BAUDRATE = 0x00ebf000; // 57600 baud
}

	
// Directs the serial port transmit and recieve to USB
// Sets the baud rate to 115200
void Sodaq_RN2483::serialToUSB(void){
	NRF_UART0->PSELTXD = 24; // USB Tx
	NRF_UART0->PSELRXD = 25; // USB Rx
	NRF_UART0->BAUDRATE = 0x01d7e000; // 115200 baud
}


// Initializes the device and connects to the network using Over-The-Air Activation.
// Returns true on successful connection.
bool Sodaq_RN2483::initOTA(const uint8_t devEUI[8], const uint8_t appEUI[8], const uint8_t appKey[16], bool adr)
{
    uBit.serial.send("[initOTA]\r\n", SYNC_SPINWAIT);

    return resetDevice() &&
        setMacParam(STR_DEV_EUI, devEUI, 8) &&
        setMacParam(STR_APP_EUI, appEUI, 8) &&
        setMacParam(STR_APP_KEY, appKey, 16) &&
        setMacParam(STR_ADR, BOOL_TO_ONOFF(adr)) &&
        joinNetwork(STR_OTAA);
}

// Initializes the device and connects to the network using Activation By Personalization.
// Returns true on successful connection.
bool Sodaq_RN2483::initABP(const uint8_t devAddr[4], const uint8_t appSKey[16], const uint8_t nwkSKey[16], bool adr)
{
    uBit.serial.send("[initABP]\r\n", SYNC_SPINWAIT);



    return resetDevice() &&
        setMacParam(STR_DEV_ADDR, devAddr, 4) &&
        setMacParam(STR_APP_SESSION_KEY, appSKey, 16) &&
        setMacParam(STR_NETWORK_SESSION_KEY, nwkSKey, 16) &&
        setMacParam(STR_ADR, BOOL_TO_ONOFF(adr)) &&
        joinNetwork(STR_ABP);
}

// Sends the given payload without acknowledgement.
// Returns 0 (NoError) when transmission is successful or one of the MacTransmitErrorCodes otherwise.
uint8_t Sodaq_RN2483::send(uint8_t port, const uint8_t* payload, uint8_t size)
{
    uBit.serial.send("[send]\r\n", SYNC_SPINWAIT);

    return macTransmit(STR_UNCONFIRMED, port, payload, size);
}

// Sends the given payload with acknowledgement.
// Returns 0 (NoError) when transmission is successful or one of the MacTransmitErrorCodes otherwise.
uint8_t Sodaq_RN2483::sendReqAck(uint8_t port, const uint8_t* payload,
    uint8_t size, uint8_t maxRetries)
{
    uBit.serial.send("[sendReqAck]\r\n", SYNC_SPINWAIT);

    if (!setMacParam(STR_RETRIES, maxRetries)) {
        // not a fatal error -just show a debug message
        uBit.serial.send("[sendReqAck] Non-fatal error: setting number of retries failed.\r\n", SYNC_SPINWAIT);
    }

    return macTransmit(STR_CONFIRMED, port, payload, size);
}

// Copies the latest received packet (optionally starting from the "payloadStartPosition"
// position of the payload) into the given "buffer", up to "size" number of bytes.
// Returns the number of bytes written or 0 if no packet is received since last transmission.
uint16_t Sodaq_RN2483::receive(uint8_t* buffer, uint16_t size,
    uint16_t payloadStartPosition)
{
    uBit.serial.send("[receive]\r\n", SYNC_SPINWAIT);

    if (!this->packetReceived) {
        uBit.serial.send("[receive]: There is no packet received!\r\n", SYNC_SPINWAIT);
        return 0;
    }

    uint16_t inputIndex = payloadStartPosition * 2; // payloadStartPosition is in bytes, not hex char pairs
    uint16_t outputIndex = 0;

    // check that the asked starting position is within bounds
    if (inputIndex >= this->receivedPayloadBufferSize) {
        uBit.serial.send("[receive]: Out of bounds start position!\r\n", SYNC_SPINWAIT);
        return 0;
    }

    // stop at the first string termination char, or if output buffer is over, or if payload buffer is over
    while (outputIndex < size
        && inputIndex + 1 < this->receivedPayloadBufferSize
        && this->receivedPayloadBuffer[inputIndex] != 0
        && this->receivedPayloadBuffer[inputIndex + 1] != 0) {
        buffer[outputIndex] = HEX_PAIR_TO_BYTE(
            this->receivedPayloadBuffer[inputIndex],
            this->receivedPayloadBuffer[inputIndex + 1]);

        inputIndex += 2;
        outputIndex++;
    }

    // Note: if the payload has an odd length, the last char is discarded

    buffer[outputIndex] = 0; // terminate the string

    uBit.serial.send("[receive]: Done\r\n", SYNC_SPINWAIT);
    return outputIndex;
}

// Gets the preprogrammed EUI node address from the module.
// Returns the number of bytes written or 0 in case of error.
uint8_t Sodaq_RN2483::getHWEUI(uint8_t* buffer, uint8_t size)
{
    uBit.serial.send("[getHWEUI]", SYNC_SPINWAIT);
    
	serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(STR_CMD_GET_HWEUI, SYNC_SPINWAIT);
	uBit.serial.send(CRLF, SYNC_SPINWAIT);

    uint8_t inputIndex = 0;
    uint8_t outputIndex = 0;
    inputBuffer[0] = '\0';

    unsigned long start = uBit.systemTime();
    while (uBit.systemTime() < start + DEFAULT_TIMEOUT) {
		
        if (uBit.serial.rxBufferedSize() != 0){
				int readChar = uBit.serial.read();
				if( readChar == '\n'){
				this->inputBuffer[inputIndex -1] = '\0'; // Put NULL in buffer  to replace \r
				inputIndex = 0;
				while (outputIndex < size
                && inputIndex + 1 < this->inputBufferSize
                && this->inputBuffer[inputIndex] != 0
                && this->inputBuffer[inputIndex + 1] != 0) {
                buffer[outputIndex] = HEX_PAIR_TO_BYTE(
                    this->inputBuffer[inputIndex],
                    this->inputBuffer[inputIndex + 1]);
                inputIndex += 2;
                outputIndex++;
            }
			serialToUSB();
			uBit.serial.send("HWEUI: ", SYNC_SPINWAIT);
			uBit.serial.send(this->inputBuffer, SYNC_SPINWAIT);
            uBit.serial.send(CRLF, SYNC_SPINWAIT); 
            return outputIndex;
				
			}
			else this->inputBuffer[inputIndex++] = (char) readChar; // put normal char in buffer
            
        }
    }
	serialToUSB();
    uBit.serial.send("[getHWEUI] Timed out without a response!", SYNC_SPINWAIT);
    return 0;
}

// Waits for the given string. Returns true if the string is received before a timeout.
// Returns false if a timeout occurs or if string is not received.
// Entry here assumes we are in "serialToRN2483" state
bool Sodaq_RN2483::expectString(const char* str, uint16_t timeout)
{
	uint8_t inputIndex = 0;
	inputBuffer[0] = '\0';
    unsigned long start = uBit.systemTime();
    while (uBit.systemTime() < start + timeout) {
        if (uBit.serial.rxBufferedSize() != 0){
			int readChar = uBit.serial.read();
			if( readChar == '\n'){
				serialToUSB();
				this->inputBuffer[inputIndex -1] = '\0'; // Put NULL in buffer  to replace \r
				uBit.serial.send(this->inputBuffer, SYNC_SPINWAIT);
				uBit.serial.send(CRLF, SYNC_SPINWAIT); 
				if (strstr(this->inputBuffer, str) != NULL)  return true;
				else return false;
            }
            
			else this->inputBuffer[inputIndex++] = (char) readChar; // put normal char in buffer
        }
    }
    return false;
}

bool Sodaq_RN2483::expectOK()
{
    return expectString(STR_RESULT_OK, DEFAULT_TIMEOUT);
}

// Sends a reset command to the module and waits for the success response (or timeout).
// Also sets-up some initial parameters like power index, SF and FSB channels.
// Returns true on success.
bool Sodaq_RN2483::resetDevice()
{
    uBit.serial.send("[resetDevice]\r\n",SYNC_SPINWAIT);

	serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(STR_CMD_RESET, SYNC_SPINWAIT);
	uBit.serial.send(CRLF, SYNC_SPINWAIT);

    if (expectString(STR_DEVICE_TYPE_RN, 2000)) {
        if (strstr(this->inputBuffer, STR_DEVICE_TYPE_RN2483) != NULL) {
            uBit.serial.send("The device type is RN2483\r\n", SYNC_SPINWAIT);
            isRN2903 = false;
            return macReset(STR_BAND_868) &&
				setPowerIndex(DEFAULT_PWR_IDX_868) &&
                setSpreadingFactor(DEFAULT_SF_868);
        }
        else if (strstr(this->inputBuffer, STR_DEVICE_TYPE_RN2903) != NULL) {
            uBit.serial.send("The device type is RN2903\r\n", SYNC_SPINWAIT);
            isRN2903 = true;

            return macReset(STR_BAND_868) &&
				setFsbChannels(DEFAULT_FSB) &&
                setPowerIndex(DEFAULT_PWR_IDX_915) &&
                setSpreadingFactor(DEFAULT_SF_915);
        }
        else {
            uBit.serial.send("Unknown device type!\r\n", SYNC_SPINWAIT);

            return false;
        }
    }

    return false;
}

// Enables all the channels that belong to the given Frequency Sub-Band (FSB)
// and disables the rest.
// fsb is [1, 8] or 0 to enable all channels.
// Returns true if all channels were set successfully.
bool Sodaq_RN2483::setFsbChannels(uint8_t fsb)
{
    uBit.serial.send("[setFsbChannels]\r\n",SYNC_SPINWAIT);

    uint8_t first125kHzChannel = fsb > 0 ? (fsb - 1) * 8 : 0;
    uint8_t last125kHzChannel = fsb > 0 ? first125kHzChannel + 7 : 71;
    uint8_t fsb500kHzChannel = fsb + 63;

    bool allOk = true;
    for (uint8_t i = 0; i < 72; i++) {
		
		serialToRN2483(); // UART now pointing to RN2483
		uBit.serial.send(STR_CMD_SET_CHANNEL_STATUS, SYNC_SPINWAIT);
		uBit.serial.send(i,SYNC_SPINWAIT);
		uBit.serial.send(" ", SYNC_SPINWAIT);
		uBit.serial.send(BOOL_TO_ONOFF(((i == fsb500kHzChannel) || (i >= first125kHzChannel && i <= last125kHzChannel))), SYNC_SPINWAIT);
		uBit.serial.send(CRLF, SYNC_SPINWAIT);

        allOk &= expectOK();
    }

    return allOk;
}

// Sets the spreading factor.
// In reality it sets the datarate of the module according to the
// LoraWAN specs mapping for 868MHz and 915MHz, 
// using the given spreading factor parameter.
bool Sodaq_RN2483::setSpreadingFactor(uint8_t spreadingFactor)
{
    uBit.serial.send("[setSpreadingFactor]\r\n", SYNC_SPINWAIT);

    int8_t datarate;
    if (!isRN2903) {
        // RN2483 SF(DR) = 7(5), 8(4), 9(3), 10(2), 11(1), 12(0)
        datarate = 12 - spreadingFactor;
    }
    else {
        // RN2903 SF(DR) = 7(3), 8(2), 9(1), 10(0)
        datarate = 10 - spreadingFactor;
    }

    if (datarate > -1) {
        return setMacParam(STR_DATARATE, datarate);
    }

    return false;
}

// Sets the power index (868MHz: 1 to 5 / 915MHz: 5, 7, 8, 9 or 10)
// Returns true if succesful.
bool Sodaq_RN2483::setPowerIndex(uint8_t powerIndex)
{
    uBit.serial.send("[setPowerIndex]\r\n", SYNC_SPINWAIT);

    return setMacParam(STR_PWR_IDX, powerIndex);
}

// Sends the command together with the given paramValue (optional)
// to the device and awaits for the response.
// Returns true on success.
// NOTE: command should include a trailing space if paramValue is set
bool Sodaq_RN2483::sendCommand(const char* command, const uint8_t* paramValue, uint16_t size)
{
    uBit.serial.send("[sendCommand] ", SYNC_SPINWAIT);
    uBit.serial.send(command, SYNC_SPINWAIT);
    uBit.serial.send("[parameters] ", SYNC_SPINWAIT);
	for (uint16_t i = 0; i < size; ++i) {
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))), SYNC_SPINWAIT);
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i]))), SYNC_SPINWAIT);
        uBit.serial.send(" ", SYNC_SPINWAIT);
    }
    uBit.serial.send(CRLF, SYNC_SPINWAIT);

	serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(command, SYNC_SPINWAIT);
 

    for (uint16_t i = 0; i < size; ++i) {
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))), SYNC_SPINWAIT);
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i]))), SYNC_SPINWAIT);
    }

	uBit.serial.send(CRLF, SYNC_SPINWAIT);
    return expectOK();
}

// Sends the command together with the given paramValue (optional)
// to the device and awaits for the response.
// Returns true on success.
// NOTE: command should include a trailing space if paramValue is set
bool Sodaq_RN2483::sendCommand(const char* command, uint8_t paramValue)
{
	uBit.serial.send("[sendCommand] ", SYNC_SPINWAIT);
	uBit.serial.send(command, SYNC_SPINWAIT);
    uBit.serial.send("[parameter] ", SYNC_SPINWAIT);
	uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue))), SYNC_SPINWAIT);
	uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue))), SYNC_SPINWAIT);
	uBit.serial.send(CRLF, SYNC_SPINWAIT);


	serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(command, SYNC_SPINWAIT);
 	uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue))), SYNC_SPINWAIT);
	uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue))), SYNC_SPINWAIT);
	uBit.serial.send(CRLF, SYNC_SPINWAIT);

    return expectOK();
}

// Sends the command together with the given paramValue (optional)
// to the device and awaits for the response.
// Returns true on success.
// NOTE: command should include a trailing space if paramValue is set
bool Sodaq_RN2483::sendCommand(const char* command, const char* paramValue)
{
    uBit.serial.send("[sendCommand] ", SYNC_SPINWAIT);
	uBit.serial.send(command, SYNC_SPINWAIT);
    uBit.serial.send("[parameter] ", SYNC_SPINWAIT);
    if (paramValue != NULL) {
		uBit.serial.send(paramValue, SYNC_SPINWAIT);
	}
	uBit.serial.send(CRLF, SYNC_SPINWAIT);


	serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(command, SYNC_SPINWAIT);
	if (paramValue != NULL) {
		uBit.serial.send(paramValue, SYNC_SPINWAIT);
	}
	uBit.serial.send(CRLF, SYNC_SPINWAIT);

    return expectOK();
 
}

// Sends a join network command to the device and waits for the response (or timeout).
// Returns true on success.
bool Sodaq_RN2483::joinNetwork(const char* type)
{
    uBit.serial.send("[joinNetwork]\r\n",SYNC_SPINWAIT );
    
    serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(STR_CMD_JOIN, SYNC_SPINWAIT);
	uBit.serial.send(type, SYNC_SPINWAIT);
	uBit.serial.send(CRLF, SYNC_SPINWAIT);
	if(expectOK()){
		serialToRN2483(); // UART now pointing to RN2483
		return expectString(STR_ACCEPTED, 30000);
	}
	else return false;

    return expectOK() && expectString(STR_ACCEPTED, 30000);
}

// Initialises the mac radio for given band.
// Returns true on success.
bool Sodaq_RN2483::macReset(const char* band)
{
	uBit.serial.send("[macReset]\r\n",SYNC_SPINWAIT );
    
	serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(STR_CMD_MAC_RESET, SYNC_SPINWAIT);
	uBit.serial.send(band, SYNC_SPINWAIT);
	uBit.serial.send(CRLF, SYNC_SPINWAIT);

    return expectOK();
}

// Sends the given mac command together with the given paramValue
// to the device and awaits for the response.
// Returns true on success.
// NOTE: paramName should include a trailing space
bool Sodaq_RN2483::setMacParam(const char* paramName, const uint8_t* paramValue, uint16_t size)
{
    uBit.serial.send("[setMacParam] ", SYNC_SPINWAIT);
    uBit.serial.send(paramName, SYNC_SPINWAIT); 
    uBit.serial.send("= [parameters] ", SYNC_SPINWAIT);
	for (uint16_t i = 0; i < size; ++i) {
	uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))), SYNC_SPINWAIT);
	uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i]))), SYNC_SPINWAIT);
	uBit.serial.send(" ", SYNC_SPINWAIT);
    }
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    
	serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(STR_CMD_SET, SYNC_SPINWAIT);
	uBit.serial.send(paramName, SYNC_SPINWAIT);
 

    for (uint16_t i = 0; i < size; ++i) {
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))), SYNC_SPINWAIT);
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i]))), SYNC_SPINWAIT);
    }

	uBit.serial.send(CRLF, SYNC_SPINWAIT);
    return expectOK();

}

// Sends the given mac command together with the given paramValue
// to the device and awaits for the response.
// Returns true on success.
// NOTE: paramName should include a trailing space
bool Sodaq_RN2483::setMacParam(const char* paramName, uint8_t paramValue)
{
    uBit.serial.send("[setMacParam] ", SYNC_SPINWAIT);
	uBit.serial.send(paramName, SYNC_SPINWAIT);
    if(paramValue < 10) uBit.serial.send((char) (paramValue + '0'), SYNC_SPINWAIT);
    else if(paramValue < 100) {
		uBit.serial.send((char)(paramValue/10 + '0'), SYNC_SPINWAIT);
		uBit.serial.send((char)(paramValue%10 + '0'), SYNC_SPINWAIT);
	}
	else{
		uBit.serial.send((char)(paramValue/100 + '0'), SYNC_SPINWAIT);
		uBit.serial.send((char)((paramValue%100)/10 + '0'), SYNC_SPINWAIT);
		uBit.serial.send((char)((paramValue%100)%10 + '0'), SYNC_SPINWAIT);

	}
	uBit.serial.send(CRLF, SYNC_SPINWAIT);


	serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(STR_CMD_SET, SYNC_SPINWAIT);
	uBit.serial.send(paramName, SYNC_SPINWAIT);
	
	if(paramValue < 10) uBit.serial.send((char) (paramValue + '0'), SYNC_SPINWAIT);
    else if(paramValue < 100) {
		uBit.serial.send((char)(paramValue/10 + '0'), SYNC_SPINWAIT);
		uBit.serial.send((char)(paramValue%10 + '0'), SYNC_SPINWAIT);
	}
	else{
		uBit.serial.send((char)(paramValue/100 + '0'), SYNC_SPINWAIT);
		uBit.serial.send((char)((paramValue%100)/10 + '0'), SYNC_SPINWAIT);
		uBit.serial.send((char)((paramValue%100)%10 + '0'), SYNC_SPINWAIT);

	}
	
	uBit.serial.send(CRLF, SYNC_SPINWAIT);

    return expectOK();
  
}

// Sends the given mac command together with the given paramValue
// to the device and awaits for the response.
// Returns true on success.
// NOTE: paramName should include a trailing space
bool Sodaq_RN2483::setMacParam(const char* paramName, const char* paramValue)
{
	uBit.serial.send("[setMacParam] ", SYNC_SPINWAIT);
	uBit.serial.send(paramName, SYNC_SPINWAIT);
    uBit.serial.send("[parameter] ", SYNC_SPINWAIT);
    if (paramValue != NULL) {
		uBit.serial.send(paramValue, SYNC_SPINWAIT);
	}
	uBit.serial.send(CRLF, SYNC_SPINWAIT);


	serialToRN2483(); // UART now pointing to RN2483
	uBit.serial.send(STR_CMD_SET, SYNC_SPINWAIT);
	uBit.serial.send(paramName, SYNC_SPINWAIT);
	if (paramValue != NULL) {
		uBit.serial.send(paramValue, SYNC_SPINWAIT);
	}
	uBit.serial.send(CRLF, SYNC_SPINWAIT);

    return expectOK();
 
    
    
}

// Returns the enum that is mapped to the given "error" message.
uint8_t Sodaq_RN2483::lookupMacTransmitError(const char* error)
{
    uBit.serial.send("[lookupMacTransmitError]: ", SYNC_SPINWAIT);
    uBit.serial.send(error);

    if (error[0] == 0) {
        uBit.serial.send("[lookupMacTransmitError]: The string is empty!", SYNC_SPINWAIT);
        return NoResponse;
    }

    StringEnumPair_t errorTable[] =
    {
        { STR_RESULT_INVALID_PARAM, InternalError },
        { STR_RESULT_NOT_JOINED, NotConnected },
        { STR_RESULT_NO_FREE_CHANNEL, Busy },
        { STR_RESULT_SILENT, Busy },
        { STR_RESULT_FRAME_COUNTER_ERROR, NetworkFatalError },
        { STR_RESULT_BUSY, Busy },
        { STR_RESULT_MAC_PAUSED, InternalError },
        { STR_RESULT_INVALID_DATA_LEN, PayloadSizeError },
        { STR_RESULT_MAC_ERROR, NoAcknowledgment },
    };

    for (StringEnumPair_t * p = errorTable; p->stringValue != NULL; ++p) {
        if (strcmp(p->stringValue, error) == 0) {
            uBit.serial.send("[lookupMacTransmitError]: found ", SYNC_SPINWAIT);
            uBit.serial.send(p->enumValue);
            uBit.serial.send(CRLF, SYNC_SPINWAIT); 

            return p->enumValue;
        }
    }

    uBit.serial.send("[lookupMacTransmitError]: not found!\r\n", SYNC_SPINWAIT);
    return NoResponse;
}

uint8_t Sodaq_RN2483::macTransmit(const char* type, uint8_t port, const uint8_t* payload, uint8_t size)
{
    uBit.serial.send("[macTransmit] : ", SYNC_SPINWAIT);
    uBit.serial.send(type);
	if(port < 10) uBit.serial.send((char) (port + '0'), SYNC_SPINWAIT);
    else if(port < 100) {
		uBit.serial.send((char)(port/10 + '0'), SYNC_SPINWAIT);
		uBit.serial.send((char)(port%10 + '0'), SYNC_SPINWAIT);
	}
	else{
		uBit.serial.send((char)(port/100 + '0'), SYNC_SPINWAIT);
		uBit.serial.send((char)((port%100)/10 + '0'), SYNC_SPINWAIT);
		uBit.serial.send((char)((port%100)%10 + '0'), SYNC_SPINWAIT);

	}
	uBit.serial.send(CRLF, SYNC_SPINWAIT);
	uBit.serial.send("[Data] : ", SYNC_SPINWAIT);
	for (int i = 0; i < size; ++i) {
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(payload[i]))), SYNC_SPINWAIT);
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(payload[i]))), SYNC_SPINWAIT);
    }
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    
	serialToRN2483(); // UART now pointing to RN2483		
    uBit.serial.send(STR_CMD_MAC_TX, SYNC_SPINWAIT);
    uBit.serial.send(type, SYNC_SPINWAIT);
    uBit.serial.send(port, SYNC_SPINWAIT);
    uBit.serial.send(" ", SYNC_SPINWAIT);

    for (int i = 0; i < size; ++i) {
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(payload[i]))), SYNC_SPINWAIT);
        uBit.serial.send(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(payload[i]))), SYNC_SPINWAIT);
    }

    uBit.serial.send(CRLF, SYNC_SPINWAIT);

    if (!expectOK()) {
        return lookupMacTransmitError(this->inputBuffer); // inputBuffer still has the last line read
    }
    
        uBit.serial.send("Waiting for server response", SYNC_SPINWAIT);
 
	serialToRN2483(); // UART now pointing to RN2483
    this->packetReceived = false; // prepare for receiving a new packet

   
	uint8_t inputIndex = 0;
    unsigned long start = uBit.systemTime();
    while (uBit.systemTime() < start + RECEIVE_TIMEOUT) {
		if (uBit.serial.rxBufferedSize() != 0){
			int readChar = uBit.serial.read();
			if( readChar == '\n'){
				this->inputBuffer[inputIndex -1] = '\0'; // Put NULL in buffer  to replace \r
				serialToUSB();
				uBit.serial.send("Receive: ", SYNC_SPINWAIT);
				uBit.serial.send(this->inputBuffer, SYNC_SPINWAIT);
				uBit.serial.send(CRLF, SYNC_SPINWAIT); 
				if (strstr(this->inputBuffer, " ") != NULL) // to avoid double delimiter search
				{
					// there is a splittable line -only case known is mac_rx
					uBit.serial.send("Splittable response found\r\n", SYNC_SPINWAIT);
					return onMacRX();
				}
				else if (strstr(this->inputBuffer, STR_RESULT_MAC_TX_OK)) {
					// done
					uBit.serial.send("Received mac_tx_ok\r\n", SYNC_SPINWAIT);
					return NoError;
				}
				else {
					// lookup the error message
					uBit.serial.send("Some other string received (error)\r\n", SYNC_SPINWAIT);
					return lookupMacTransmitError(this->inputBuffer);
				}

            }
            
			else this->inputBuffer[inputIndex++] = (char) readChar; // put normal char in buffer
        }
    }
    
    uBit.serial.send("Timed-out waiting for a response!", SYNC_SPINWAIT);
    return RNTimeout;
 
}

// Parses the input buffer and copies the received payload into the "received payload" buffer
// when a "mac rx" message has been received. It is called internally by macTransmit().
// Returns 0 (NoError) or otherwise one of the MacTransmitErrorCodes.
uint8_t Sodaq_RN2483::onMacRX()
{
    uBit.serial.send("[onMacRX]\r\n", SYNC_SPINWAIT);

    // parse inputbuffer, put payload into packet buffer
    char* token = strtok(this->inputBuffer, " ");

    // sanity check
    if (strcmp(token, STR_RESULT_MAC_RX) != 0) {
        uBit.serial.send("[onMacRX]: mac_rx keyword not found!", SYNC_SPINWAIT);
        return InternalError;
    }

    // port
    token = strtok(NULL, " ");

    // payload
    token = strtok(NULL, " "); // until end of string

    uint16_t len = strlen(token) + 1; // include termination char
    memcpy(this->receivedPayloadBuffer, token, len <= this->receivedPayloadBufferSize ? len : this->receivedPayloadBufferSize);

    this->packetReceived = true; // enable receive() again
    return NoError;
}

// Provides a quick test of several methods as a pseudo-unit test.
void Sodaq_RN2483::runTestSequence()
{

    // expectString
    uBit.serial.send("write \"testString\" and then CRLF\r\n", SYNC_SPINWAIT);
    if (uBit.serial.send("testString\r\n", SYNC_SPINWAIT)) {
        uBit.serial.send("[expectString] positive case works!\r\n", SYNC_SPINWAIT);
    }

    uBit.serial.send("", SYNC_SPINWAIT);
    uBit.serial.send("write something other than \"testString\" and then CRLF", SYNC_SPINWAIT);
    if (!expectString("testString", 5000)) {
        uBit.serial.send("[expectString] negative case works!", SYNC_SPINWAIT);
    }

    // setMacParam(array)
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    uint8_t testValue[] = { 0x01, 0x02, 0xDE, 0xAD, 0xBE, 0xEF };
    setMacParam("testParam ", testValue, ARRAY_SIZE(testValue));

    // macTransmit
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    uint8_t testValue2[] = { 0x01, 0x02, 0xDE, 0xAD, 0xBE, 0xEF };
    macTransmit(STR_CONFIRMED, 1, testValue2, ARRAY_SIZE(testValue2));


    // receive
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    uBit.serial.send("==== receive", SYNC_SPINWAIT);
    char mockResult[] = "303132333435363738";
    memcpy(this->receivedPayloadBuffer, mockResult, strlen(mockResult) + 1);
    uint8_t payload[64];
    uBit.serial.send("* without having received packet", SYNC_SPINWAIT);
    uint8_t length = receive(payload, sizeof(payload));
    uBit.serial.send(reinterpret_cast<char*>(payload), SYNC_SPINWAIT);
    uBit.serial.send("Length: ", SYNC_SPINWAIT);
    uBit.serial.send(length, SYNC_SPINWAIT);
    uBit.serial.send("* having received packet", SYNC_SPINWAIT);
    this->packetReceived = true;
    length = receive(payload, sizeof(payload));
    uBit.serial.send(reinterpret_cast<char*>(payload));
    uBit.serial.send("Length: ", SYNC_SPINWAIT);
    uBit.serial.send(length, SYNC_SPINWAIT);

    // onMacRX
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    uBit.serial.send("==== onMacRX", SYNC_SPINWAIT);
    char mockRx[] = "mac_rx 1 303132333435363738";
    memcpy(this->inputBuffer, mockRx, strlen(mockRx) + 1);
    this->packetReceived = false;// reset
    uBit.serial.send("Input buffer now is: ", SYNC_SPINWAIT);
    uBit.serial.send(this->inputBuffer, SYNC_SPINWAIT);
    uBit.serial.send("onMacRX result code: ", SYNC_SPINWAIT);
    uBit.serial.send(onMacRX(), SYNC_SPINWAIT);
    uint8_t payload2[64];
    if (receive(payload2, sizeof(payload2)) != 9) {
        uBit.serial.send("len is wrong!\r\n", SYNC_SPINWAIT);
    }
    uBit.serial.send(reinterpret_cast<char*>(payload2), SYNC_SPINWAIT);
    if (receive(payload2, sizeof(payload2), 2) != 7) {
        uBit.serial.send("len is wrong!\r\n", SYNC_SPINWAIT);
    }
    uBit.serial.send(reinterpret_cast<char*>(payload2), SYNC_SPINWAIT);
    if (receive(payload2, sizeof(payload2), 3) != 6) {
        uBit.serial.send("len is wrong!\r\n", SYNC_SPINWAIT);
    }
    uBit.serial.send(reinterpret_cast<char*>(payload2), SYNC_SPINWAIT);

    // lookup error
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    uBit.serial.send(CRLF, SYNC_SPINWAIT);
    

    uBit.serial.send("empty string: ", SYNC_SPINWAIT);
    uBit.serial.send((lookupMacTransmitError("") == NoResponse) ? "passed" : "wrong", SYNC_SPINWAIT);

    uBit.serial.send("\"random\": ", SYNC_SPINWAIT);
    uBit.serial.send((lookupMacTransmitError("random") == NoResponse) ? "passed" : "wrong", SYNC_SPINWAIT);

    uBit.serial.send("\"invalid_param\": ", SYNC_SPINWAIT);
    uBit.serial.send((lookupMacTransmitError("invalid_param") == InternalError) ? "passed" : "wrong", SYNC_SPINWAIT);

    uBit.serial.send("\"not_joined\": ", SYNC_SPINWAIT);
    uBit.serial.send((lookupMacTransmitError("not_joined") == NotConnected) ? "passed" : "wrong", SYNC_SPINWAIT);

    uBit.serial.send("\"busy\": ", SYNC_SPINWAIT);
    uBit.serial.send((lookupMacTransmitError("busy") == Busy) ? "passed" : "wrong", SYNC_SPINWAIT);

    uBit.serial.send("\"invalid_param\": ", SYNC_SPINWAIT);
    uBit.serial.send((lookupMacTransmitError("invalid_param") == InternalError) ? "passed" : "wrong", SYNC_SPINWAIT);



}
