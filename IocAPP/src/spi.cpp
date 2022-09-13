#include "spi.h"
#include <iostream>
#include <unistd.h>
#include <memory>
#if (_NO_SPI != 1)
#include <pigpio.h>
#endif

unsigned SPI::m_baud_rate{};
int SPI::spi_handle{-1};
bool SPI::spi_initialised{false};
unsigned SPI::m_spi_flags{}; // = 0xa01e1; // 0b00 1010 0000 0001 1110 0001;
// Raspi: Core clock speed of 250MHz can be devided by any even number from 2 to 65536
// => 3.814kHz to 125MHz
/*
 * spi_flags consists of the least significant 22 bits.

        21 20 19 18 17 16 15 14 13 12 11 10 9  8  7  6  5  4  3  2  1  0
        b  b  b  b  b  b  R  T  n  n  n  n  W  A u2 u1 u0 p2 p1 p0  m  m
        0  0  1  0  1  0  0  0  0  0  0  0  0  1  1  1  1  0  0  0  0  0
        word size bits  msb msb only-3wire 3wire aux CEx?  activ-low? spi-mode
*/

// extern "C" {
// //#include <pigpiod_if2.h> // if you use pigpiod
// #include <pigpio.h>
// }

#if (_NO_SPI == 1)
auto gpioInitialise() -> int
{
    return 0;
}
auto spiOpen(int spibus, unsigned baudrate, unsigned flags) -> int
{
    return 0;
}
auto spiXfer(int spi_handle, char *txBuf, char *rxBuf, std::size_t size) -> std::size_t
{
    return size;
}
constexpr int PI_BAD_CHANNEL{-1};
constexpr int PI_BAD_SPI_SPEED{-2};
constexpr int PI_BAD_FLAGS{-3};
constexpr int PI_NO_AUX_SPI{-4};
constexpr int PI_SPI_OPEN_FAILED{-5};
#endif

auto SPI::initialise(unsigned f_baud_rate, unsigned f_word_size, SPI::MODE f_mode) -> bool
{
    if (spi_initialised)
    {
        return false;
    }
    m_spi_flags = 0b111100000u;
    m_spi_flags |= (f_word_size << 16);
    switch (f_mode)
    {
    case SPI::MODE::MODE_0:
        break;
    case SPI::MODE::MODE_1:
        m_spi_flags |= 0b01u;
        break;
    case SPI::MODE::MODE_2:
        m_spi_flags |= 0b10u;
        break;
    case SPI::MODE::MODE_3:
        m_spi_flags |= 0b11u;
        break;
    default:
        break;
    }
    if (gpioInitialise() < 0)
    {
        return false;
    }
    m_baud_rate = f_baud_rate;
    spi_handle = spiOpen(2, m_baud_rate, m_spi_flags);
    if (spi_handle < 0)
    {
        std::cout << "could not initialise spi bus" << std::endl;
        switch (spi_handle)
        {
        case PI_BAD_CHANNEL:
            std::cout << "DMA channel not 0-15" << std::endl;
            break;
        case PI_BAD_SPI_SPEED:
            std::cout << "bad SPI speed" << std::endl;
            break;
        case PI_BAD_FLAGS:
            std::cout << "bad spi open flags" << std::endl;
            break;
        case PI_NO_AUX_SPI:
            std::cout << "no auxiliary SPI on Pi A or B" << std::endl;
            break;
        case PI_SPI_OPEN_FAILED:
            std::cout << "can't open SPI device" << std::endl;
            break;
        default:
            break;
        }
        return false;
    }
    spi_initialised = true;
    return true;
}

auto SPI::writeSpi(std::string data) -> bool
{
    if (!spi_initialised)
    {
        if (!initialise())
        {
            return false;
        }
    }
    auto txBuf{std::make_unique<char[]>(data.size())};
    for (std::size_t i = 0; i < data.size(); i++)
    {
        txBuf[i] = data[i];
    }
    auto rxBuf{std::make_unique<char[]>(data.size())};
    if (spiXfer(spi_handle, txBuf.get(), rxBuf.get(), data.size()) != data.size())
    {
        std::cout << "wrong number of bytes transfered" << std::endl;
        return false;
    }
    return true;
}

auto SPI::readSpi(std::string data) -> std::string
{
    // sends data_in bytes (or datawords that contain more than 8 bit) and returns data
    // readOffset=0 means simultaneous read while writing. readOffset=data_in.size()-1 means
    // only read data returned after data_in was sent
    if (!spi_initialised)
    {
        if (!initialise())
        {
            return "";
        }
    }

    auto txBuf{std::make_unique<char[]>(data.size())};
    auto rxBuf{std::make_unique<char[]>(data.size())};
    for (std::size_t i = 0; i < data.size(); i++)
    {
        txBuf[i] = data[i];
        rxBuf[i] = 0x00u;
    }
    if (spiXfer(spi_handle, txBuf.get(), rxBuf.get(), data.size()) != data.size())
    {
        std::cout << "wrong number of bytes transfered" << std::endl;
        return "";
    }

    std::string data_out;
    for (std::size_t i = 0; i < data.size(); i++)
    {
        data_out += rxBuf[i];
    }
    return data_out;
}
