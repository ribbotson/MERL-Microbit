/*
 * MCP7940N.h
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
// Defines for the registers and register contents

#define rtc_RTCSEC_Reg			0x00
#define rtc_RTCSEC_SEC			0 // seconds in bcd bit shift
#define rtc_RTCSEC_ST			7 // Start Oscillator bit shift

#define rtc_RTCMIN_Reg			0x01
#define rtc_RTCMIN_MIN			0 // minutes in bcd bit shift

#define rtc_RTCHOUR_Reg			0x02
#define rtc_RTCHOUR_HR			0 // hours in bcd shift
#define rtc_RTCHOUR_PM  		5 // PM in 12 hour mode
#define rtc_RTCHOUR_12			6 // 12 hour mode

#define rtc_RTCWKDAY_Reg		0x03
#define rtc_RTCWKDAY_WKDAY		0 // weekday in bcd shift
#define rtc_RTCWKDAY_VBATEN 	3 // external battery enable
#define rtc_RTCWKDAY_PWRFAIL 	4 // power fail status bit
#define rtc_RTCWKDAY_OSCRUN 	5 // oscillator running status bit

#define rtc_RTCDATE_Reg			0x04
#define rtc_RTCDATE_DATE		0 // day in bcd bit shift

#define rtc_RTCMTH_Reg			0x05
#define rtc_RTCMTH_MTH			0 // month in bcd bit shift
#define rtc_RTCMTH_LPYR			5 // Start Oscillator bit shift

#define rtc_RTCYEAR_Reg			0x06
#define rtc_RTCYEAR_YR			0 // year in bcd bit shift

#define rtc_RTCCONTROL_Reg		0x07
#define rtc_RTCCONTROL_SQFS		0 // square wave frequency
#define rtc_RTCCONTROL_CSRTRIM 	2 // coarse trim mode enable
#define rtc_RTCCONTROL_EXTOSC 	3 // external oscillator input enable
#define rtc_RTCCONTROL_ALM0EN	4 // alarm 0 enable
#define rtc_RTCCONTROL_ALM1EN 	5 // alarm 1 enable
#define rtc_RTCCONTROL_SQWEN 	6 // square wave output enable
#define rtc_RTCCONTROL_OUT		7 // output bit control

#define rtc_OSCTRIM_Reg			0x08
#define rtc_OSCTRIM_TRIMVAL		0 // trim value
#define rtc_OSCTRIM_SIGN		7 // trim sign

#define rtc_RTCALM0SEC_Reg		0x0A
#define rtc_RTCALM0SEC_SEC		0 // alarm seconds in bcd bit shift

#define rtc_RTCALM0MIN_Reg		0x0B
#define rtc_RTCALM0MIN_MIN		0 // alarm minutes in bcd bit shift

#define rtc_RTCALM0HOUR_Reg		0x0C
#define rtc_RTCALM0HOUR_HR		0 // hours in bcd shift
#define rtc_RTCALM0HOUR_PM  	5 // PM in 12 hour mode
#define rtc_RTCALM0HOUR_12		6 // 12 hour mode

#define rtc_RTCALM0WKDAY_Reg	0x0D
#define rtc_RTCALM0WKDAY_DAY	0 // weekday in bcd shift
#define rtc_RTCALM0WKDAY_IF		3 // interrupt flag
#define rtc_RTCALM0WKDAY_MSK0 	4 // alarm mask bit 0
#define rtc_RTCALM0WKDAY_MSK1 	5 // alarm mask bit 1
#define rtc_RTCALM0WKDAY_MSK2 	6 // alarm mask bit 2
#define rtc_RTCALM0WKDAY_POL 	7 // alarm polarity

#define rtc_RTCALM0DATE_Reg		0x0E
#define rtc_RTCALM0DATE_DATE	0 // day in bcd bit shift

#define rtc_RTCALM0MTH_Reg		0x0F
#define rtc_RTCALM0MTH_MTH		0 // month in bcd bit shift

#define rtc_RTCALM1SEC_Reg		0x11
#define rtc_RTCALM1SEC_SEC		0 // alarm seconds in bcd bit shift

#define rtc_RTCALM1MIN_Reg		0x12
#define rtc_RTCALM1MIN_MIN		0 // alarm minutes in bcd bit shift

#define rtc_RTCALM1HOUR_Reg		0x13
#define rtc_RTCALM1HOUR_HR		0 // hours in bcd shift
#define rtc_RTCALM1HOUR_PM  	5 // PM in 12 hour mode
#define rtc_RTCALM1HOUR_12		6 // 12 hour mode

#define rtc_RTCALM1WKDAY_Reg	0x14
#define rtc_RTCALM1WKDAY_DAY	0 // weekday in bcd shift
#define rtc_RTCALM1WKDAY_IF		3 // interrupt flag
#define rtc_RTCALM1WKDAY_MSK0 	4 // alarm mask bit 0
#define rtc_RTCALM1WKDAY_MSK1 	5 // alarm mask bit 1
#define rtc_RTCALM1WKDAY_MSK2 	6 // alarm mask bit 2
#define rtc_RTCALM1WKDAY_POL 	7 // alarm polarity

#define rtc_RTCALM1DATE_Reg		0x15
#define rtc_RTCALM1DATE_DATE	0 // day in bcd bit shift

#define rtc_RTCALM1MTH_Reg		0x16
#define rtc_RTCALM1MTH_MTH		0 // month in bcd bit shift

#define rtc_RTCPWRDNMIN_Reg		0x18
#define rtc_RTCPWRDNMIN_MIN		0 // alarm minutes in bcd bit shift

#define rtc_RTCPWRDNHOUR_Reg	0x19
#define rtc_RTCPWRDNHOUR_HR		0 // hours in bcd shift
#define rtc_RTCPWRDNHOUR_PM  	5 // PM in 12 hour mode
#define rtc_RTCPWRDNHOUR_12		6 // 12 hour mode

#define rtc_RTCPWRDNDATE_Reg	0x1A
#define rtc_RTCPWRDNDATE_DATE	0 // day in bcd bit shift

#define rtc_RTCPWRDNMTH_Reg		0x1B
#define rtc_RTCPWRDNMTH_MTH		0 // month in bcd bit shift
#define rtc_RTCPWRDNMTH_DAY		5 // day in week bit shift

#define rtc_RTCPWRUPMIN_Reg		0x1C
#define rtc_RTCPWRUPMIN_MIN		0 // alarm minutes in bcd bit shift

#define rtc_RTCPWRUPHOUR_Reg	0x1D
#define rtc_RTCPWRUPHOUR_HR		0 // hours in bcd shift
#define rtc_RTCPWRUPHOUR_PM  	5 // PM in 12 hour mode
#define rtc_RTCPWRUPHOUR_12		6 // 12 hour mode

#define rtc_RTCPWRUPDATE_Reg	0x1E
#define rtc_RTCPWRUPDATE_DATE	0 // day in bcd bit shift

#define rtc_RTCPWRUPMTH_Reg		0x1F
#define rtc_RTCPWRUPMTH_MTH		0 // month in bcd bit shift
#define rtc_RTCPWRUPMTH_DAY		5 // day in week bit shift

#define rtc_RAMSTART_Reg		0x20
#define rtc_RAMEND_Reg			0x5F

#define PWRDN					0
#define PWRUP					1

struct wdtreg {
	unsigned char dtbaseaddress;
	unsigned char dtsec;
	unsigned char dtmin;
	unsigned char dthour;
	unsigned char dtwkday;
	unsigned char dtday;
	unsigned char dtmonth;
	unsigned char dtyear;
	unsigned char control;
	unsigned char osctrim;
};

struct walmreg {
	unsigned char baseaddress;
	unsigned char almsec;
	unsigned char almmin;
	unsigned char almhour;
	unsigned char almwkday;
	unsigned char almdate;
	unsigned char almmonth;
};


struct rdtreg {
	unsigned char dtsec;
	unsigned char dtmin;
	unsigned char dthour;
	unsigned char dtwkday;
	unsigned char dtday;
	unsigned char dtmonth;
	unsigned char dtyear;
	unsigned char control;
	unsigned char osctrim;
};

struct ralmreg {
	unsigned char almsec;
	unsigned char almmin;
	unsigned char almhour;
	unsigned char almwkday;
	unsigned char almdate;
	unsigned char almmonth;
};

struct rpowerreg {
	unsigned char pmin;
	unsigned char phour;
	unsigned char pday;
	unsigned char pmonth;

};

/*	library based on jeelabs RTClib [original library at https://github.com/jcw/rtclib ] altered to support Microchip MCP7940M RTC, used in Arduino
	based embedded environments. To use this library, add #include <MCP7940.h> to the top of your program.*/

class DateTime {	//DateTime class constructs the variable to store RTC Date and Time, and is a direct copy from the original RTClib.
  public:

  DateTime(long t =0);
  DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour=0, uint8_t min=0, uint8_t sec=0);
  DateTime(const char* date, const char* time);
  
  uint16_t year() const  {return 2000+yOff;}
  uint8_t month() const  {return m;}
  uint8_t day() const    {return d;}
  uint8_t hour() const   {return hh;}
  uint8_t minute() const {return mm;}
  uint8_t second() const {return ss;}
  uint8_t DayOfWeek() const;

  long get() const;

  protected:
  uint8_t yOff, m, d, hh, mm, ss;
};

class rtc_MCP7940{	//RTC functions
  public:			
  static void adjust(const DateTime& dt);								//change date and time
  static DateTime now();												//get current time and date from RTC registers
  static bool isrunning();												//check to make sure clock is ticking
  static bool isset();													//check whether clock has been set
  static bool isalarm(uint8_t alarm);									//check whether alarm triggered
  static void setalarm(uint8_t alarm, uint8_t mode, DateTime& at);		//set alarm 
  static void clralarm(uint8_t alarm);									//clear alarm
  
  static DateTime getalarm(uint8_t alarm);								//get the alarm setting
  static DateTime getpupdn(uint8_t reg);									//read power fail/restore registers
  static void poweroff(void);											//turn of the microbit power
  static uint8_t ordinalDate(uint8_t toDay, uint8_t toMonth);			//convert date and month to ordinal (julian) date
  static unsigned long get(){return now().get();}
  
  static uint8_t readSram(uint8_t address);								// read SRAM byte Address 0 to 0x3f
  static void writeSram(uint8_t address, uint8_t data);					// write SRAM byte
  static void readSram(uint8_t address, uint8_t* data, uint8_t length);  // read SRAM bytes
  static void writeSram(uint8_t address, uint8_t* data, uint8_t length); // write SRAM bytes

  static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4);}	//bcd to bin conv (RTC to MCU)
  static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10);}	//bin to bcd conv (MCU to RTC)
 
};

