#include "saa_i2c.h"
#include "serialadapter_aux_spi.h"
#include "spi.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>

saa_i2c::saa_i2c(unsigned f_baud_rate, unsigned f_n_asics, const bool output)
: m_spi(std::make_shared<serialadapter_aux_spi>(f_baud_rate,f_n_asics,output))
{
    
}

auto saa_i2c::i2c_write(uint16_t address, uint8_t command, uint8_t bit_rate, uint8_t boardID, const bool output) -> bool
{
    std::vector<uint8_t> commands;
    commands.push_back(command);    
    return saa_i2c::i2c_write(address, commands, bit_rate, boardID, output);
}

auto saa_i2c::i2c_write(uint16_t address, std::vector<uint8_t> commands, uint8_t bit_rate, uint8_t boardID, const bool output) -> bool
{
    m_spi->write16bit(0x01,address,boardID,output); //init
    m_spi->write8bit(0x02,commands.size(),boardID,output);
    m_spi->write8bit(0x03,bit_rate,boardID,output);

    for(uint8_t i = 0; i < commands.size();i++){   //command in register
        m_spi->write8bit(4+i,commands[i],boardID,output);
    }
 
    m_spi->write8bit(0x00,0x04,boardID,output);      //send command to asic

    while(m_spi->read8bit(0x00,boardID,output) == 0x44){
        usleep(20000); // wait for i2c to finish
        //std::cout << "ich muss noch warten!" << std::endl;
    }

    if(m_spi->read8bit(0x00,boardID,output) >= 0x80){
        return true;
        
    }    
    else {
        // throw std::runtime_error(std::string("I2C write access failed."));   
        return false;
    }
}

auto saa_i2c::i2c_write_slow(uint16_t address, std::vector<uint8_t> commands, uint8_t bit_rate, uint8_t boardID, const bool output) -> bool
{
    m_spi->write16bit(0x01,address,boardID,output); //init
    m_spi->write8bit(0x02,commands.size(),boardID,output);
    m_spi->write8bit(0x03,bit_rate,boardID,output);

    for(uint8_t i = 0; i < commands.size();i++){   //command in register
        m_spi->write8bit(4+i,commands[i],boardID,output);
    }
 
    m_spi->write8bit(0x00,0x04,boardID,output);      //send command to asic

    while(m_spi->read8bit(0x00,boardID,output) == 0x44){
        usleep(85000); // wait for i2c to finish
        //std::cout << "ich muss noch warten!" << std::endl;
    }

    if(m_spi->read8bit(0x00,boardID,output) >= 0x80){
        return true;
        
    }    
    else {
        // throw std::runtime_error(std::string("I2C write access failed."));   
        return false;
    }
}

auto saa_i2c::i2c_write(std::vector<uint16_t> address, std::vector<std::vector<uint8_t>> commands, std::vector<bool> mask, std::vector<uint8_t> bit_rate, const bool output) -> bool
{
    std::vector<uint8_t> commands_size;
    for(uint8_t i = 0; i < commands.size(); i++) {
        commands_size.push_back(commands[i].size());
    }
    
    m_spi->write16bit(std::vector<uint8_t>(address.size(),0x01),address,mask,output); //init
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x02),commands_size,mask,output);
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x03),bit_rate,mask,output);

    for(uint8_t j = 0; j < address.size(); j++){
        if(mask[j] == 1) {
            for(uint8_t i = 0; i < commands[j].size();i++){   //command in register
                m_spi->write8bit(4+i,commands[j][i],j,output);
            }
        }
    }
 
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x00),std::vector<uint8_t>(address.size(),0x04),mask,output);      //send command to asic

        usleep(85000); // wait for i2c to finish

    if(m_spi->read8bit(std::vector<uint8_t>(address.size(),0x00),mask,output) == std::vector<uint8_t>(address.size(),0x80)){
        return true;
    }    
    else {
        // throw std::runtime_error(std::string("I2C write access failed."));   
        return false;
    }
}

auto saa_i2c::i2c_write(std::vector<uint16_t> address, std::vector<std::vector<uint8_t>> commands, std::vector<uint8_t> bit_rate, const bool output) -> bool
{
    std::vector<uint8_t> commands_size;
    for(uint8_t i = 0; i < commands.size(); i++) {
        commands_size.push_back(commands[i].size());
    }
    m_spi->write16bit(std::vector<uint8_t>(address.size(),0x01),address,output); //init
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x02),commands_size,output);
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x03),bit_rate,output);

    for(uint8_t j = 0; j < address.size(); j++){
        for(uint8_t i = 0; i < commands[j].size();i++){   //command in register
            m_spi->write8bit(4+i,commands[j][i],j,output);
        }
    }
 
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x00),std::vector<uint8_t>(address.size(),0x04),output);      //send command to asic

        usleep(85000); // wait for i2c to finish

    if(m_spi->read8bit(std::vector<uint8_t>(address.size(),0x00),output) == std::vector<uint8_t>(address.size(),0x80)){
        return true;
    }    
    else {
        // throw std::runtime_error(std::string("I2C write access failed."));   
        return false;
    }
}

auto saa_i2c::i2c_read(uint16_t address, uint8_t n_bytes, uint8_t bit_rate, uint8_t boardID, const bool output) -> std::vector<uint8_t>
{
    std::vector<uint8_t> data;

    m_spi->write16bit(0x01,address,boardID,output); //init
    m_spi->write8bit(0x02,n_bytes,boardID,output);
    m_spi->write8bit(0x03,bit_rate,boardID,output);
    m_spi->write8bit(0x00,0x05,boardID,output);      //send command to asic

    while(static_cast<int>(m_spi->read8bit(0x00,boardID,output)) == 0x45){
        usleep(10000); // wait for i2c to finish before reading
    }

    if(m_spi->read8bit(0x00,boardID,output) >= 0x81){
        for(uint8_t i = 0; i < n_bytes;i++){   //command in register
            data.push_back(m_spi->read8bit(4+i,boardID,output));
        }
        return data;
    }    
    else {
        // throw std::runtime_error(std::string("I2C read access failed."));   
        return {0};
    }
}

auto saa_i2c::i2c_read(std::vector<uint16_t> address, std::vector<uint8_t> n_bytes, std::vector<uint8_t> bit_rate, const bool output) -> std::vector<std::vector<uint8_t>>
{
    std::vector<std::vector<uint8_t>> data;

    m_spi->write16bit(std::vector<uint8_t>(address.size(),0x01),address,output); //init
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x02),n_bytes,output);
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x03),bit_rate,output);
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x00),std::vector<uint8_t>(address.size(),0x05),output);      //send command to asic

    usleep(85000); // wait for i2c to finish before reading

    if(m_spi->read8bit(std::vector<uint8_t>(address.size(),0x00),output) == std::vector<uint8_t>(address.size(),0x81)){
        for(uint8_t j = 0; j < address.size();j++){
            for(uint8_t i = 0; i < n_bytes[j]; i++){   //command in register
                data[j].push_back(m_spi->read8bit(4+i,j,output));
            }
        }
        return data;
    }
    return {{0}};
}

auto saa_i2c::i2c_read(std::vector<uint16_t> address, std::vector<uint8_t> n_bytes, std::vector<bool> mask,std::vector<uint8_t> bit_rate, const bool output) -> std::vector<std::vector<uint8_t>>
{
    std::vector<std::vector<uint8_t>> data;

    m_spi->write16bit(std::vector<uint8_t>(address.size(),0x01),address,mask,output); //init
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x02),n_bytes,mask,output);
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x03),bit_rate,mask,output);
    m_spi->write8bit(std::vector<uint8_t>(address.size(),0x00),std::vector<uint8_t>(address.size(),0x05),mask,output);      //send command to asic

    usleep(85000); // wait for i2c to finish before reading

    if(m_spi->read8bit(std::vector<uint8_t>(address.size(),0x00),mask,output) == std::vector<uint8_t>(address.size(),0x81)){
        for(uint8_t j = 0; j < address.size();j++){
            if(mask[j] == 1) {
                for(uint8_t i = 0; i < n_bytes[j]; i++){   //command in register
                    data[j].push_back(m_spi->read8bit(4+i,j,output));
                }
            }
        }
        return data;
    }
    return {{0}};
}
