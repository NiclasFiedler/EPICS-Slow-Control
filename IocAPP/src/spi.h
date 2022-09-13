#ifndef SPI_H
#define SPI_H

#include <string>

class SPI
{
public:
	enum class MODE {
		MODE_0,
		MODE_1,
		MODE_2,
		MODE_3
	};
    static bool initialise(
      unsigned f_baud_rate = static_cast<unsigned>(2e5)
    , unsigned f_word_size = 10
    , SPI::MODE f_mode = SPI::MODE::MODE_1);
    static bool writeSpi(std::string data);
    static std::string readSpi(std::string data_in);
private:
    static unsigned m_word_size;
    static SPI::MODE m_mode;
    static int spi_handle;
    static bool spi_initialised;
    static unsigned m_baud_rate;
    static unsigned m_spi_flags;
    SPI() {}
};

#endif // SPI_H
