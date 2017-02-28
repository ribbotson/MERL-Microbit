

#include "TCS34275.h"
#include "MicroBit.h"


/* Example code for the Adafruit TCS34725 breakout library */

   
/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */

MicroBit uBit;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

int main() {
  	uBit.init();	
	uBit.serial.baud(115200);

  
	if (tcs.begin()) {
		uBit.serial.printf("Found sensor\r\n");
	} else {
		uBit.serial.printf("No TCS34725 found ... check your connections\r\n");
		while (1);
	}


	while(1) {
		uint16_t r, g, b, c, colorTemp, lux;
	  
		tcs.getRawData(&r, &g, &b, &c);
		colorTemp = tcs.calculateColorTemperature(r, g, b);
		lux = tcs.calculateLux(r, g, b);
	  
		uBit.serial.printf("Color Temp: %d K - ", colorTemp); 
		uBit.serial.printf("Lux: %d - ", lux); 
		uBit.serial.printf("R: %d ", r); 
		uBit.serial.printf("G: %d ", g); 
		uBit.serial.printf("B: %d ", b); 
		uBit.serial.printf("C: %d ", c); 
		uBit.serial.printf("\r\n ");
	}
	release_fiber();
	return 0;		
}
