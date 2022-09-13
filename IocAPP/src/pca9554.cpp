#include "saa_i2c.h"
#include "pca9554.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <bitset>

pca9554::pca9554(std::shared_ptr<saa_i2c> i2c)
: m_saa_i2c(i2c)
{
    pca9554::set_output(); //output mode (default)
}

void pca9554::toggle_off(uint8_t pin, uint8_t bit_rate, uint8_t boardID, const bool output)
 {
    std::vector<uint8_t> data = {0x01,0x00};
    data[1] = pca9554::status(bit_rate,boardID,output); //current output values
    data[1] &= (0xff ^ (0b1 << pin)); //pin toggle false
    m_saa_i2c->i2c_write(0x20,data,bit_rate,boardID,output); //overwrite of output config 
}

void pca9554::toggle_off(std::vector<uint8_t> pins, uint8_t bit_rate, uint8_t boardID, const bool output)
{
    std::vector<uint8_t> data = {0x01,0x00};
    data[1] = pca9554::status(bit_rate,boardID,output); //current output values
    for(u_int8_t i=0; i<pins.size();i++){
        data[1] &= (0xff ^ (0b1 << i)); //pin toggle false
    }
    m_saa_i2c->i2c_write(0x20,data,bit_rate,boardID,output); //overwrite of output config    
}

void pca9554::toggle_on(uint8_t pin, uint8_t bit_rate, uint8_t boardID, const bool output)
 {
    std::vector<uint8_t> data = {0x01,0x00};
    data[1] = pca9554::status(bit_rate,boardID,output); //current output values
    data[1] |= (0b1 << pin); //pin toggle true
    m_saa_i2c->i2c_write(0x20,data,bit_rate,boardID,output); //overwrite of output config
 }

 void pca9554::toggle_on(std::vector<uint8_t> pins, uint8_t bit_rate, uint8_t boardID, const bool output)
{
    std::vector<uint8_t> data = {0x01,0x00};
    data[1] = pca9554::status(bit_rate,boardID,output); //current output values
    for(u_int8_t i=0; i<pins.size();i++){
        data[1] |= (0b1 << i); //pin toggle true
    }
    m_saa_i2c->i2c_write(0x20,data,bit_rate,boardID,output); //overwrite of output config    
}

auto pca9554::status(uint8_t bit_rate, uint8_t boardID, const bool output) -> uint8_t
 {
    std::vector<uint8_t> data;
    
    m_saa_i2c->i2c_write(0x20,{0x01},bit_rate,boardID,output); 
    data = m_saa_i2c->i2c_read(0x20,2,bit_rate,boardID,output); //return current output values
    return data[1];
 }

void pca9554::set_output(uint8_t bit_rate, uint8_t boardID, const bool output)
 {
    m_saa_i2c->i2c_write(0x20,{0x03,0x00},bit_rate,boardID,output); //sets all pins to output mode
}

void pca9554::toggle_apfel_off(uint8_t bit_rate, uint8_t boardID, const bool output) 
{
    pca9554::toggle_off({0,1,2,3},bit_rate,boardID,output);
}

void pca9554::toggle_apfel_on(uint8_t bit_rate, uint8_t boardID, const bool output) 
{
    pca9554::toggle_on({0,1,2,3},bit_rate,boardID,output);
}

auto pca9554::status_apfel(uint8_t bit_rate, uint8_t boardID, const bool output) -> std::vector<bool>
{
    std::vector<bool> statusapfel;
    uint8_t status = pca9554::status(bit_rate,boardID,output);
    statusapfel.push_back((status >> 0) & 1);
    statusapfel.push_back((status >> 1) & 1);
    statusapfel.push_back((status >> 2) & 1);
    statusapfel.push_back((status >> 3) & 1);
    return statusapfel;
}

auto pca9554::status_apfel_pin(uint8_t pin, uint8_t bit_rate, uint8_t boardID, const bool output) -> bool
{
    std::vector<bool> statusapfel = pca9554::status_apfel(bit_rate,boardID,output);
    return statusapfel[pin];
}

void pca9554::toggle_led_off(uint8_t bit_rate, uint8_t boardID, const bool output) 
{
    pca9554::toggle_off(4,bit_rate,boardID,output);
}

void pca9554::toggle_led_on(uint8_t bit_rate, uint8_t boardID, const bool output) 
{
    pca9554::toggle_on(4,bit_rate,boardID,output);
}

auto pca9554::status_led(uint8_t bit_rate, uint8_t boardID, const bool output) -> bool
{
    uint8_t status = pca9554::status(bit_rate,boardID,output);
    return (status >> 4) & 1;
}
