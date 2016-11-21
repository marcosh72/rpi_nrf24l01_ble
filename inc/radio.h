#ifndef RADIO_H_
#define RADIO_H_

#include "spi.h"

#define RADIO_MAX_PAYLOAD           32

class Radio{
private:
  SPI *spi;
  uint_fast8_t channel = 0;
  uint8_t SwapBits(uint8_t b);
public:
  Radio(SPI *spidev);
  uint_fast8_t Transmit(uint8_t *payload, uint_fast8_t len);
  uint_fast8_t GetRFChannel(void);
  uint_fast8_t GetLEChannel(void);
  uint_fast8_t SetRFChannel(uint8_t channel_set);
  const uint8_t chRf[3] = {2, 26, 80};
  const uint8_t chLe[3] = {37, 38, 39};
};

#endif
