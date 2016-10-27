#include <iostream>
#include <iomanip>
#include "radio.h"
#include "spi.h"

using namespace std;

Radio::Radio(void){
  rf_channel = 38; // can be 37, 38 or 39
  cout << "Radio initialized on channel " << (int)rf_channel << "." << endl << endl;
}

uint_fast8_t Radio::Transmit(uint8_t *payload, uint_fast8_t len){
  uint_fast8_t i;
  uint8_t rx[len];

  SPI::WriteRead(payload, nullptr, len);

  cout << "Radio TX:\t";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < len; i++){
    cout << hex << setw(4) << (int)*payload++ << " ";
  }
  cout << endl;

  return 0;
}
