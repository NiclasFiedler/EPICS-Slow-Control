#include "saa_i2c.h"
#include "eeprom.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>

eeprom::eeprom(std::shared_ptr<saa_i2c> i2c)
: m_saa_i2c(i2c){}

auto eeprom::read_nbytes(uint8_t reg, uint8_t n_reg, uint8_t bit_rate, uint8_t boardID, const bool output) -> std::vector<uint8_t>
{
    std::vector<uint8_t> dataraw; 
    std::vector<uint8_t> data;

    m_saa_i2c->i2c_write(0x50,reg,bit_rate,boardID,output);
    dataraw = m_saa_i2c->i2c_read(0x50,(n_reg+1),bit_rate,boardID,output);  //read request
    dataraw.erase(dataraw.begin());
    return dataraw;
}

auto eeprom::read(uint8_t reg, uint8_t bit_rate, uint8_t boardID, const bool output) -> uint8_t
{
    std::vector<uint8_t> dataraw; 
    dataraw = eeprom::read_nbytes(reg,1,bit_rate,boardID,output);

    return dataraw[0];
}


void eeprom::write(uint8_t reg, std::vector<uint8_t> data, uint8_t bit_rate, uint8_t boardID, const bool output)
{
    if(reg + data.size() > 0x80){
        std::cout << "Registers beyond 0x80 are write protected." << std::endl;
    }
    else if(data.size() > 8){
        std::cout << "Only 8 registers can be written simultaneously." << std::endl;
    }
    else if(reg%8 + data.size() > 8){
        std::cout << "Exceeding boundaries of page write." << std::endl;
    }
    else {
        data.insert(data.begin(),reg);
        m_saa_i2c->i2c_write(0x50,data,bit_rate,boardID,output);
    }
}

void eeprom::write(uint8_t reg, uint8_t data, uint8_t bit_rate, uint8_t boardID, const bool output)
{
    std::vector<uint8_t> datavec;
    datavec.push_back(data); 
    eeprom::write(reg,datavec,bit_rate,boardID,output);
}
