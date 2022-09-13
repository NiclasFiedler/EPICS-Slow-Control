#ifndef SHT21_H
#define SHT21_H

#include "saa_i2c.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class sht21
{
public:
    sht21(std::shared_ptr<saa_i2c> saa_i2c);
    
    /**
    * Reads temperature and humidity from SHT21
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured temperature and humidity
    */
    auto read(uint8_t bit_rate = 0x80, uint8_t boardID = 0,const bool output = 0) -> std::vector<float>;
    
    /**
    * Reads temperature from SHT21
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured temperature
    */
    auto read_temp(uint8_t bit_rate = 0x80, uint8_t boardID = 0,const bool output = 0) -> float;
    
    /**
    * Reads humidity from SHT21
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured humidity
    */
    auto read_hum(uint8_t bit_rate = 0x80, uint8_t boardID = 0,const bool output = 0) -> float;

private:
    std::shared_ptr<saa_i2c> m_saa_i2c;
};

#endif
