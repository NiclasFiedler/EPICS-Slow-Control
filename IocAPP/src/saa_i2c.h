#ifndef SAA_I2C_H
#define SAA_I2C_H

#include "spi.h"
#include "serialadapter_aux_spi.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class saa_i2c
{
public:
    saa_i2c(unsigned f_baud_rate, unsigned f_n_asics, const bool output);
    
    /**
    * Writes one command to the I2C bus of one SAA
    * @param address Address of device
    * @param command Command to be sent
    * @param bit_rate Frequency of sent bits
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns status of the I2C access
    */
    auto i2c_write(uint16_t address, uint8_t command, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> bool;
    
    /**
    * Writes multiple commands to the I2C bus of one SAA
    * @param address Address of device
    * @param commands Commands to be sent
    * @param bit_rate Frequency of sent bits
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns status of the I2C access
    */
    auto i2c_write(uint16_t address, std::vector<uint8_t> commands, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> bool;

    /**
    * Writes multiple commands to the I2C bus of one SAA with a bigger delay for the SHT21
    * @param address Address of device
    * @param commands Commands to be sent
    * @param bit_rate Frequency of sent bits
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns status of the I2C access
    */
    auto i2c_write_slow(uint16_t address, std::vector<uint8_t> commands, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> bool;
    
    /**
    * Writes multiple commands to the I2C bus of all SAAs
    * @param address Addresses of devices
    * @param commands Commands to be sent
    * @param bit_rate Frequencies of sent bits
    * @param output Choose if raw data should be printed
    * @return Returns status of the I2C access
    */
    auto i2c_write(std::vector<uint16_t> address, std::vector<std::vector<uint8_t>> commands, std::vector<uint8_t> bit_rate, const bool output) -> bool;
    
    /**
    * Writes multiple commands to the I2C bus of selected SAAs
    * @param address Addresses of devices
    * @param commands Commands to be sent
    * @param mask Selection of the SAAs
    * @param bit_rate Frequencies of sent bits
    * @param output Choose if raw data should be printed
    * @return Returns status of the I2C access
    */
    auto i2c_write(std::vector<uint16_t> address, std::vector<std::vector<uint8_t>> commands, std::vector<bool> mask, std::vector<uint8_t> bit_rate, const bool output) -> bool;
    
    /**
    * Reads data from the I2C bus of one SAA
    * @param address Address of device
    * @param n_bytes Number of bytes to be read
    * @param bit_rate Frequency of sent bits
    * @param boardID Position of the SAA in chain
    * @param output Choose if raw data should be printed
    * @return Returns read data
    */
    auto i2c_read(uint16_t address, uint8_t n_bytes, uint8_t bit_rate = 0x80, uint8_t boardID = 0, const bool output = 0) -> std::vector<uint8_t>;

    /**
    * Reads data from the I2C bus of all SAAs
    * @param address Addresses of devices
    * @param n_bytes Number of bytes to be read
    * @param bit_rate Frequencies of sent bits
    * @param output Choose if raw data should be printed
    * @return Returns read data
    */
    auto i2c_read(std::vector<uint16_t> address, std::vector<uint8_t> n_bytes, std::vector<uint8_t> bit_rate, const bool output = 0) -> std::vector<std::vector<uint8_t>>;

    /**
    * Reads data from the I2C bus of selected SAAs
    * @param address Addresses of devices
    * @param n_bytes Number of bytes to be read
    * @param bit_rate Frequencies of sent bits
    * @param mask Selection of the SAAs
    * @param output Choose if raw data should be printed
    * @return Returns read data
    */
    auto i2c_read(std::vector<uint16_t> address, std::vector<uint8_t> n_bytes, std::vector<bool> mask,std::vector<uint8_t> bit_rate, const bool output) -> std::vector<std::vector<uint8_t>>;

private:
    std::shared_ptr<serialadapter_aux_spi> m_spi;
};

#endif // end of SAA_I2C_H
