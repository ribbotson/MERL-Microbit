/*
 * rtctest.cpp
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
 * Simple date conversions and calculations test
 * 2010-02-04 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php
 * */

#include "rtc_MCP7940N.h"
#include "MicroBit.h"
#include <Cmd.h>

MicroBit uBit;
rtc_MCP7940 rtc;
ManagedString invalidArgs = "Invalid arguments \r\n";
ManagedString alarm = "\r\n Ding Ding Ding Alarm! :";
ManagedString pwrdn = "Power Failed at :";
ManagedString pwrup = "\r\nPower Restored at :";

void show_rtc_now(int arg_cnt, char **arg) {
	DateTime dt = rtc.now();
	uBit.serial.send(dt.year());
	uBit.serial.send('/');
	uBit.serial.send(dt.month());
	uBit.serial.send('/');
	uBit.serial.send(dt.day());
	uBit.serial.send(' ');
	uBit.serial.send(dt.hour());
	uBit.serial.send(':');
	uBit.serial.send(dt.minute());
	uBit.serial.send(':');
	uBit.serial.send(dt.second());

}

void show_rtc_now_sec(int arg_cnt, char **arg) {
	DateTime dt = rtc.now();
	uBit.serial.send((int) dt.get());
}

void set_rtc(int arg_cnt, char **args) {
	
	
	if (arg_cnt == 7) {
		DateTime dt (cmdStr2Num( args[1], 10), cmdStr2Num( args[2], 10),
		cmdStr2Num( args[3], 10), cmdStr2Num( args[4], 10),
		cmdStr2Num( args[5], 10), cmdStr2Num( args[6], 10));
		rtc.adjust(dt);
	}
	else if (arg_cnt == 4) {
		DateTime dt  (cmdStr2Num( args[1], 10), cmdStr2Num( args[2], 10),
		cmdStr2Num(args[3], 10));
		rtc.adjust(dt);
	}
	else {
		uBit.serial.send(invalidArgs);
	}
}

void show_pdpu(int arg_cnt, char **args)
{
  	DateTime pt = rtc.getpupdn(PWRDN);
  	uBit.serial.send(pwrdn);
	uBit.serial.send(pt.month());
	uBit.serial.send('/');
	uBit.serial.send(pt.day());
	uBit.serial.send(' ');
	uBit.serial.send(pt.hour());
	uBit.serial.send(':');
	uBit.serial.send(pt.minute());
	uBit.serial.send(':');
	uBit.serial.send(pt.second());
	pt = rtc.getpupdn(PWRUP);
	uBit.serial.send(pwrup);
	uBit.serial.send(pt.month());
	uBit.serial.send('/');
	uBit.serial.send(pt.day());
	uBit.serial.send(' ');
	uBit.serial.send(pt.hour());
	uBit.serial.send(':');
	uBit.serial.send(pt.minute());
	uBit.serial.send(':');
	uBit.serial.send(pt.second());
	
}

void set_alarm(int arg_cnt, char **args)
{
  if (arg_cnt == 9) {
	DateTime at (cmdStr2Num( args[3], 10), cmdStr2Num( args[4], 10),
	cmdStr2Num( args[5], 10), cmdStr2Num( args[6], 10),
	cmdStr2Num( args[7], 10), cmdStr2Num( args[8], 10));
	if(cmdStr2Num(args[1],10) == 0) rtc.setalarm(0, cmdStr2Num(args[2],10), at);
	if(cmdStr2Num(args[1],10) == 1) rtc.setalarm(1, cmdStr2Num(args[2],10), at);
	
  }
  else {
	uBit.serial.send(invalidArgs);  
  }
}

void clr_alarm(int arg_cnt, char **args)
{
	int alarm = cmdStr2Num(args[1],10);
	if( (alarm == 0) ||( alarm == 1)){
		rtc.clralarm(cmdStr2Num(args[1],10));
	  
  }
}

void show_alarm(int arg_cnt, char **args)
{
  	int alarm = cmdStr2Num(args[1], 10);
	if( (alarm == 0) ||( alarm == 1)){
		DateTime dt = rtc.getalarm(cmdStr2Num(args[1],10));
		uBit.serial.send(dt.month());
		uBit.serial.send('/');
		uBit.serial.send(dt.day());
		uBit.serial.send(' ');
		uBit.serial.send(dt.hour());
		uBit.serial.send(':');
		uBit.serial.send(dt.minute());
		uBit.serial.send(':');
		uBit.serial.send(dt.second());
	}
}

void sram_write(int arg_cnt, char **args)
{
  rtc.writeSram(cmdStr2Num(args[1],10),cmdStr2Num(args[2],10));
}

void sram_read(int arg_cnt, char **args)
{
  uBit.serial.send((int)rtc.readSram(cmdStr2Num(args[1],10)));
}

void pwr_off(int arg_cnt, char **args)
{
	rtc.poweroff();
 }


int main()
{
	uBit.init();
	cmdInit();
	cmdAdd("now", show_rtc_now);
	cmdAdd("nows", show_rtc_now_sec);
	cmdAdd("set", set_rtc);
	cmdAdd("pwr", show_pdpu);
	cmdAdd("salrm", set_alarm);
	cmdAdd("calrm", clr_alarm);
	cmdAdd("ralrm", show_alarm);
	cmdAdd("wsram", sram_write);
	cmdAdd("rsram", sram_read);
	cmdAdd("off", pwr_off);
	
	
	cmd_display();
	
	while(true)
	{
		cmdPoll();
		if (rtc.isalarm(0)) {
			uBit.serial.send(alarm);
			uBit.serial.send('0');
			cmd_display();
			rtc.clralarm(0);
		}
		if (rtc.isalarm(1)) {
			uBit.serial.send(alarm);
			uBit.serial.send('1');
			cmd_display();
			rtc.clralarm(1);
		}
	}

	release_fiber();
	return 0;	
}

