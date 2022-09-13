#ifndef HVBOARD_H
#define HVBOARD_H

#include "saa_i2c.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "sht21.h"
#include "sht31.h"
#include "ads1115.h"
#include "lm75.h"
#include "eeprom.h"
#include "pca9554.h"
#include "apfel.h"

class hvboard
{
public:
    hvboard(unsigned n_asics = 1, unsigned baud_rate = 32000, bool output = false);

    struct hv_channel {
        size_t dac_ch;
        uint16_t adc_addr;
        size_t volt_ch;
        size_t curr_ch;
    };
    inline static const std::map<size_t, hv_channel> channel_table {
        {1,{2,0x4a,1,0}},
        {2,{1,0x4a,2,3}},
        {3,{0,0x48,1,0}},
        {4,{3,0x48,2,3}},
        {5,{4,0x49,2,3}},
        {6,{7,0x49,1,0}},
        {7,{6,0x4b,2,3}},
        {8,{5,0x4b,1,0}},
    };
    std::shared_ptr<serialadapter_aux_spi> m_spi;

    std::shared_ptr<saa_i2c> m_saa;
    
    std::shared_ptr<sht21> m_sht21;
    std::shared_ptr<sht31> m_sht31;
    std::shared_ptr<ads1115> m_ads12;
    std::shared_ptr<ads1115> m_ads34;
    std::shared_ptr<ads1115> m_ads56;
    std::shared_ptr<ads1115> m_ads78;
    std::shared_ptr<lm75> m_lm75;
    std::shared_ptr<pca9554> m_pca;
    std::shared_ptr<eeprom> m_eeprom;
    std::shared_ptr<apfel> m_apfel;

    /**
    * Reads measured voltage from channel
    * @param channel Selection of the channel
    * @param bit_rate Set bit rate
    * @param output Choose if raw data should be printed
    * @return Returns measured voltage in V 
    */
    auto readVoltage(size_t channel, uint8_t boardID, uint8_t bit_rate = 0x80, const bool output = 0) -> float;

    /**
    * Reads measured current from channel
    * @param channel Selection of the channel
    * @param bit_rate Set bit rate
    * @param output Choose if raw data should be printed
    * @return Returns measured current in nA
    */
    auto readCurrent(size_t channel, uint8_t boardID, uint8_t bit_rate = 0x80, const bool output = 0) -> float;

    /**
    * Sets voltage via DAC
    * @param channel Selection of the channel
    * @param DAC_value Sets DAC value
    */
    void setVoltage(size_t channel, uint16_t DAC_value, uint8_t boardID);

    /**
    * Reads DAC values
    * @param channel Selection of the channel
    * @return Returns the DAC value
    */
    auto getDAC(size_t channel, uint8_t boardID) -> uint16_t;
    
    auto getHvChannelCount() const -> std::size_t;

private:

};

#endif
