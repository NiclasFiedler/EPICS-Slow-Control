#ifndef PCA9554_H
#define PCA9554_H

#include "saa_i2c.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class pca9554
{
public:
    pca9554(std::shared_ptr<saa_i2c> saa_i2c);
    
    /**
    * Toggles pin off
    * @param pin Pin selection
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void toggle_off(uint8_t pin, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0);

    /**
    * Toggles pins off
    * @param pins Pin selection
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void toggle_off(std::vector<uint8_t> pins, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0);

    /**
    * Toggles pin on
    * @param pin Pin selection
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void toggle_on(uint8_t pin, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0);

    /**
    * Toggles pins on
    * @param pins Pin selection
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void toggle_on(std::vector<uint8_t> pins, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0);

    /**
    * Returns the current status for all pins
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns status of all pins as bit sequence
    */
    auto status(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> uint8_t;

    /**
    * Sets all pins in output mode
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void set_output(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0);

    /**
    * Toggles all APFEL asics off
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void toggle_apfel_off(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0);

    /**
    * Toggles all APFEL asics on
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void toggle_apfel_on(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0);

    /**
    * Returns the current status for all APFEL asics
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns status of all pins with APFEL asics
    */
    auto status_apfel(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> std::vector<bool>;
    
    /**
    * Returns the current status for one APFEL asic
    * @param pin Pin selection
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns status of all pins with APFEL asics
    */
    auto status_apfel_pin(uint8_t pin, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> bool;

    /**
    * Toggles LED off
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void toggle_led_off(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0); 

    /**
    * Toggles LED on
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    */
    void toggle_led_on(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0);

    /**
    * Returns the current status for the LED
    * @param bit_rate Set bit rate
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns status of the LED pin
    */
    auto status_led(uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> bool;

private:
    std::shared_ptr<saa_i2c> m_saa_i2c;
};

#endif
