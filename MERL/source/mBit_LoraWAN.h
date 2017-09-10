/*
 * mBit_LoRaWAN.h
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
 */
#include "MicroBit.h"

#define MCP7940I2CAddress	(0x6F << 1)
#define powerOffPin			P16 // drive this pin low to power off
#define RN2483ResetPin		P2  //drive this l[in low to reset RN2483
#define RN2483TxPin			P12
#define RN2483RxPin			P8
#define GroveIOD0Pin		P0
#define GroveIOD1Pin		P1
#define I2CSDAPin			P20 // Default mBit I2C
#define I2CSCLPin			P19 // Default mbit I2c

