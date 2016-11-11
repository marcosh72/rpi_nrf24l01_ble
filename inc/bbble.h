#ifndef BBBLE_H_
#define BBBLE_H_

#include "radio.h"

#define WITH_CRC 1
#define WITHOUT_CRC 0

class BBBLE{
private:
  Radio *radio;

  uint8_t packet[32];
  uint_fast8_t packet_len;

  uint_fast8_t PopulateHeader(void);
  uint_fast8_t PopulatePayload(uint8_t *payload);
  uint_fast8_t ReverseByteAndBit(uint8_t CRC);
  uint_fast8_t CRC24(void);
  uint_fast8_t Whiten(void);
public:
  BBBLE(Radio *radiodev);
  uint_fast8_t Transmit(void);
  uint_fast8_t ChangeMessage(const char *message);
};

#endif
