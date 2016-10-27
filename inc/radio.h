#ifndef RADIO_H_
#define RADIO_H_

class Radio{
private:
  uint_fast8_t rf_channel;
public:
  Radio();
  uint_fast8_t Transmit(uint8_t *payload, uint_fast8_t len);
};

#endif
