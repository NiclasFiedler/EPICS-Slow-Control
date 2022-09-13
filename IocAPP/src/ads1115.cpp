#include "saa_i2c.h"
#include "ads1115.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <bitset>

ads1115::ads1115(std::shared_ptr<saa_i2c> i2c, uint16_t _address)
: m_saa_i2c(i2c), address(_address)
{

}

auto ads1115::read(uint8_t channel, uint8_t bit_rate, uint8_t boardID, const bool output) -> float
 {
    /* ADS1115 comparison modes:

            Input1   |  Input2     |    MUX Code
            ---------+-------------+-------------
            AIN0 = 0 |  AIN1 = 1   | -> MUX = 000
            AIN0 = 0 |  AIN3 = 3   | -> MUX = 001
            AIN1 = 1 |  AIN3 = 3   | -> MUX = 010
            AIN2 = 2 |  AIN3 = 3   | -> MUX = 011
            AIN0 = 0 |  GND  = 4   | -> MUX = 100
            AIN1 = 1 |  GND  = 4   | -> MUX = 101
            AIN2 = 2 |  GND  = 4   | -> MUX = 110
            AIN3 = 3 |  GND  = 4   | -> MUX = 111
    */
    
    std::vector<uint8_t> dataraw; //voltage initialisation
    std::vector<uint8_t> writedata = {0x01,0x83,0x83};
    float volt;
    int16_t voltraw;

    writedata[1] |= 0x40;
    writedata[1] |= (channel & 0x03) << 4;           

    m_saa_i2c->i2c_write(address,writedata,bit_rate,boardID,output); //initialise single shot mode and start measuring
    
    m_saa_i2c->i2c_write(address,{0x00},bit_rate,boardID,output); //conversion register
    dataraw = m_saa_i2c->i2c_read(address,0x03,bit_rate,boardID,output);  //read request
    
    voltraw = dataraw[1] << 8;
    voltraw |= dataraw[2];
    volt = 4096. * voltraw / 32768. /1000; //conversion
    
    return volt;
} 

auto ads1115::comp_ch1_ch2(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
{
    return std::abs(ads1115::read(1,bit_rate,boardID,output)-ads1115::read(2,bit_rate,boardID,output));
}

auto ads1115::comp_ch1_ch3(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
{
    return std::abs(ads1115::read(1,bit_rate,boardID,output)-ads1115::read(3,bit_rate,boardID,output));
}

auto ads1115::comp_ch1_ch4(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
{
    return std::abs(ads1115::read(1,bit_rate,boardID,output)-ads1115::read(4,bit_rate,boardID,output));
}

auto ads1115::comp_ch2_ch3(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
{
    return std::abs(ads1115::read(2,bit_rate,boardID,output)-ads1115::read(3,bit_rate,boardID,output));
}

auto ads1115::comp_ch2_ch4(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
{
    return std::abs(ads1115::read(2,bit_rate,boardID,output)-ads1115::read(4,bit_rate,boardID,output));
}   

auto ads1115::comp_ch3_ch4(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
{
    return std::abs(ads1115::read(3,bit_rate,boardID,output)-ads1115::read(4,bit_rate,boardID,output));
}
