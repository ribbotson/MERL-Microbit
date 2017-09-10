



#include "MicroBit.h"
#include "DS1820.h"

#define MAX_PROBES      4
 
DS1820 sensor;
MicroBit uBit;
 
void printx64(uint64_t pvalue){
	uint64_t shift_value;
	char shift_char;
	for(int i=60; i >= 0; i -= 4){
		shift_value = (pvalue >> i) & 0x0f;
		if (shift_value > 9) shift_char = 'A' - 10 + shift_value;
		else shift_char = '0' + shift_value;
		uBit.serial.send(shift_char);
	}
}
 
 
int main() {  
	int tempnow;
	uBit.init();
	
	uBit.serial.printf("DS18B20 Search Devices\r\n");
	for(int i = 0; i < MAX_PROBES; i++){
		uBit.serial.printf("%d: ", i);
		printx64(sensor.get_ROM(i));
		uBit.serial.printf("\r\n");
	}
//	for(int i = 0; i < MAX_PROBES; i++){
//		if(sensor.unassignedProbe()) uBit.serial.printf("Found Sensor\r\n") ;
//		uBit.sleep(1000);
//	}

//Merl
//	sensor.set_ROM( 0, 0x5903165563DAFF28); //White Sensor South Bottom
//	sensor.set_ROM( 1, 0x4303165579E0FF28); //Black Sensor North Bottom
//	sensor.set_ROM( 2, 0x110416589CB6FF28); //Blue Sensor South Top
//	sensor.set_ROM( 3, 0x820316557FACFF28); //Red Sensor North Top

//Home
	sensor.set_ROM( 0, 0x6E041684217FFF28); //White Sensor South Bottom
	sensor.set_ROM( 1, 0x720516824298FF28); //Black Sensor North Bottom
	sensor.set_ROM( 2, 0x66031681E7D5FF28); //Blue Sensor South Top
	sensor.set_ROM( 3, 0xD90316877F04FF28); //Red Sensor North Top
	for(int i = 0; i < MAX_PROBES; i++){
		uBit.serial.printf("%d: ", i);
		printx64(sensor.get_ROM(i));
		uBit.serial.printf("\r\n");

	}
    while(1) {
		sensor.convertTemperature(true);
		tempnow = sensor.temperature(0);
		uBit.serial.printf("Temperature: %d.%d", tempnow/10, tempnow % 10);
		uBit.sleep(100);
		tempnow = sensor.temperature(1);
		uBit.serial.printf(" %d.%d ", tempnow/10, tempnow % 10);
		uBit.sleep(100);
		tempnow = sensor.temperature(2);
		uBit.serial.printf(" %d.%d ", tempnow/10, tempnow % 10);
		uBit.sleep(100);
		tempnow = sensor.temperature(3);
		uBit.serial.printf(" %d.%d\r\n", tempnow/10, tempnow % 10);
		uBit.sleep(1000);
    }
	release_fiber();
}
