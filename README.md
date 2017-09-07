# MERL-Microbit
Here is c code developed for an IoT device to monitor the growth of sugar beet at [MERL](https://www.reading.ac.uk/TheMERL/) The Museum of English Rural Life, Reading. The hardware is the BBC Microbit with the addition of [LoRaWAN_MicroBit](https://github.com/ribbotson/LoRaWAN_MicroBit)

## Description
The IoT device enables the following sensors to send environment data over a LoRaWAN connection to the [The Things Network](https://www.thethingsnetwork.org/):

* Four of soil temperature sensors. DS1820 sensors on One wire Bus

+ Air temperature, air pressure, air humidity. BME280 sensor on I2C bus

+ Light intensity and colour. TCS34275 sensor on I2C bus

+ Soil Humidity. Chirp sensor on I2C Bus

+ Solar power supply Monitor. ADS1115 on I2C Bus

Libraries are also provided for the Microchip RN2483 LoRaWAN radio, and for the Microchip MCP7940 Real Time clock used to keep the IOT device asleep most of the time

## Hardware Configuration.
I2C devices are connected using a grove breakout
One wire devices are on the digital I/O of the LoRaWAN Microbit

## Software Configuration
The software is developed in c using the mbed libraries. Development environment is yotta

## Authors

* **Richard Ibbotson**  



## License



## Acknowledgments

* All the folks at [rLab](http://www.rlab.org.uk) Reading Hackspace especially mikethebee
+ [MERL](https://www.reading.ac.uk/TheMERL/) The Museum of English Rural Life
+ [The Things Network](https://www.thethingsnetwork.org/)


