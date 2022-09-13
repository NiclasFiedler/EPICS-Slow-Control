#include "saa_i2c.h"
#include "sht31.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>

sht31::sht31(std::shared_ptr<saa_i2c> i2c)
: m_saa_i2c(i2c)
{
    
}

auto sht31::read(uint8_t bit_rate, uint8_t boardID,const bool output) -> std::vector<float>
 {
    std::vector<float> data; //temperature and humidity initialisation
    std::vector<uint8_t> dataraw;
    float temp;                                  
    float hum;
    uint16_t tempraw;
    uint16_t humraw;
    
    m_saa_i2c->i2c_write(0x44,{0x30,0xa2},bit_rate,boardID,output); //soft reset
    m_saa_i2c->i2c_write(0x44,{0x24,0x16},bit_rate,boardID,output); //measure request
    dataraw = m_saa_i2c->i2c_read(0x44,0x07,bit_rate,boardID,output);  //read request
    
    humraw = dataraw[4] << 8;
    humraw |= dataraw[5];
    hum = 100. * humraw / 65535.; 
    
    tempraw = dataraw[1] << 8;
    tempraw |= dataraw[2];
    temp = -45 + 175. * tempraw / 65535.;
    
    data.push_back(static_cast<float>(hum));
    data.push_back(static_cast<float>(temp));

    return data;
}

auto sht31::read_temp(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
 {
    std::vector<uint8_t> dataraw; //temperature initialisation
    float temp;                                  
    uint16_t tempraw;
    
    m_saa_i2c->i2c_write(0x44,{0x30,0xa2},bit_rate,boardID,output); //soft reset
    m_saa_i2c->i2c_write(0x44,{0x24,0x16},bit_rate,boardID,output); //measure request
    dataraw = m_saa_i2c->i2c_read(0x44,0x07,bit_rate,boardID,output);  //read request
    
    tempraw = dataraw[1] << 8;
    tempraw |= dataraw[2];
    temp = -45 + 175. * tempraw / 65535.;

    return temp;    
}

auto sht31::read_hum(uint8_t bit_rate, uint8_t boardID, const bool output) -> float
 {
    std::vector<uint8_t> dataraw; //humidity initialisation                               
    float hum;
    uint16_t humraw;
    
    m_saa_i2c->i2c_write(0x44,{0x30,0xa2},bit_rate,boardID,output); //soft reset
    m_saa_i2c->i2c_write(0x44,{0x24,0x16},bit_rate,boardID,output); //measure request
    dataraw = m_saa_i2c->i2c_read(0x44,0x07,bit_rate,boardID,output);  //read request
    
    humraw = dataraw[4] << 8;
    humraw |= dataraw[5];
    hum = 100. * humraw / 65535.; 

    return hum;    
}
