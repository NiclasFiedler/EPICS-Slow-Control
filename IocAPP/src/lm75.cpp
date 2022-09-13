#include "saa_i2c.h"
#include "lm75.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <bitset>

lm75::lm75(std::shared_ptr<saa_i2c> i2c)
: m_saa_i2c(i2c)
{
    
}

auto lm75::read(uint8_t bit_rate, uint8_t boardID, const bool output) -> float  //sht31
 {
    std::vector<uint8_t> dataraw; //temperature initialisation
    int16_t data;
    float temp;
    
    dataraw = m_saa_i2c->i2c_read(0x4c,0x03,bit_rate,boardID,output);  //read request
    
    data = dataraw[1] << 8;
    data |= (dataraw[2] & 0x80);

    temp = data / 256.;
    
    return temp;    
}
