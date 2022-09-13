#include "hvboard.h"
#include "spi.h"
#include "serialadapter_aux_spi.h"
#include "saa_i2c.h"
#include "sht21.h"
#include "sht31.h"
#include "ads1115.h"
#include "lm75.h"
#include "eeprom.h"
#include "pca9554.h"
#include "apfel.h"
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <map>
#include <bitset>

hvboard::hvboard(unsigned n_asics, unsigned baud_rate, bool output)
:     m_spi(std::make_shared<serialadapter_aux_spi>(baud_rate, n_asics, output)),
      m_saa(std::make_shared<saa_i2c>(baud_rate, n_asics, output)),
      m_eeprom(std::make_shared<eeprom>(m_saa)),
      m_ads12(std::make_shared<ads1115>(m_saa, 0x4a)),
      m_ads34(std::make_shared<ads1115>(m_saa, 0x48)),
      m_ads56(std::make_shared<ads1115>(m_saa, 0x49)),
      m_ads78(std::make_shared<ads1115>(m_saa, 0x4b)),
      m_lm75(std::make_shared<lm75>(m_saa)),
      m_sht21(std::make_shared<sht21>(m_saa)),
      m_sht31(std::make_shared<sht31>(m_saa)),
      m_pca(std::make_shared<pca9554>(m_saa)),
      m_apfel(std::make_shared<apfel>(m_spi))
{
    // does pca have to be set here?
    // pca.setOutputPorts(0x0f);
    // pca.setOutputState(0x0f);
}

auto hvboard::readVoltage(size_t channel, uint8_t boardID, uint8_t bit_rate, const bool output) -> float
{
    if (channel_table.at(channel).adc_addr == 0x4a)
        return m_ads12->read(channel_table.at(channel).volt_ch, bit_rate, boardID, output) * 134.33;
    else if (channel_table.at(channel).adc_addr == 0x48)
        return m_ads34->read(channel_table.at(channel).volt_ch, bit_rate, boardID, output) * 134.33;
    else if (channel_table.at(channel).adc_addr == 0x49)
        return m_ads56->read(channel_table.at(channel).volt_ch, bit_rate, boardID, output) * 134.33;
    else if (channel_table.at(channel).adc_addr == 0x4b)
        return m_ads78->read(channel_table.at(channel).volt_ch, bit_rate, boardID, output) * 134.33;
    else
        return 0.;
}

auto hvboard::readCurrent(size_t channel, uint8_t boardID , uint8_t bit_rate, const bool output) -> float
{
    if (channel_table.at(channel).adc_addr == 0x4a)
        return m_ads12->read(channel_table.at(channel).curr_ch, bit_rate, boardID, output);
    else if (channel_table.at(channel).adc_addr == 0x48)
        return m_ads34->read(channel_table.at(channel).curr_ch, bit_rate, boardID, output);
    else if (channel_table.at(channel).adc_addr == 0x49)
        return m_ads56->read(channel_table.at(channel).curr_ch, bit_rate, boardID, output);
    else if (channel_table.at(channel).adc_addr == 0x4b)
        return m_ads78->read(channel_table.at(channel).curr_ch, bit_rate, boardID, output);
    else
        return 0.;
}

void hvboard::setVoltage(size_t channel, uint16_t DAC_value, uint8_t boardID)
{
    if (channel_table.at(channel).adc_addr == 0x4a)
        m_spi->write16bit(channel_table.at(channel).dac_ch + 0x40, DAC_value,boardID);
    else if (channel_table.at(channel).adc_addr == 0x48)
        m_spi->write16bit(channel_table.at(channel).dac_ch + 0x40, DAC_value,boardID);
    else if (channel_table.at(channel).adc_addr == 0x49)
        m_spi->write16bit(channel_table.at(channel).dac_ch + 0x40, DAC_value,boardID);
    else if (channel_table.at(channel).adc_addr == 0x4b)
        m_spi->write16bit(channel_table.at(channel).dac_ch + 0x40, DAC_value,boardID);
}

auto hvboard::getDAC(size_t channel, uint8_t boardID) -> uint16_t
{
    if(channel_table.at(channel).adc_addr == 0x4a) return m_spi->read16bit(channel_table.at(channel).dac_ch + 0x40,boardID);
    else if(channel_table.at(channel).adc_addr == 0x48) return m_spi->read16bit(channel_table.at(channel).dac_ch + 0x40,boardID);
    else if(channel_table.at(channel).adc_addr == 0x49) return m_spi->read16bit(channel_table.at(channel).dac_ch + 0x40,boardID);
    else if(channel_table.at(channel).adc_addr == 0x4b) return m_spi->read16bit(channel_table.at(channel).dac_ch + 0x40,boardID);
    return 0;
}

auto hvboard::getHvChannelCount() const -> std::size_t
{
    return channel_table.size();
}
