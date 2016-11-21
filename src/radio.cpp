#include <iostream>
#include <iomanip>
#include "radio.h"
#include "spi.h"

using namespace std;

Radio::Radio(SPI *spidev){
  uint_fast8_t i;

  channel = 0; // can be 0, 1 or 2
  spi = spidev;

  const uint8_t configs[12][2] = {
    {0x20, 0x12},   // on, no crc, int on RX/TX done
    {0x21, 0x00},   // no autoACK
    {0x22, 0x00},   // no rx
    {0x23, 0x02},   // 5 bytes adderss
    {0x24, 0x00},   // no auto retransmit
    {0x25, chRf[channel]}, // channel
    {0x26, 0x06},   // 1mbps at 0dbm
    {0x27, 0x3e},   // clear various flags
    {0x3c, 0x00},   // no dym payloads
    {0x3d, 0x00},   // no features
    {0x31, 32},     // always rx 32 bytes
    {0x22, 0x01}   // rx on pipe 0
  };

  const uint8_t addr_conf[] = {
    0x30,   // set TX_ADDR
    SwapBits(0x8e),
    SwapBits(0x89),
    SwapBits(0xbe),
    SwapBits(0xd6),
    0x2a,   // set RX_ADDR_P0
    SwapBits(0x8e),
    SwapBits(0x89),
    SwapBits(0xbe),
    SwapBits(0xd6)
  };

  for(i = 0; i < 12; i++){
    spi->WriteRead((uint8_t *)&configs[i][0], nullptr, 2);
  }
  spi->WriteRead((uint8_t *)&addr_conf[0], nullptr, 5);
  spi->WriteRead((uint8_t *)&addr_conf[5], nullptr, 5);

  cout << "Radio initialized on channel " << (int)chLe[channel] << "." << endl << endl;
}

uint8_t Radio::SwapBits(uint8_t b){
  uint8_t rev = 0;

  rev |= (b >> 7) & 0x01;
  rev |= (b >> 5) & 0x02;
  rev |= (b >> 3) & 0x04;
  rev |= (b >> 1) & 0x08;
  rev |= (b << 1) & 0x10;
  rev |= (b << 3) & 0x20;
  rev |= (b << 5) & 0x40;
  rev |= (b << 7) & 0x80;

  return rev;
}


uint_fast8_t Radio::Transmit(uint8_t *payload, uint_fast8_t len){
  uint_fast8_t i;
  uint8_t buf[len + 1];

  cout << "Radio TX:\t";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < len; i++){
    cout << hex << setw(4) << (int)payload[i] << " ";
  }
  cout << endl;

  buf[0] = 0xa0; // W_TX_PAYLOAD
  for(i = 0; i < len; i++){
    buf[i + 1] = payload[i];
  }
  spi->WriteRead((uint8_t *)&buf[0], nullptr, len + 1);

  buf[0] = 0x20;
  buf[1] = 0x12;
  spi->WriteRead((uint8_t *)&buf[0], nullptr, 2); // tx on
  bcm2835_gpio_set(RPI_GPIO_P1_15);
  bcm2835_delay(2);
  bcm2835_gpio_clr(RPI_GPIO_P1_15);

  return 0;
}

uint_fast8_t Radio::SetRFChannel(uint8_t channel_set) {
  uint8_t buf[2];
  channel = channel_set;

  buf[0] = 0x25;
  buf[1] = chRf[channel];
  spi->WriteRead((uint8_t *)&buf[0], nullptr, 2); // set channel

  buf[0] = 0x27;
  buf[1] = 0x6E;
  spi->WriteRead((uint8_t *)&buf[0], nullptr, 2); // clear flags

  buf[0] = 0xE2;
  spi->WriteRead((uint8_t *)&buf[0], nullptr, 1); // clear RX FIFO

  buf[0] = 0xE1;
  spi->WriteRead((uint8_t *)&buf[0], nullptr, 1); // clear TX FIFO

  return 0;

}

uint_fast8_t Radio::GetRFChannel(void){
  return chRf[channel];
}

uint_fast8_t Radio::GetLEChannel(void){
  return chLe[channel];
}
