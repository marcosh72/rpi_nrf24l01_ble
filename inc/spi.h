#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

#define SPI_MODE_0              0
#define SPI_MODE_1              1
#define SPI_MODE_2              2
#define SPI_MODE_3              3

class SPI{
private:
  bool clock_phase;
  bool clock_polarity;
  uint32_t clock_frequency;
public:
  SPI(uint32_t clk_frequency, uint_fast8_t mode);
  static uint_fast8_t WriteRead(uint8_t *tx_data, uint8_t *rx_data, uint_fast8_t len);
};

#endif // SPI_H_
