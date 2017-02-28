#include "MicroBit.h"
#include "BME280.h"


MicroBit uBit;
BME280 sensor;


int main() {
	uBit.init();	
	uBit.serial.baud(115200);

    while(1) {
		int temp = sensor.getTemperature();
        uBit.serial.printf("%d.%d degC, %d hPa, %d %%\r\n", temp / 10, temp % 10, sensor.getPressure(), sensor.getHumidity());
        uBit.sleep(2000);
    }
	release_fiber();
	return 0;	
}
