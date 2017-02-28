#include "MicroBit.h"
#include "ADS1115.h"


MicroBit uBit;
ADS1115 ads;


int main() {
	uBit.init();	
	uBit.serial.baud(115200);
	  
	uBit.serial.printf("Getting single-ended readings from AIN0..3");
	uBit.serial.printf("ADC Range: +/- 6.144V (1 bit = 0.1875mV)");
	
	  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1115
  //                                                                -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.0078125mV

	ads.begin();

    while(1) {
		
		int16_t adc0, adc1, adc2, adc3;

		adc0 = ads.readADC_SingleEnded(0);
		adc1 = ads.readADC_SingleEnded(1);
		adc2 = ads.readADC_SingleEnded(2);
		adc3 = ads.readADC_SingleEnded(3);
		uBit.serial.printf("AIN0: %d\r\n", adc0);
		uBit.serial.printf("AIN1: %d\r\n", adc1);
		uBit.serial.printf("AIN2: %d\r\n", adc2);
		uBit.serial.printf("AIN3: %d\r\n", adc3);

		uBit.sleep(1000);
		
    }
	release_fiber();
	return 0;	
}
