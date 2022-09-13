#include "serialadapter_aux_spi.h"
#include "spi.h"
#include <string>
#include <unistd.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <vector>

serialadapter_aux_spi::serialadapter_aux_spi(unsigned f_baud_rate, unsigned f_n_asics, const bool output)
 : m_baud_rate{std::move(f_baud_rate)}
 , m_n_asics{std::move(f_n_asics)}
{
    SPI::initialise(m_baud_rate);
    if(!init_Asic(output)){
        throw std::runtime_error("Failed to initialize the serial adapter asic.");
    }
    usleep(2000);
}

void serialadapter_aux_spi::saa_index_error(std::size_t i_saa)
{

    throw std::runtime_error(std::string(
        "Tried to access SAA index " + std::to_string(i_saa) + " but only " + std::to_string(m_n_asics) + " are initialized. Keep in mind that i_saa is indexed starting at 0."));
}

void serialadapter_aux_spi::address_data_mismatch_error(std::size_t address_len, std::size_t data_len)
{
    throw std::runtime_error(std::string(
        "Address and data vector mismatch. Tried to write " + std::to_string(data_len) + " bytes to daisychain but " + std::to_string(address_len)
        + " addresses were given."));
}

void serialadapter_aux_spi::address_mask_mismatch_error(std::size_t address_len, std::size_t mask_len)
{
    throw std::runtime_error(std::string(
        "Address and mask vector mismatch. Tried with " + std::to_string(mask_len) + " entries in mask but " + std::to_string(address_len)
        + " addresses were given."));
}

auto serialadapter_aux_spi::serialize(const std::vector<uint8_t> &data) -> std::string {
    std::string str;
    for (auto ch : data){
        str += static_cast<char>(ch);
    }
    return str;
}

void serialadapter_aux_spi::print_hex(const std::string& data, const std::string &prefix, const bool output)
{
    print_hex(std::vector<std::string>{{data}}, prefix, output);
}

auto serialadapter_aux_spi::deserialize(const std::string& data, bool skip_even) -> std::vector<uint8_t>{
    std::vector<uint8_t> ret_data;
    for (std::size_t i = 0; i < data.size(); i++){
        if (skip_even && (i%2u==1u)){
            continue;
        }
        ret_data.push_back(static_cast<uint8_t>(data[i]));
    }
    return ret_data;
}

void serialadapter_aux_spi::print_hex(const std::vector<std::string>& data, const std::string &prefix, const bool output)
{
    if(output){std::cout << prefix;}
    if (data.empty()){
        if(output){std::cout << std::endl;}
        return;
    }
    if (data.size()>1){
        if(output){std::cout << std::endl;}
    }
    for (auto line : data){
        for (std::size_t i = 1; i < line.size(); i=i+2){
            uint32_t val = 0;
            val |= static_cast<uint32_t>(line[i]);
            val = val << 8;
            val |= static_cast<uint32_t>(line[i-1]);
            if(output){std::cout << " " << std::setfill('0') << std::setw(4) << std::hex << static_cast<int>(val);}
            else{std::cout << std::hex;}
        }
        if(output){std::cout << std::endl;}
    }
}

auto serialadapter_aux_spi::write8bit(uint8_t address, uint8_t data, std::size_t i_saa, const bool output) -> bool
{
    if (i_saa >= m_n_asics){
        saa_index_error(i_saa);
    }
    std::vector<uint8_t> address_arr{};
    std::vector<uint8_t> data_arr{};
    std::vector<bool> mask{};
    for (std::size_t i = 0; i < m_n_asics; i++){
        if (i == i_saa){
            address_arr.push_back(address);
            data_arr.push_back(data);
            mask.push_back(true);
        }
        else{
            address_arr.push_back(0x00u);
            data_arr.push_back(0x00u);
            mask.push_back(false);
        }
    }
    return write8bit(address_arr, data_arr, mask, output);
}

auto serialadapter_aux_spi::write8bit(const std::vector<uint8_t> &address, const std::vector<uint8_t> &data, const bool output) -> bool
{
    auto mask = std::vector<bool>(address.size(), true);
    return write8bit(address,data,mask,output);
}

auto serialadapter_aux_spi::write8bit(const std::vector<uint8_t> &address, const std::vector<uint8_t> &data, const std::vector<bool> &mask, const bool output) -> bool
{
    if (address.size() != data.size()){
        address_data_mismatch_error(address.size(), data.size());
    }
    if (address.size() != mask.size()){
        address_mask_mismatch_error(address.size(), mask.size());
    }
    // reverse / invert all vectors because first written values are for last SAA
    auto rev_data = data;
    std::reverse(rev_data.begin(), rev_data.end());
    auto rev_address = address;
    std::reverse(rev_address.begin(), rev_address.end());
    auto rev_mask = mask;
    std::reverse(rev_mask.begin(), rev_mask.end());

    std::string data_out{};
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({0x0bu, 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({rev_address[i], 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_data.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({rev_data[i], 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }
    return true;
}

auto serialadapter_aux_spi::write16bit(uint8_t address, uint16_t data, std::size_t i_saa, const bool output) -> bool
{
    if (i_saa >= m_n_asics){
        saa_index_error(i_saa);
    }
    std::vector<uint8_t> address_arr{};
    std::vector<uint16_t> data_arr{};
    std::vector<bool> mask{};
    for (std::size_t i = 0; i < m_n_asics; i++){
        if (i == i_saa){
            address_arr.push_back(address);
            data_arr.push_back(data);
            mask.push_back(true);
        }
        else{
            address_arr.push_back(0x00u);
            data_arr.push_back(0x00u);
            mask.push_back(false);
        }
    }
    return write16bit(address_arr, data_arr, mask, output);
}

auto serialadapter_aux_spi::write16bit(const std::vector<uint8_t> &address, const std::vector<uint16_t> &data, const bool output) -> bool
{
    auto mask = std::vector<bool>(address.size(), true);
    return write16bit(address,data,mask, output);
}

auto serialadapter_aux_spi::write16bit(const std::vector<uint8_t> &address, const std::vector<uint16_t> &data, const std::vector<bool> &mask, const bool output) -> bool
{
    if (address.size() != data.size()){
        address_data_mismatch_error(address.size(), data.size());
    }
    if (address.size() != mask.size()){
        address_mask_mismatch_error(address.size(), mask.size());
    }
    // reverse / invert all vectors because first written values are for last SAA
    auto rev_data = data;
    std::reverse(rev_data.begin(), rev_data.end());
    auto rev_address = address;
    std::reverse(rev_address.begin(), rev_address.end());
    auto rev_mask = mask;
    std::reverse(rev_mask.begin(), rev_mask.end());

    std::string data_out{};
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({0x0du, 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({rev_address[i], 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_data.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({static_cast<uint8_t>(rev_data[i]>>8), 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_data.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({static_cast<uint8_t>(rev_data[i] & 0xff), 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }
    return true;
}

auto serialadapter_aux_spi::write32bit(uint8_t address, uint32_t data, std::size_t i_saa, const bool output) -> bool
{
    if (i_saa >= m_n_asics){
        saa_index_error(i_saa);
    }
    std::vector<uint8_t> address_arr{};
    std::vector<uint32_t> data_arr{};
    std::vector<bool> mask{};
    for (std::size_t i = 0; i < m_n_asics; i++){
        if (i == i_saa){
            address_arr.push_back(address);
            data_arr.push_back(data);
            mask.push_back(true);
        }
        else{
            address_arr.push_back(0x00u);
            data_arr.push_back(0x00u);
            mask.push_back(false);
        }
    }
    return write32bit(address_arr, data_arr, mask, output);
}

auto serialadapter_aux_spi::write32bit(const std::vector<uint8_t> &address, const std::vector<uint32_t> &data, const bool output) -> bool
{
    auto mask = std::vector<bool>(address.size(), true);
    return write32bit(address, data, mask, output);
}

auto serialadapter_aux_spi::write32bit(const std::vector<uint8_t> &address, const std::vector<uint32_t> &data, const std::vector<bool> &mask, const bool output) -> bool
{
    if (address.size() != data.size()){
        address_data_mismatch_error(address.size(), data.size());
    }
    if (address.size() != mask.size()){
        address_mask_mismatch_error(address.size(), mask.size());
    }
    // reverse / invert all vectors because first written values are for last SAA
    auto rev_data = data;
    std::reverse(rev_data.begin(), rev_data.end());
    auto rev_address = address;
    std::reverse(rev_address.begin(), rev_address.end());
    auto rev_mask = mask;
    std::reverse(rev_mask.begin(), rev_mask.end());

    std::string data_out{};
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({0x0fu, 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({rev_address[i], 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_data.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({static_cast<uint8_t>((rev_data[i]>>24) & 0xff), 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_data.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({static_cast<uint8_t>((rev_data[i] >> 16) & 0xff), 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_data.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({static_cast<uint8_t>((rev_data[i] >> 8) & 0xff), 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < rev_data.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({static_cast<uint8_t>(rev_data[i] & 0xff), 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }
    return true;
}

auto serialadapter_aux_spi::read8bit(uint8_t address, std::size_t i_saa, const bool output) -> uint8_t
{
    if (i_saa >= m_n_asics){
        saa_index_error(i_saa);
    }
    std::vector<uint8_t> address_arr{};
    std::vector<bool> mask{};
    for (std::size_t i = 0; i < m_n_asics; i++){
        if (i == i_saa){
            address_arr.push_back(address);
            mask.push_back(true);
        }
        else{
            address_arr.push_back(0x00u);
            mask.push_back(false);
        }
    }
    auto values = read8bit(address_arr, mask, output);
    return values[i_saa];
}

auto serialadapter_aux_spi::read8bit(const std::vector<uint8_t> &address, const bool output) -> std::vector<uint8_t>
{
    auto mask = std::vector<bool>(address.size(), true);
    return read8bit(address, mask, output);
}

auto serialadapter_aux_spi::read8bit(const std::vector<uint8_t> &address, const std::vector<bool> &mask, const bool output) -> std::vector<uint8_t>
{
    if (address.size() != mask.size()){
        address_mask_mismatch_error(address.size(), mask.size());
    }
    auto rev_address = address;
    std::reverse(rev_address.begin(), rev_address.end());
    auto rev_mask = mask;
    std::reverse(rev_mask.begin(), rev_mask.end());

    std::string data_out{};
    std::string data_in{};
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({0x0au, 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    SPI::writeSpi(data_out);

    data_out = "";
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({rev_address[i], 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    SPI::writeSpi(data_out);

    data_out = "";
    for (std::size_t i = 0; i < rev_address.size(); i++){
        data_out += serialize({0x00u, 0x00u});
    }
    for (std::size_t i = 0; i < 2; i++){
        print_hex(data_out, "data_out:", output);
        data_in += SPI::readSpi(data_out);
    }

    print_hex(data_in, "data_in:", output);
    auto data = deserialize(data_in);
    //print_hex(serialize(data), "data deserialized:");
    if (data.size() < m_n_asics){
        return std::vector<uint8_t>{};
    }
    std::vector<uint8_t> ret_data(m_n_asics,0x0u);
    for (std::size_t i = m_n_asics; i < data.size(); i = i + m_n_asics){
        for (std::size_t j = 0; j < m_n_asics; j++){
            ret_data[j] = data[i+j];
        }
    }
    std::reverse(ret_data.begin(), ret_data.end());
    return ret_data;
}

auto serialadapter_aux_spi::read16bit(uint8_t address, std::size_t i_saa, const bool output) -> uint16_t
{
    if (i_saa >= m_n_asics){
        saa_index_error(i_saa);
    }
    std::vector<uint8_t> address_arr{};
    std::vector<bool> mask{};
    for (std::size_t i = 0; i < m_n_asics; i++){
        if (i == i_saa){
            address_arr.push_back(address);
            mask.push_back(true);
        }
        else{
            address_arr.push_back(0x00u);
            mask.push_back(false);
        }
    }
    auto values = read16bit(address_arr, mask,output);
    return values[i_saa];
}

auto serialadapter_aux_spi::read16bit(const std::vector<uint8_t> &address, const bool output) -> std::vector<uint16_t> {
    auto mask = std::vector<bool>(address.size(),true);
    return read16bit(address, mask, output);
}

auto serialadapter_aux_spi::read16bit(const std::vector<uint8_t> &address, const std::vector<bool> &mask, const bool output) -> std::vector<uint16_t>
{
    if (address.size() != mask.size()){
        address_mask_mismatch_error(address.size(), mask.size());
    }
    auto rev_address = address;
    std::reverse(rev_address.begin(), rev_address.end());
    auto rev_mask = mask;
    std::reverse(rev_mask.begin(), rev_mask.end());

    std::string data_out{};
    std::string data_in{};
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({0x0cu, 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    SPI::writeSpi(data_out);

    data_out = "";
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i])
        {
            data_out += serialize({rev_address[i], 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    SPI::writeSpi(data_out);

    data_out = "";
    for (std::size_t i = 0; i < rev_address.size(); i++){
        data_out += serialize({0x00u, 0x00u});
    }
    for (std::size_t i = 0; i < 3; i++){
        print_hex(data_out, "data_out:", output);
        data_in += SPI::readSpi(data_out);
    }

    print_hex(data_in, "data_in:", output);
    auto data = deserialize(data_in);
    //print_hex(serialize(data), "data deserialized:");
    if (data.size() < 3*m_n_asics){
        return std::vector<uint16_t>{};
    }
    std::vector<uint16_t> ret_data(m_n_asics,0x0u);
    for (std::size_t i = m_n_asics; i < data.size(); i = i + m_n_asics){
        for (std::size_t j = 0; j < m_n_asics; j++){
            ret_data[j] <<= 8;
            ret_data[j] += data[i+j];
        }
    }
    std::reverse(ret_data.begin(), ret_data.end());
    return ret_data;
}

auto serialadapter_aux_spi::read32bit(uint8_t address, std::size_t i_saa, const bool output) -> uint32_t
{
    if (i_saa >= m_n_asics){
        saa_index_error(i_saa);
    }
    std::vector<uint8_t> address_arr{};
    std::vector<bool> mask{};
    for (std::size_t i = 0; i < m_n_asics; i++){
        if (i == i_saa){
            address_arr.push_back(address);
            mask.push_back(true);
        }
        else{
            address_arr.push_back(0x00u);
            mask.push_back(false);
        }
    }
    auto values = read32bit(address_arr, mask, output);
    return values[i_saa];
}

auto serialadapter_aux_spi::read32bit(const std::vector<uint8_t> &address, const bool output) -> std::vector<uint32_t>
{
    auto mask = std::vector<bool>(address.size(), true);
    return read32bit(address, mask, output);
}


auto serialadapter_aux_spi::read32bit(const std::vector<uint8_t> &address, const std::vector<bool> &mask, const bool output) -> std::vector<uint32_t>
{
    if (address.size() != mask.size()){
        address_mask_mismatch_error(address.size(), mask.size());
    }
    auto rev_address = address;
    std::reverse(rev_address.begin(), rev_address.end());
    auto rev_mask = mask;
    std::reverse(rev_mask.begin(), rev_mask.end());

    std::string data_out{};
    std::string data_in{};
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({0x0eu, 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    SPI::writeSpi(data_out);

    data_out = "";
    for (std::size_t i = 0; i < rev_address.size(); i++){
        if (rev_mask[i]){
            data_out += serialize({rev_address[i], 0x01u});
        }
        else{
            data_out += serialize({0x00u, 0x00u});
        }
    }
    print_hex(data_out, "data_out:", output);
    SPI::writeSpi(data_out);

    data_out = "";
    for (std::size_t i = 0; i < rev_address.size(); i++){
        data_out += serialize({0x00u, 0x00u});
    }
    for (std::size_t i = 0; i < 5; i++){
        print_hex(data_out, "data_out:", output);
        data_in += SPI::readSpi(data_out);
    }

    print_hex(data_in, "data_in:", output);
    auto data = deserialize(data_in);
    //print_hex(serialize(data), "data deserialized:");
    if (data.size() < 5*m_n_asics){
        return std::vector<uint32_t>{};
    }
    std::vector<uint32_t> ret_data(m_n_asics,0x0u);
    for (std::size_t i = m_n_asics; i < data.size(); i = i + m_n_asics){
        for (std::size_t j = 0; j < m_n_asics; j++){
            ret_data[j] <<= 8;
            ret_data[j] += data[i+j];
        }
    }
    std::reverse(ret_data.begin(), ret_data.end());
    return ret_data;
}

auto serialadapter_aux_spi::init_Asic(const bool output) -> bool{
    // initialise asic, this means send 6 10bit words -> 12 8bit chars lsbyte first
    std::string data_out = "";
    std::string data_in = "";
    
    for (std::size_t i = 0; i < m_n_asics; i++){
        data_out += serialize({0x10u, 0x01u});
    }
    
    print_hex(data_out, "init_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < m_n_asics; i++){
        data_out += serialize({0x00u,0x01u});
    }
    
    print_hex(data_out, "init_out:", output);
    if (!SPI::writeSpi(data_out)){
        return false;
    }

    data_out = "";
    for (std::size_t i = 0; i < m_n_asics; i++){
        data_out += serialize({0x00u,0x00u});
    }
    for (std::size_t i = 0; i < 3; i++){
        print_hex(data_out, "init_out:", output);
        data_in += SPI::readSpi(data_out);
    }

    print_hex(data_in, "init_in:", output);
    std::vector<uint8_t> response_codes{};
    if (data_in.size()<2*m_n_asics){
        return false;
    }
    for (std::size_t i = 1; i < 2*m_n_asics; i = i+2){
        response_codes.push_back(static_cast<uint8_t>(data_in[i-1]));
        response_codes.push_back(static_cast<uint8_t>(data_in[i]));
    }
    print_hex(serialize(response_codes), "response_codes:", output);
    return true;
}
