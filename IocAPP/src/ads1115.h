#ifndef ADS1115_H
#define ADS1115_H

#include "saa_i2c.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class ads1115
{
public:
    ads1115(std::shared_ptr<saa_i2c> saa_i2c, uint16_t address);
    
    /**
    * Reads voltage of selected channel from ADS1115
    * @param channel Selection of the channel
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured voltage
    */
    auto read(uint8_t channel, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> float;

    /**
    * Reads difference of voltages of channel 1 and 2 from ADS1115
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured voltage
    */
    auto comp_ch1_ch2(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> float;
    
    /**
    * Reads difference of voltages of channel 1 and 3 from ADS1115
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured voltage
    */
    auto comp_ch1_ch3(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> float;
    
    /**
    * Reads difference of voltages of channel 1 and 4 from ADS1115
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured voltage
    */
    auto comp_ch1_ch4(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> float;
    
    /**
    * Reads difference of voltages of channel 2 and 3 from ADS1115
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured voltage
    */
    auto comp_ch2_ch3(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> float;
    
    /**
    * Reads difference of voltages of channel 2 and 4 from ADS1115
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured voltage
    */
    auto comp_ch2_ch4(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> float;
        
    /**
    * Reads difference of voltages of channel 3 and 4 from ADS1115
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns measured voltage
    */
    auto comp_ch3_ch4(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> float;



private:
    std::shared_ptr<saa_i2c> m_saa_i2c;
    uint16_t address;
};

#endif
