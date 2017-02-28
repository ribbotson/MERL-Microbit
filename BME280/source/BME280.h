/**
 *  BME280 Combined humidity and pressure sensor library
 *
 *  @author  Toyomasa Watarai
 *  @version 1.0
 *  @date    06-April-2015
 *
 *  Library for "BME280 temperature, humidity and pressure sensor module" from Switch Science
 *    https://www.switch-science.com/catalog/2236/
 *
 *  For more information about the BME280:
 *    http://ae-bst.resource.bosch.com/media/products/dokumente/bme280/BST-BME280_DS001-10.pdf
 */
 
#ifndef MBED_BME280_H
#define MBED_BME280_H

#include "MicroBit.h"


#define DEFAULT_SLAVE_ADDRESS (0x76 << 1)


 
/** BME280 class
 *
 *  BME280: A library to correct environmental data using Boshe BME280 device
 *
 *  BME280 is an environmental sensor
 *  @endcode
 */
 
class BME280
{
public:

    /** Create a BME280 instance
     * @param slave_adr (option) I2C-bus address (default: 0x76)
     */
    BME280(char slave_adr = DEFAULT_SLAVE_ADDRESS);


    /** Destructor of BME280
     */
    virtual ~BME280();

    /** Initializa BME280 sensor
     *
     *  Configure sensor setting and read parameters for calibration
     *
     */
    void initialize(void);

    /** Read the current temperature value (1/10th degree Celsius) from BME280 sensor
     *
     */
    int getTemperature(void);

    /** Read the current pressure value (hectopascal)from BME280 sensor
     *
     */
    int getPressure(void);

    /** Read the current humidity value (humidity %) from BME280 sensor
     *
     */
    int getHumidity(void);

private:

    char        address;
    uint16_t    dig_T1;
    int16_t     dig_T2, dig_T3;
    uint16_t    dig_P1;
    int16_t     dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    uint16_t    dig_H1, dig_H3;
    int16_t     dig_H2, dig_H4, dig_H5, dig_H6;
    int32_t     t_fine;

};

#endif // MBED_BME280_H
