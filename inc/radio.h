#ifndef RADIO_H_
#define RADIO_H_

#include "spi.h"

class Radio{
private:
  SPI *spi;
  uint_fast8_t rf_channel;
public:
  Radio(SPI *spidev);
  uint_fast8_t Transmit(uint8_t *payload, uint_fast8_t len);
  uint_fast8_t GetRFChannel(void);
};

#endif
