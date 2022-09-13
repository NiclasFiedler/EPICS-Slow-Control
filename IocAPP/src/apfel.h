#ifndef APFEL_H
#define APFEL_H

#include "saa_i2c.h"
#include <cstdint>
#include <string>
#include <memory>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <bitset>
#include <Python.h>
#include <fstream>
#include <sstream>
#include <iterator>
#include <map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>

class apfel
{
public:
    apfel(std::shared_ptr<serialadapter_aux_spi> saa_spi);

    struct apfel_dac { //parameters for choosing an APFEL channel
        size_t chip_id; //position of the APFEL ASIC on the backplane (1-4)
        size_t ch; //APD on the crystal (1-2)
        size_t gain; //High(1)/low(0) gain (0-1)
    };

    struct parameters { //important parameters connected to APFEL DACs
        size_t sadc_ch; //channel of the SADC (0-31)
        size_t dac; //main DAC for shapping of the signal (1-3)
        size_t hv_ch; //HV channel for changing the voltage (1-8)
    };

    const std::map<size_t, parameters> sadc_map { //connection between APFEL channel and other parameters
        {210,{0,3,1}},
        {211,{1,1,1}},
        {310,{2,3,5}},
        {321,{3,2,6}},
        {221,{4,2,2}},
        {220,{5,3,2}},
        {320,{6,3,6}},
        {311,{7,1,5}},
        {110,{8,3,7}},
        {120,{9,3,8}},
        {410,{10,3,3}},
        {421,{11,2,4}},
        {121,{12,2,8}},
        {111,{13,1,7}},
        {420,{14,3,4}},
        {411,{15,1,3}},
    };
    
    /**
    * Sets DAC of the APFEL ASIC to a given value
    * @param chip_ID Position of the chip on the backplane
    * @param dac_channel Channel of the DAC
    * @param data DAC value to be set
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    */
    void set_DAC(uint8_t chip_ID, uint8_t dac_channel, uint16_t data, std::size_t i_saa = 0, const bool output = 0);
    
    /**
    * Reads current DAC value of the APFEL ASIC
    * @param chip_ID Position of the chip on the backplane
    * @param dac_channel Channel of the DAC
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    * @return Returns current DAC value
    */
    auto read_DAC(uint8_t chip_ID, uint8_t dac_channel, std::size_t i_saa = 0, const bool output = 0) -> uint16_t;
    
    /**
    * Sets amplifier to a specific mode
    * @param chip_ID Position of the chip on the backplane
    * @param channel Selection of channel (1 or 2)
    * @param mode Selection of mode (16 or 32)
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    */
    void set_amp_mode(uint8_t chip_ID, uint8_t channel, uint8_t mode, std::size_t i_saa = 0, const bool output = 0);

    /**
    * Resets all DACs of one APFEL-ASIC
    * @param chip_ID Position of the chip on the backplane
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    */
    void global_reset(uint8_t chip_ID, std::size_t i_saa = 0, const bool output = 0);

    /**
    * Sends a testpulse from the APFEL ASIC
    * @param chip_ID Position of the chip on the backplane
    * @param channel Selection of channel (1 or 2)
    * @param data Testpulse value to be set
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    */
    void testpulse(uint8_t chip_ID, uint8_t channel, uint8_t data, std::size_t i_saa = 0, const bool output = 0);

    /**
    * Sends n testpulses from the APFEL ASIC with a given delay
    * @param chip_ID Position of the chip on the backplane
    * @param channel Selection of channel (1 or 2)
    * @param data Testpulse value to be set
    * @param n Number of cycles 
    * @param t Delay between cycles (in ms)
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    */
    void testpulse_toggle(uint8_t chip_ID, uint8_t channel, uint8_t data, uint32_t t = 0, std::size_t i_saa = 0, const bool output = 0);

    /**
    * Hardware-autocalibrates the DACs of one APFEL ASIC
    * @param chip_ID Position of the chip on the backplane
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    */
    void autocalib(uint8_t chip_ID, std::size_t i_saa = 0, const bool output = 0);

    /**
    * Hardware-autocalibrates the DACs of all APFEL ASICs
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    */
    void autocalib_all(std::size_t i_saa = 0, const bool output = 0);

    /**
    * Converts a apfel_dac into a hash for sadc_map
    * @param v_apfel apfel_dac to be converted
    * @return Returns converted hash
    */
    auto apfel2hash(apfel_dac v_apfel) -> size_t;

    /**
    * Converts a hash for the sadc_map into a apfel_dac
    * @param hash hash to be converted
    * @return Returns converted apfel_dac
    */
    auto hash2apfel(size_t hash) -> apfel_dac;

    /**
    * Converts bin file from SADC to usable data
    * @param path Path of the bin file
    * @return Returns traces as a vector
    */
    auto bin2txt(std::string path) -> std::vector<std::vector<std::vector<float>>>;

    /**
    * Starts a measurement with the SADC and calculates the height of the peak
    * @param v_apfel Channel to be measured
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @return Returns height of the peak
    */
    auto getHeight(apfel_dac v_apfel, std::size_t i_saa = 0) -> double;

    /**
    * Software-autocalibrates one channel of one APFEL
    * @param v_apfel Channel to be autocalibrated
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param dac_value_output Toggle for output of the changing DAC values for debugging
    * @return Returns optimal DAC values
    */
    auto sw_autocalib_one_ch(apfel_dac v_apfel, std::size_t i_saa = 0, bool dac_value_output = false) -> std::vector<int>;

    /**
    * Software-autocalibrates all channels of one APFEL
    * @param chip_id APFEL to be autocalibrated
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param dac_value_output Toggle for output of the changing DAC values for debugging
    * @return Returns optimal DAC values
    */
    auto sw_autocalib_all_ch(uint8_t chip_id, std::size_t i_saa = 0, bool dac_value_output = false) -> std::vector<int>;

    /**
    * Software-autocalibrates all low-gain-channels of one APFEL
    * @param chip_id APFEL to be autocalibrated
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param dac_value_output Toggle for output of the changing DAC values for debugging    
    * @return Returns optimal DAC values
    */
    auto sw_autocalib_lg(uint8_t chip_id, std::size_t i_saa = 0, bool dac_value_output = false) -> std::vector<int>;

    /**
    * Software-autocalibrates all APFEL-ASICs
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param dac_value_output Toggle for output of the changing DAC values for debugging    
    * @return Returns optimal DAC values
    */
    auto sw_autocalib_all(std::size_t i_saa = 0, bool dac_value_output = false) -> std::vector<std::vector<int>>;
    

private:
    std::shared_ptr<serialadapter_aux_spi> m_spi;
};

#endif
