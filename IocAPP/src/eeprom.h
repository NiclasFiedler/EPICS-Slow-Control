#ifndef EEPROM_H
#define EEPROM_H

#include "saa_i2c.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class eeprom
{
public:
    eeprom(std::shared_ptr<saa_i2c> saa_i2c);
    
   /**
    * Reads data from EEPROM
    * @param reg Adress of the first register that shall be read
    * @param n_reg Number of registers that shall be read
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns data that is stored in the registers
    */
    auto read_nbytes(uint8_t reg, uint8_t n_reg, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> std::vector<uint8_t>;

    /**
    * Reads one byte from EEPROM
    * @param reg Adress of the register that shall be read
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns data that is stored in the register
    */
    auto read(uint8_t reg, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> uint8_t;
    
    /**
    * Writes data to EEPROM
    * @param reg Adress of the first register that shall be written
    * @param data Data that shall be written on register by register
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void write(uint8_t reg,std::vector<uint8_t> data, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0);

    /**
    * Writes one byte to EEPROM
    * @param reg Adress of the register that shall be written
    * @param data Data that shall be written
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void write(uint8_t reg, uint8_t data, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0); 

private:
    std::shared_ptr<saa_i2c> m_saa_i2c;
};

#endif
