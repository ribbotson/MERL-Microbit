#include "DS1820.h"
#include "MicroBit.h"

extern MicroBit uBit;
 
 
DS1820::DS1820(){
	
	int i;
	for(i = 0; i<MaxSensors; i++) DSROMId[i] = 0;
	uBit.io.P0.setDigitalValue(1);
	uBit.io.P0.setPull(PullUp);
	uBit.io.P1.getDigitalValue();
}

 
bool DS1820::onewire_reset() {
// This will return false if no devices are present on the data bus
    bool presence=false;
    volatile int i;
    
    uBit.io.P0.setDigitalValue(0);
	for (i = 0; i < 650; i++) {
	};
	uBit.io.P0.setDigitalValue(1);
	for (i = 0; i < 50; i++) {
	};
	if (uBit.io.P1.getDigitalValue() == 0)
		presence = true;
	for (i = 0; i < 650; i++) {
	};
    return presence;
}
 
void DS1820::onewire_bit_out (bool bit_data) {
	volatile int i;
	int delay1, delay2;
	if (bit_data) {
		delay1 = 1;
		delay2 = 80;
	} 
	else {
		delay1 = 75;
		delay2 = 6;
	}
	uBit.io.P0.setDigitalValue(0);
	for (i = 1; i < delay1; i++) {
	};
	uBit.io.P0.setDigitalValue(1);
	for (i = 1; i < delay2; i++) {
	};
}
 
void DS1820::onewire_byte_out(char data) { // output data character (least sig bit first).
    int n;
    for (n=0; n<8; n++) {
        onewire_bit_out(data & 0x01);
        data = data >> 1; // now the next bit is in the least sig bit position.
    }
}
 
bool DS1820::onewire_bit_in() {
    bool answer;
	volatile int i;
	
	uBit.io.P0.setDigitalValue(0);
	uBit.io.P0.setDigitalValue(1);
	for (i = 1; i < 20; i++) {
	};
	answer = uBit.io.P1.getDigitalValue();
	for (i = 1; i < 60; i++) {
	};
	return answer;

}
 
char DS1820::onewire_byte_in() { // read byte, least sig byte first
    char answer = 0x00;
    int i;
    for (i=0; i<8; i++) {
        answer = answer >> 1; // shift over to make room for the next bit
        if (onewire_bit_in())
            answer = answer | 0x80; // if the data port is high, make this bit a 1
    }
    return answer;
}

 
bool DS1820::unassignedProbe() {
    return search_ROM_routine(0xF0);
}
 
bool DS1820::search_ROM_routine(char command) {
    bool DS1820_done_flag = false;
    int DS1820_last_descrepancy = 0;
    char DS1820_search_ROM[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    
    int descrepancy_marker, ROM_bit_index;
    bool Bit_A, Bit_B;
    uint8_t byte_counter, bit_mask;
 
    while (!DS1820_done_flag) {
        if (!onewire_reset()) {
            return false;
        } else {
            ROM_bit_index=1;
            descrepancy_marker=0;
            char command_shift = command;
            for (int n=0; n<8; n++) {           // Search ROM command or Search Alarm command
                onewire_bit_out(command_shift & 0x01);
                command_shift = command_shift >> 1; // now the next bit is in the least sig bit position.
            } 
            byte_counter = 0;
            bit_mask = 0x01;
            while (ROM_bit_index<=64) {
                Bit_A = onewire_bit_in();
                Bit_B = onewire_bit_in();
                if (Bit_A & Bit_B) {
                    descrepancy_marker = 0; // data read error, this should never happen
                    ROM_bit_index = 0xFF;
                } 
                else if (Bit_A | Bit_B) {
					// Set ROM bit to Bit_A
					if (Bit_A) {
						DS1820_search_ROM[byte_counter] = DS1820_search_ROM[byte_counter] | bit_mask; // Set ROM bit to one
					} else {
						DS1820_search_ROM[byte_counter] = DS1820_search_ROM[byte_counter] & ~bit_mask; // Set ROM bit to zero
					}
				} 
				else {
					uBit.serial.printf("Two\r\n");
					// both bits A and B are low, so there are two or more devices present
					if ( ROM_bit_index == DS1820_last_descrepancy ) {
						DS1820_search_ROM[byte_counter] = DS1820_search_ROM[byte_counter] | bit_mask; // Set ROM bit to one
					} else {
						if ( ROM_bit_index > DS1820_last_descrepancy ) {
							DS1820_search_ROM[byte_counter] = DS1820_search_ROM[byte_counter] & ~bit_mask; // Set ROM bit to zero
							descrepancy_marker = ROM_bit_index;
						} else {
							if (( DS1820_search_ROM[byte_counter] & bit_mask) == 0x00 )
                                    descrepancy_marker = ROM_bit_index;
						}
					}
				}
				onewire_bit_out (DS1820_search_ROM[byte_counter] & bit_mask);
				ROM_bit_index++;
				if (bit_mask & 0x80) {
					byte_counter++;
					bit_mask = 0x01;
				} else {
					bit_mask = bit_mask << 1;
				}
                
            }
            DS1820_last_descrepancy = descrepancy_marker;
            if (ROM_bit_index != 0xFF) {
                int i,j;
                uint64_t ROMAddress = 0;
                for(i = 0; i < 8; i++){
					ROMAddress |= ((uint64_t) DS1820_search_ROM[i] << (i * 8));	
				}
                
                for(j=0; j < MaxSensors; j++ ){
                    if (DSROMId[j] == 0) {   // empty entry
						DSROMId[j] = ROMAddress;    
						return true;                      
                    }
                    else {                    //Otherwise, check if ROM is already known                      
                        if(DSROMId[j] == ROMAddress) break;                      
                    }
                }                        
            }
        }
        if (DS1820_last_descrepancy == 0)
            DS1820_done_flag = true;
    }
    return false;
}
 
void DS1820::match_ROM(uint64_t address) {
// Used to select a specific device
    int i;
    uint64_t shift_addr = address;
    onewire_reset();
    onewire_byte_out( 0x55);  //Match ROM command
    for (i=0;i<8;i++) {
        onewire_byte_out(shift_addr & 0xff);
        shift_addr >>= 8;       
    }
}
 
void DS1820::skip_ROM() {
    onewire_reset();
    onewire_byte_out(0xCC);   // Skip ROM command
}

void DS1820::set_ROM(int sensor, uint64_t address){
	if(sensor < MaxSensors) DSROMId[sensor] = address;	
}

uint64_t DS1820::get_ROM(int sensor){
	uint64_t RomCode;
	if(sensor >= MaxSensors) return 0;
	RomCode = DSROMId[sensor];
	return RomCode;
}

 
bool DS1820::ROM_checksum_error(char *_ROM_address) {
    char _CRC=0x00;
    int i;
    for(i=0;i<7;i++) // Only going to shift the lower 7 bytes
        _CRC = CRC_byte(_CRC, _ROM_address[i]);
    // After 7 bytes CRC should equal the 8th byte (ROM CRC)
    return (_CRC!=_ROM_address[7]); // will return true if there is a CRC checksum mis-match         
}
 
bool DS1820::RAM_checksum_error() {
    char _CRC=0x00;
    int i;
    for(i=0;i<8;i++) // Only going to shift the lower 8 bytes
        _CRC = CRC_byte(_CRC, RAM[i]);
    // After 8 bytes CRC should equal the 9th byte (RAM CRC)
    return (_CRC!=RAM[8]); // will return true if there is a CRC checksum mis-match        
}
 
char DS1820::CRC_byte (char _CRC, char byte ) {
    int j;
    for(j=0;j<8;j++) {
        if ((byte & 0x01 ) ^ (_CRC & 0x01)) {
            // DATA ^ LSB CRC = 1
            _CRC = _CRC>>1;
            // Set the MSB to 1
            _CRC = _CRC | 0x80;
            // Check bit 3
            if (_CRC & 0x04) {
                _CRC = _CRC & 0xFB; // Bit 3 is set, so clear it
            } else {
                _CRC = _CRC | 0x04; // Bit 3 is clear, so set it
            }
            // Check bit 4
            if (_CRC & 0x08) {
                _CRC = _CRC & 0xF7; // Bit 4 is set, so clear it
            } else {
                _CRC = _CRC | 0x08; // Bit 4 is clear, so set it
            }
        } else {
            // DATA ^ LSB CRC = 0
            _CRC = _CRC>>1;
            // clear MSB
            _CRC = _CRC & 0x7F;
            // No need to check bits, with DATA ^ LSB CRC = 0, they will remain unchanged
        }
        byte = byte>>1;
    }
return _CRC;
}
 
int DS1820::convertTemperature (int sensor, bool wait) {
    // Convert temperature into scratchpad RAM for all devices at once
    int delay_time = 750; // Default delay time

    match_ROM(DSROMId[sensor]);
    onewire_byte_out( 0x44);  // perform temperature conversion
	if(wait){
		uBit.sleep(delay_time);
		return(0);
	}
	else return delay_time;
}

int DS1820::convertTemperature (bool wait) {
    // Convert temperature into scratchpad RAM for all devices at once
    int delay_time = 750; // Default delay time

    skip_ROM();
    onewire_byte_out( 0x44);  // perform temperature conversion
	if(wait){
		uBit.sleep(delay_time);
		return(0);
	}
	else return delay_time;
}
 
void DS1820::read_RAM(int sensor) {
    // This will copy the DS1820's 9 bytes of RAM data
    // into the objects RAM array. Functions that use
    // RAM values will automaticly call this procedure.
    int i;
    //skip_ROM();
    match_ROM(DSROMId[sensor]);             // Select this device
    onewire_byte_out( 0xBE);   //Read Scratchpad command
    for(i=0;i<9;i++) {
        RAM[i] = onewire_byte_in();
    }

}

bool DS1820::setResolution(int sensor, unsigned int resolution) {
    bool answer = false;
    resolution = resolution - 9;
    if (resolution < 4) {
        resolution = resolution<<5; // align the bits
        RAM[4] = (RAM[4] & 0x60) | resolution; // mask out old data, insert new
        write_scratchpad (sensor, (RAM[2]<<8) + RAM[3]);
        answer = true;
    }
    return answer;
}
 
void DS1820::write_scratchpad(int sensor, int data) {
    RAM[3] = data;
    RAM[2] = data>>8;
    match_ROM(DSROMId[sensor]);
    onewire_byte_out(0x4E);   // Copy scratchpad into DS1820 ram memory
    onewire_byte_out(RAM[2]); // T(H)
    onewire_byte_out(RAM[3]); // T(L)
    onewire_byte_out(RAM[4]); // Configuration register
    
}
 
int DS1820::temperature(int sensor) {
// The data specs state that count_per_degree should be 0x10 (16)

    int reading;
    read_RAM(sensor);
    if (RAM_checksum_error())
        // Indicate we got a CRC error
        reading = invalid_conversion;
    else {
        reading = (RAM[1] << 8) + RAM[0];
        if (reading & 0x8000) { // negative degrees C
            reading = 0-((reading ^ 0xffff) + 1); // 2's comp then convert to signed int
        }
        reading = reading * 10 / 16;
    }
    return reading;
}

