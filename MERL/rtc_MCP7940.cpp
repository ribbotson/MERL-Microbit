/*
 * rtc_MCP7940.cpp
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

/*	library based on jeelabs RTClib [original library at https://github.com/jcw/rtclib ] altered to support Microchip MCP7940M RTC, used in Arduino 
	based embedded environments. To use this library, add #include <MCP7940.h> to the top of your program.*/

#include "rtc_MCP7940N.h"
#include "mBit_LoRaWAN.h"
#include "MicroBit.h"

#define SECONDS_PER_DAY   86400L

extern MicroBit uBit;

/*	THIS SECTION COPIED DIRECTLY FROM RTClib [ https://github.com/jcw/rtclib ]
	USE PER http://opensource.org/licenses/mit-license.php */

static const uint8_t daysInMonth [] {
  31,28,31,30,31,30,31,31,30,31,30,31
};

static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
  if(y >= 2000)
    y -= 2000;
  uint16_t days = d;
  for(uint8_t i = 1; i<m; ++i)
    days += daysInMonth[i - 1];
  if(m>2 && y%4 ==0)
    ++days;
  return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
  return ((days * 24L + h) * 60 + m) * 60 + s;
}

DateTime::DateTime(long t) {

	ss=t%60;
	t/=60;
	mm=t%60;
	t/=60;
	hh=t%24;
	uint16_t days = t/24;
	uint8_t leap;
	for (yOff = 0; ;++yOff) {
		leap = yOff%4==0;
		if(days<365+leap)
		break;
		days -= 365 + leap;
	}
	for(m=1; ;++m){
		uint8_t daysPerMonth=daysInMonth[m - 1];
		if(leap && m == 2)
		++daysPerMonth;
		if(days<daysPerMonth)
		break;
		days -= daysPerMonth;
	}
	d=days+1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec) {
	if(year >= 2000)
    year -= 2000;
	yOff = year;
	m = month;
	d = day;
	hh = hour;
	mm = min;
	ss = sec;
}

static uint8_t conv2d(const char* p) {
  uint8_t v=0;
  if('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

DateTime::DateTime (const char* date, const char* time) {
	yOff = conv2d(date + 9);
	switch(date[0]) {
		case 'J': m = date[1] == 'a' ? 1 : m = date[2] == 'n' ? 6 : 7; break;
		case 'F': m = 2; break;
		case 'A': m = date[2] == 'r' ? 4 : 8; break;
		case 'M': m = date[2] == 'r' ? 3 : 5; break;
		case 'S': m = 9; break;
		case 'O': m = 10; break;
		case 'N': m = 11; break;
		case 'D': m = 12; break;
	}
	d=conv2d(date+4);
	hh=conv2d(time);
	mm=conv2d(time+3);
	ss=conv2d(time+6);
}

uint8_t DateTime::DayOfWeek() const{
  uint16_t day = get()/SECONDS_PER_DAY;
  return (day+6)%7;
}

long DateTime::get()const{
  uint16_t days=date2days(yOff, m ,d);
  return time2long(days, hh, mm, ss);
}

/*	END SECTION COPIED FROM RTClib */

/*	THIS SECTION BASED ON RTClib
	It has been adjusted to support the Microchip MCP7940M

	Version:	Initial
	Author:		C.Koiter
	Date:		August 2014
*/




void rtc_MCP7940::adjust(const DateTime& dtvals) {	//change date and time registers based on user input
	
	wdtreg dtreg;
	
	dtreg.dtbaseaddress = rtc_RTCSEC_Reg;
	dtreg.dtsec = (1 << rtc_RTCSEC_ST);
	dtreg.dtmin = bin2bcd(dtvals.minute());
	dtreg.dthour = bin2bcd(dtvals.hour());
	dtreg.dtwkday = (1 << rtc_RTCWKDAY_VBATEN);
	dtreg.dtday = bin2bcd(dtvals.day());
	dtreg.dtmonth = bin2bcd(dtvals.month());
	dtreg.dtyear = bin2bcd(dtvals.year()-2000);
	dtreg.control = 0;
	uBit.i2c.write(MCP7940I2CAddress, (char*) &dtreg,9);
	return;

}

bool rtc_MCP7940::isset(){		//check whether clock is set
	uint8_t dtsecreg = rtc_RTCSEC_Reg;
	uint8_t dtsecval;
	uBit.i2c.write(MCP7940I2CAddress, (char*) &dtsecreg, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, (char*) &dtsecval, 1);
	return ((dtsecval & (1 << rtc_RTCSEC_ST)) == (1 << rtc_RTCSEC_ST));
}

bool rtc_MCP7940::isrunning(){		//check whether clock is running
	uint8_t dtwkreg = rtc_RTCWKDAY_Reg;
	uint8_t dtwkval;
	uBit.i2c.write(MCP7940I2CAddress, (char*) &dtwkreg, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, (char*) &dtwkval, 1);
	return ((dtwkval & (1 << rtc_RTCWKDAY_OSCRUN)) == (1 << rtc_RTCWKDAY_OSCRUN));
}

bool rtc_MCP7940::isalarm(uint8_t alarm){		//check whether alarm is triggered
	if (alarm == 0){
		uint8_t almreg = rtc_RTCALM0WKDAY_Reg;
		uint8_t almval;
		uBit.i2c.write(MCP7940I2CAddress, (char*) &almreg, 1, true);
		uBit.i2c.read(MCP7940I2CAddress, (char*) &almval, 1);
		return ((almval & (1 << rtc_RTCALM0WKDAY_IF)) == (1 << rtc_RTCALM0WKDAY_IF));
	}
	else if	(alarm == 1){
		uint8_t almreg = rtc_RTCALM1WKDAY_Reg;
		uint8_t almval;
		uBit.i2c.write(MCP7940I2CAddress, (char*) &almreg, 1, true);
		uBit.i2c.read(MCP7940I2CAddress, (char*) &almval, 1);
		return ((almval & (1 << rtc_RTCALM1WKDAY_IF)) == (1 << rtc_RTCALM1WKDAY_IF));
	}
	else return 0;
}


DateTime rtc_MCP7940::now(){					//current date and time	
	rdtreg dtreg;
	
	char secreg = rtc_RTCSEC_Reg;
	uBit.i2c.write(MCP7940I2CAddress,(char*) &secreg, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, ((char*) &dtreg ), 7);

	uint8_t ss = bcd2bin(dtreg.dtsec & 0x7F);
	uint8_t mm = bcd2bin(dtreg.dtmin);
	uint8_t hh = bcd2bin(dtreg.dthour & 0x3F);
	uint8_t d = bcd2bin(dtreg.dtday);
	uint8_t m = bcd2bin(dtreg.dtmonth & 0x1F);
	uint16_t y = bcd2bin(dtreg.dtyear) + 2000;

	return DateTime (y, m, d, hh, mm, ss);
}
 
DateTime rtc_MCP7940::getalarm(uint8_t alarm){
	ralmreg alreg;
	char almsecreg; 
	if (alarm == 0)	almsecreg = rtc_RTCALM0SEC_Reg;
	else if (alarm == 1) almsecreg = rtc_RTCALM1SEC_Reg;
	else return DateTime (0,0,0,0,0,0);
	uBit.i2c.write(MCP7940I2CAddress,(char*) &almsecreg, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, ((char*) &alreg ), 7);

	uint8_t ss = bcd2bin(alreg.almsec & 0x7F);
	uint8_t mm = bcd2bin(alreg.almmin);
	uint8_t hh = bcd2bin(alreg.almhour & 0x3F);
	uint8_t d = bcd2bin(alreg.almdate);
	uint8_t m = bcd2bin(alreg.almmonth & 0x1F);

	return DateTime (0, m, d, hh, mm, ss);
}

void rtc_MCP7940::setalarm(uint8_t alarm, uint8_t mode, DateTime& atvals){
	walmreg ar;
	if (alarm == 0)	ar.baseaddress = rtc_RTCALM0SEC_Reg;
	else if (alarm == 1) ar.baseaddress = rtc_RTCALM1SEC_Reg;
	else return;
	ar.almsec = bin2bcd(atvals.second());
	ar.almmin = bin2bcd(atvals.minute());
	ar.almhour = bin2bcd(atvals.hour());
	ar.almwkday = ((mode & 0x07) << rtc_RTCALM1WKDAY_MSK0);
	ar.almdate = bin2bcd(atvals.day());
	ar.almmonth = bin2bcd(atvals.month());
	uBit.i2c.write(MCP7940I2CAddress, (char*) &ar ,7);
	
	char almrv[2];
	almrv[0] = (rtc_RTCCONTROL_Reg); // get current control register value
	uBit.i2c.write(MCP7940I2CAddress, (char*) &almrv, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, (char*) &almrv[1], 1);
	
	if (alarm == 0)	almrv[1] = almrv[1] | (1 << rtc_RTCCONTROL_ALM0EN);
	else almrv[1] = almrv[1] | (1 << rtc_RTCCONTROL_ALM1EN);
	
	uBit.i2c.write(MCP7940I2CAddress, (char*) &almrv, 2);
	return;
	
}

void rtc_MCP7940::clralarm(uint8_t alarm){
	walmreg ar;
	// clear the alarm registers
	if (alarm == 0)	ar.baseaddress = rtc_RTCALM0SEC_Reg;
	else if (alarm == 1) ar.baseaddress = rtc_RTCALM1SEC_Reg;
	else return;
	ar.almsec = 0;
	ar.almmin = 0;
	ar.almhour =0;
	ar.almwkday = 0; // and the alarm flag
	ar.almdate = 0;
	ar.almmonth = 0;
	uBit.i2c.write(MCP7940I2CAddress, (char*) &ar ,7);
	
	// clear the enable
	char almrv[2];
	almrv[0] = (rtc_RTCCONTROL_Reg); // get current control register value
	uBit.i2c.write(MCP7940I2CAddress, (char*) &almrv, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, (char*) &almrv[1], 1);
	
	if (alarm == 0)	almrv[1] = almrv[1] &= ~(1 << rtc_RTCCONTROL_ALM0EN);
	else almrv[1] = almrv[1] &= ~(1 << rtc_RTCCONTROL_ALM1EN);	
	uBit.i2c.write(MCP7940I2CAddress, (char*) &almrv, 2);

	return;
}

void rtc_MCP7940::poweroff(void){
	uBit.io.powerOffPin.setDigitalValue(0);
	
}	

DateTime rtc_MCP7940::getpupdn(uint8_t reg){
	rpowerreg pwreg;
	char pwrminreg; 
	if (reg == 0)	pwrminreg = rtc_RTCPWRDNMIN_Reg;
	else if (reg == 1) pwrminreg = rtc_RTCPWRUPMIN_Reg;
	else return DateTime (0,0,0,0,0,0);
	uBit.i2c.write(MCP7940I2CAddress,(char*) &pwrminreg, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, ((char*) &pwreg ), 4);
	
	uint8_t mm = bcd2bin(pwreg.pmin);
	uint8_t hh = bcd2bin(pwreg.phour & 0x3F);
	uint8_t d = bcd2bin(pwreg.pday);
	uint8_t m = bcd2bin(pwreg.pmonth & 0x1F);

	return DateTime (0, m, d, hh, mm, 0);
}

uint8_t rtc_MCP7940::ordinalDate(uint8_t toDay, uint8_t toMonth){	//convert user date to ordinal (julian) date
	uint8_t dtmonthreg = rtc_RTCMTH_Reg;
	uint8_t leap;
	uint8_t ordinal = 0;
	uBit.i2c.write(MCP7940I2CAddress, (char*) &dtmonthreg, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, (char*) &leap, 1);

	if(leap>=32){
		switch(toMonth){
			case 1: ordinal = toDay; break;
			case 2: ordinal = toDay + 31; break;
			case 3: ordinal = toDay + 60; break;
			case 4: ordinal = toDay + 91; break;
			case 5: ordinal = toDay + 121; break;
			case 6: ordinal = toDay + 152; break;
			case 7: ordinal = toDay + 182; break;
			case 8: ordinal = toDay + 213; break;
			case 9: ordinal = toDay + 244; break;
			case 10: ordinal = toDay + 274; break;
			case 11: ordinal = toDay + 305; break;
			case 12: ordinal = toDay + 335; break;
		}
	}
	else if(leap<32){												//leap year support
		switch(toMonth){
			case 1: ordinal = toDay; break;
			case 2: ordinal = toDay + 31; break;
			case 3: ordinal = toDay + 59; break;
			case 4: ordinal = toDay + 90; break;
			case 5: ordinal = toDay + 120; break;
			case 6: ordinal = toDay + 151; break;
			case 7: ordinal = toDay + 181; break;
			case 8: ordinal = toDay + 212; break;
			case 9: ordinal = toDay + 243; break;
			case 10: ordinal = toDay + 273; break;
			case 11: ordinal = toDay + 304; break;
			case 12: ordinal = toDay + 334; break;
		}
	}
	return ordinal;
}

uint8_t rtc_MCP7940::readSram(uint8_t address){								// read SRAM byte Address 0 to 0x3f
	
	uint8_t sramaddress, sramdata;
	if (address > 0x3f) return 0;
	sramaddress = address + 0x20;
	uBit.i2c.write(MCP7940I2CAddress, (char*) &sramaddress, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, (char*) &sramdata, 1);
	return sramdata;	
}

void rtc_MCP7940::writeSram(uint8_t address, uint8_t data){					// write SRAM byte
	uint8_t srambuffer[2];
	if (address > 0x3f) return ;
	srambuffer[0] = address + 0x20;
	srambuffer[1] = data;
	uBit.i2c.write(MCP7940I2CAddress, (char*) &srambuffer, 2);
	return;
}

void rtc_MCP7940::readSram(uint8_t address, uint8_t* data, uint8_t length){  // read SRAM bytes
	uint8_t sramaddress;
	if (address > 0x3f) return;
	if (((int)address + (int)length) > 0x3f) return;
	sramaddress = address + 0x20;
	uBit.i2c.write(MCP7940I2CAddress, (char*) &sramaddress, 1, true);
	uBit.i2c.read(MCP7940I2CAddress, (char*) &data, length);
	return;	
}

void rtc_MCP7940::writeSram(uint8_t address, uint8_t* data, uint8_t length){  // write SRAM bytes
	uint8_t srambuffer[65];
	if (address > 0x3f) return ;
	if (((int)address + (int)length) > 0x3f) return;
	srambuffer[0] = address + 0x20;
	memcpy(&srambuffer[1], data, length);
	uBit.i2c.write(MCP7940I2CAddress, (char*) &srambuffer, length +1);
	return;
}
