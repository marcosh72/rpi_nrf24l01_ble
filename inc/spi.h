#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>
#include <bcm2835.h>

#define SPI_MODE_0              BCM2835_SPI_MODE0
#define SPI_MODE_1              BCM2835_SPI_MODE1
#define SPI_MODE_2              BCM2835_SPI_MODE2
#define SPI_MODE_3              BCM2835_SPI_MODE3

class SPI{
private:
  bool clock_phase;
  bool clock_polarity;
  uint32_t clock_frequency;
public:
  SPI(uint32_t clk_frequency, uint_fast8_t mode);
  ~SPI(void);
  static uint_fast8_t WriteRead(uint8_t *tx_data, uint8_t *rx_data, uint_fast8_t len);
};

#endif // SPI_H_
