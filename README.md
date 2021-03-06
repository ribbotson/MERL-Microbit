# MERL-Microbit
Here is C/C++ code developed for an IoT device to monitor the growth of sugar beet at [MERL](https://www.reading.ac.uk/TheMERL/) The Museum of English Rural Life, Reading. The hardware is the BBC Microbit with the addition of [LoRaWAN_MicroBit](https://github.com/ribbotson/LoRaWAN_MicroBit)

## Description
The IoT device enables the following sensors to send environment data over a LoRaWAN connection to the [The Things Network](https://www.thethingsnetwork.org/):

* Four of soil temperature sensors. [DS18S20](https://datasheets.maximintegrated.com/en/ds/DS18S20.pdf) sensors on One wire Bus

+ Air temperature, air pressure, air humidity. [BME280](https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280_DS001-11.pdf) sensor on I2C bus

+ Light intensity and colour. [TCS34275](https://cdn-shop.adafruit.com/datasheets/TCS34725.pdf) sensor on I2C bus

+ Soil Humidity. [Chirp](https://www.tindie.com/products/miceuz/i2c-soil-moisture-sensor/) sensor on I2C Bus

+ Solar power supply Monitor. [ADS1115]( http://www.ti.com/lit/ds/symlink/ads1115.pdf) on I2C Bus

Libraries are also provided for the devices on the LoRaWAN_Microbit including the Microchip [RN2483]( http://ww1.microchip.com/downloads/en/DeviceDoc/50002346C.pdf) LoRaWAN radio, and the Microchip [MCP7940N](http://ww1.microchip.com/downloads/en/DeviceDoc/20005010F.pdf) Real Time clock used to keep the IOT device asleep most of the time

## Hardware Configuration.
I2C devices are connected using a grove breakout
One wire devices are on the digital I/O of the LoRaWAN Microbit. Breakout boards for the I2C devices were sourced for Adafruit and from eBay suppliers. DS18S20 sensors with cables and waterproof tubes were sourced from eBay.

## Software Configuration
The software is developed in c using the mbed libraries. Development environment is yotta.
Code presented here is from the source directories of the yotta projects. You will need to make yotta projects and then add the source code. A excellent description of how to do this for the BBC Microbit is here: [micro:bit IoT in C](http://www.iot-programmer.com/index.php/books/27-micro-bit-iot-in-c/chapters-micro-bit-iot-in-c/44-offline-c-c-development-with-the-micro-bit)



## Authors

* **Richard Ibbotson**  richard.ibbotson@ribbotson.com

## License

The software and associated documentation is provided under the MIT LIcense


MIT License

Copyright (c) 2017 Richard Ibbotson


Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.




## Acknowledgments

* All the folks at [rLab](http://www.rlab.org.uk) Reading Hackspace especially mikethebee
+ [MERL](https://www.reading.ac.uk/TheMERL/) The Museum of English Rural Life
+ [The Things Network](https://www.thethingsnetwork.org/)


