/* mbed DS1820 Library, for the Dallas (Maxim) 1-Wire Digital Thermometer
 * Copyright (c) 2010, Michael Hagberg Michael@RedBoxCode.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MBED_DS1820_H
#define MBED_DS1820_H

#include "MicroBit.h"

#define MaxSensors 4   
#define all_devices 0
#define invalid_conversion -1000

/** DS18B20 Dallas 1-Wire Temperature Probe
 *
 */

 
class DS1820{
public:

    
    
    DS1820(); // Constructor
    
    void set_ROM(int sensor, uint64_t address);
    uint64_t get_ROM(int sensor);


    /** Function to see if there are DS1820 devices left which are not known
    *
    * @return - true if there are one or more unassigned devices, otherwise false
      */
    bool unassignedProbe();

    /** This routine will initiate the temperature conversion within
      * one or all DS1820 probes. 
      *
      * @param wait if true, waits up to 750 ms for 
      * conversion otherwise returns immediatly.
      * @param device allows the function to apply to a specific device or
      * to all devices on the 1-Wire bus.
      * @returns milliseconds untill conversion will complete.
      */
    int convertTemperature(int sensor, bool wait);
    
    
   int convertTemperature(bool wait);

    /** This function will return the probe temperature. Approximately 10ms per
      * probe to read its RAM, do CRC check and convert temperature.
      *
      * @returns temperature for that scale in 1/10th degree, or DS1820::invalid_conversion (-1000) if CRC error detected.
      */
    int temperature(int sensor);

    /** This function sets the temperature resolution for the DS18B20
      * in the configuration register.
      *
      * @param a number between 9 and 12 to specify resolution
      * @returns true if successful
      */ 
    bool setResolution(int sensor, unsigned int resolution);       
	uint64_t DSROMId[MaxSensors];
 
private:

    char CRC_byte(char _CRC, char byte);
    bool onewire_reset();
    void match_ROM(uint64_t address);
    void skip_ROM();
    bool search_ROM_routine(char command);
    void onewire_bit_out (bool bit_data);
    void onewire_byte_out(char data);
    bool onewire_bit_in();
    char onewire_byte_in();
    bool ROM_checksum_error(char *_ROM_address);
    bool RAM_checksum_error();
    void read_RAM(int sensor);
    void write_scratchpad(int sensor, int data);

    
    char RAM[9];
 
    
 };  


#endif
