#ifndef BBBLE_H_
#define BBBLE_H_

class Radio;

class BBBLE{
private:
  uint8_t packet[32];
  uint_fast8_t packet_len;

  uint_fast8_t PopulateHeader(void);
  uint_fast8_t PopulatePayload(uint8_t *payload);
  uint_fast8_t ReverseByteAndBit(void);
  uint_fast8_t CRC24(void);
  uint_fast8_t Whiten(void);
public:
  BBBLE(const char *message);
  uint_fast8_t Transmit(void);
  uint_fast8_t ChangeMessage(const char *message);
};

#endif
