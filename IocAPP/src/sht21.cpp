#include "saa_i2c.h"
#include "sht21.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>

sht21::sht21(std::shared_ptr<saa_i2c> i2c)
: m_saa_i2c(i2c)
{
    
}

auto sht21::read(uint8_t bit_rate, uint8_t boardID, const bool output) -> std::vector<float>
 {
    std::vector<float> data; //temperature and humidity initialisation
    std::vector<uint8_t> dataraw;
    float temp;                                  
    float hum;
    uint16_t tempraw;
    uint16_t humraw;
    
    m_saa_i2c->i2c_write(0x40,{0xfe},bit_rate,boardID,output); //soft reset
    m_saa_i2c->i2c_write(0x40,{0xf5},bit_rate,boardID,output); //measure request
    dataraw = m_saa_i2c->i2c_read(0x40,0x03,bit_rate,boardID,output);  //read request
    
    humraw = dataraw[1] << 8;
    humraw |= dataraw[2];
    hum = -6 +125*humraw / 65535.;  

    m_saa_i2c->i2c_write(0x40,{0xfe},bit_rate,boardID,output); //soft reset
    m_saa_i2c->i2c_write(0x40,{0xf3},bit_rate,boardID,output); //measure request
    dataraw = m_saa_i2c->i2c_read(0x40,0x03,bit_rate,boardID,output);  //read request
    
    tempraw = dataraw[1] << 8;
    tempraw |= dataraw[2];
    temp = -46.85 + 175.72 * tempraw / 65535.;
    
    data.push_back(static_cast<float>(hum));
    data.push_back(static_cast<float>(temp));

    return data;
}

auto sht21::read_temp(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
 {
    std::vector<uint8_t> dataraw; //temperature initialisation
    float temp;                                  
    uint16_t tempraw; 

    m_saa_i2c->i2c_write_slow(0x40,{0xfe},bit_rate,boardID,output); //soft reset
    m_saa_i2c->i2c_write_slow(0x40,{0xf3},bit_rate,boardID,output); //measure request
    dataraw = m_saa_i2c->i2c_read(0x40,0x03,bit_rate,boardID,output);  //read request
    
    tempraw = dataraw[1] << 8;
    tempraw |= dataraw[2];
    temp = -46.85 + 175.72 * tempraw / 65535.;
    
    return temp;    
}

auto sht21::read_hum(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
 {                                 
    std::vector<uint8_t> dataraw; //humidity initialisation
    float hum;
    uint16_t humraw;
    
    m_saa_i2c->i2c_write_slow(0x40,{0xfe},bit_rate,boardID,output); //soft reset
    m_saa_i2c->i2c_write_slow(0x40,{0xf5},bit_rate,boardID,output); //measure request
    dataraw = m_saa_i2c->i2c_read(0x40,0x03,bit_rate,boardID,output);  //read request
    
    humraw = dataraw[1] << 8;
    humraw |= dataraw[2];
    hum = -6 +125*humraw / 65535.;  

    return hum;
}
