#ifndef SERIALADAPTER_AUX_SPI_H
#define SERIALADAPTER_AUX_SPI_H

#include <cstdint>
#include <vector>
#include <string>

class serialadapter_aux_spi
{
public:
    serialadapter_aux_spi(unsigned f_baud_rate = 32000, unsigned f_n_asics = 1, const bool output = 0);

    /**
    * Prints a data string byte for byte
    * @param data Data string
    * @param prefix Comment to be put before the output
    * @param output Choose if raw data should be printed
    */
    void print_hex(const std::string& data,  const std::string &prefix = "", const bool output = 0);

    /**
    * Prints a vector of data strings byte for byte
    * @param data Vector of data strings
    * @param prefix Comment to be put before the output
    * @param output Choose if raw data should be printed
    */
    void print_hex(const std::vector<std::string>& data, const std::string &prefix, const bool output = 0);

    /**
    * Writes 8bit word to SAA
    * @param address Address of register that should be written to (must be a 8bit register)
    * @param data Data as 1 byte for SAA
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    * @return Returns true if byte write is successful
    */
    auto write8bit(uint8_t address, uint8_t data, std::size_t i_saa = 0, const bool output = 0) -> bool;

    /**
    * Writes 8bit words to SAA daisychain, writing to all SAAs
    * @param address Addresses of registers that should be written to (must be a 8bit registers)
    * @param data Data vector with 8bit payloads
    * @param output Choose if raw data should be printed
    * @return Returns true if write is successful
    */
    auto write8bit(const std::vector<uint8_t> &address, const std::vector<uint8_t> &data, const bool output = 0) -> bool;

    /**
    * Writes 8bit words to SAA daisychain, writing to selected SAAs
    * @param address Addresses of registers that should be written to (must be a 8bit registers)
    * @param data Data vector with 8bit payloads
    * @param mask Mask which serial adapter asic should be written to
    * @param output Choose if raw data should be printed    
    * @return Returns true if write is successful
    */
    auto write8bit(const std::vector<uint8_t> &address, const std::vector<uint8_t> &data, const std::vector<bool> &mask, const bool output = 0) -> bool;

    /**
    * Writes 16bit word to SAA
    * @param address Address of register that should be written to (must be a 16bit register)
    * @param data Data as 16bit payload for SAA
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    * @return Returns true if write is successful
    */
    auto write16bit(uint8_t address, uint16_t data, std::size_t i_saa = 0, const bool output = 0) -> bool;

    /**
    * Writes 16bit words to SAA daisychain, writing to every SAA
    * @param address Addresses of registers that should be written to (must be a 16bit registers)
    * @param data Data vector with 16bit payloads
    * @param output Choose if raw data should be printed
    * @return Returns true if write is successful
    */
    auto write16bit(const std::vector<uint8_t> &address, const std::vector<uint16_t> &data, const bool output = 0) -> bool;

    /**
    * Writes 16bit words to SAA daisychain, writing to selected SAAs
    * @param address Addresses of registers that should be written to (must be a 16bit registers)
    * @param data Data vector with 16bit payloads
    * @param mask Mask which serial adapter asic should be written to
    * @param output Choose if raw data should be printed
    * @return Returns true if write is successful
    */
    auto write16bit(const std::vector<uint8_t> &address, const std::vector<uint16_t> &data, const std::vector<bool> &mask, const bool output = 0) -> bool;

    /**
    * Writes 32bit word to SAA
    * @param address Address of register that should be written to (must be a 32bit register)
    * @param data Data as 32bit payload for SAA
    * @param i_saa Select which SAA should be written to in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    * @return Returns true if byte write is successful
    */
    auto write32bit(uint8_t address, uint32_t data, std::size_t i_saa = 0, const bool output = 0) -> bool;

    /**
    * Writes 32bit words to all SAAs in the daisychain
    * @param address Addresses of registers that should be written to (must be a 32bit registers)
    * @param data Data vector with 32bit payloads
    * @param output Choose if raw data should be printed
    * @return Returns true if write is successful
    */
    auto write32bit(const std::vector<uint8_t> &address, const std::vector<uint32_t> &data, const bool output = 0) -> bool;

    /**
    * Writes 32bit words to selected SAAs in the daisychain
    * @param address Addresses of registers that should be written to (must be a 32bit registers)
    * @param data Data vector with 32bit payloads
    * @param mask Mask which serial adapter asic should be written to
    * @param output Choose if raw data should be printed
    * @return Returns true if write is successful
    */
    auto write32bit(const std::vector<uint8_t> &address, const std::vector<uint32_t> &data, const std::vector<bool> &mask, const bool output = 0) -> bool;

    /**
    * Reads 8bit register from SAA
    * @param address Address of register that should be read from (must be a 8bit register)
    * @param i_saa Select which SAA should be read from in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    * @return Returns 8bit value of the register
    */
    auto read8bit(uint8_t address, std::size_t i_saa = 0, const bool output = 0) -> uint8_t;

    /**
    * Reads 8bit registers from all SAAs in the daisychain
    * @param address Addresses of registers that should be read from (must be a 8bit registers)
    * @param output Choose if raw data should be printed
    * @return Returns 8bit values of the registers
    */
    auto read8bit(const std::vector<uint8_t> &address, const bool output = 0) -> std::vector<uint8_t>;

    /**
    * Reads 8bit registers from selected SAAs in the daisychain
    * @param address Addresses of registers that should be read from (must be a 8bit registers)
    * @param mask Mask which serial adapter asic should be read from
    * @param output Choose if raw data should be printed
    * @return Returns 8bit values of the registers
    */
    auto read8bit(const std::vector<uint8_t> &address, const std::vector<bool> &mask, const bool output = 0) -> std::vector<uint8_t>;

    /**
    * Reads 16bit register from SAA
    * @param address Address of register that should be read from (must be a 16bit register)
    * @param i_saa Select which SAA should be read from in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    * @return Returns 16bit value of the register
    */
    auto read16bit(uint8_t address, std::size_t i_saa = 0, const bool output = 0) -> uint16_t;

    /**
    * Reads 16bit registers from all SAAs in the daisychain
    * @param address Addresses of registers that should be read from (must be a 16bit registers)
    * @param output Choose if raw data should be printed
    * @return Returns 16bit values of the registers
    */
    auto read16bit(const std::vector<uint8_t> &address, const bool output = 0) -> std::vector<uint16_t>;

    /**
    * Reads 16bit registers from selected SAAs in the daisychain
    * @param address Addresses of registers that should be read from (must be a 16bit registers)
    * @param mask Mask which serial adapter asic should be read from
    * @param output Choose if raw data should be printed
    * @return Returns 16bit values of all registers even if not read from
    */
    auto read16bit(const std::vector<uint8_t> &address, const std::vector<bool> &mask, const bool output = 0) -> std::vector<uint16_t>;

    /**
    * Reads 32bit register from SAA
    * @param address Address of register that should be read from (must be a 32bit register)
    * @param i_saa Select which SAA should be read from in the daisychain, default is 0 for single SAA
    * @param output Choose if raw data should be printed
    * @return Returns 32bit value of the register
    */
    auto read32bit(uint8_t address, std::size_t i_saa = 0, const bool output = 0) -> uint32_t;

    /**
    * Reads 32bit registers from all SAAs in the daisychain
    * @param address Addresses of registers that should be read from (must be a 32bit registers)
    * @param output Choose if raw data should be printed
    * @return Returns 32bit values of the registers
    */
    auto read32bit(const std::vector<uint8_t> &address, const bool output = 0) -> std::vector<uint32_t>;

    /**
    * Reads 32bit registers from selected SAAs in the daisychain
    * @param address Addresses of registers that should be read from (must be a 32bit registers)
    * @param mask Mask which serial adapter asic should be read from
    * @param output Choose if raw data should be printed
    * @return Returns 32bit values of the registers
    */
    auto read32bit(const std::vector<uint8_t> &address, const std::vector<bool> &mask, const bool output = 0) -> std::vector<uint32_t>;

    private:

    /**
    * Creates string representation of the data vector
    * @param data Data bytes to be written
    * @return Returns the string representation of the vector
    */
    auto serialize(const std::vector<uint8_t> &data) -> std::string;

    /**
    * Creates vector representation of the data string
    * @param data String representation of data
    * @param skip_even Take only the uneven bytes, dropping the bits 9 and 10 in the 10bit representation
    * @return Vector of data bytes
    */
    auto deserialize(const std::string& data, bool skip_msb = true) -> std::vector<uint8_t>;

    /**
    * Initializes m_n_asics number of asics
    * @param output Choose if raw data should be printed
    */
    auto init_Asic(const bool output = 0) -> bool;

    void saa_index_error(std::size_t i_saa);
    void address_data_mismatch_error(std::size_t address_len, std::size_t data_len);
    void address_mask_mismatch_error(std::size_t address_len, std::size_t mask_len);
    unsigned m_baud_rate;
    unsigned m_n_asics;
};

#endif // SERIALADAPTER_AUX_SPI_H
