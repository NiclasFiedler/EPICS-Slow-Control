#ifndef LM75_H
#define LM75_H

#include "saa_i2c.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class lm75
{
public:
    lm75(std::shared_ptr<saa_i2c> saa_i2c);
    
    /**
    * Reads temperature from LM75
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured temperature
    */
    auto read(uint8_t bit_rate = 0x80, uint8_t boardID = 0,const bool output = 0) -> float;

private:
    std::shared_ptr<saa_i2c> m_saa_i2c;
};

#endif
