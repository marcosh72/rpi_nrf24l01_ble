#include <iostream>
#include <iomanip>
#include "radio.h"
#include "spi.h"

using namespace std;

Radio::Radio(SPI *spidev){
  rf_channel = 38; // can be 37, 38 or 39
  spi = spidev;
  cout << "Radio initialized on channel " << (int)rf_channel << "." << endl << endl;
}

uint_fast8_t Radio::Transmit(uint8_t *payload, uint_fast8_t len){
  uint_fast8_t i;

  cout << "Radio TX:\t";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < len; i++){
    cout << hex << setw(4) << (int)*payload++ << " ";
  }
  cout << endl;

  payload -= len;

  spi->WriteRead(payload, nullptr, len);

  return 0;
}

uint_fast8_t Radio::GetRFChannel(void){
  return rf_channel;
}
