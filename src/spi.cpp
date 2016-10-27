#include <iostream>
#include <iomanip>
#include "spi.h"

using namespace std;

SPI::SPI(uint32_t clk_frequency, uint_fast8_t mode){
  clock_frequency = clk_frequency;
  switch(mode){
    case SPI_MODE_0:
      clock_polarity = 0;
      clock_phase = 0;
      break;
    case SPI_MODE_1:
      clock_polarity = 0;
      clock_phase = 1;
      break;
    case SPI_MODE_2:
      clock_polarity = 1;
      clock_phase = 0;
      break;
    case SPI_MODE_3:
      clock_polarity = 1;
      clock_phase = 1;
      break;
    default:
      clock_polarity = 0;
      clock_phase = 0;
      break;
  }

  cout << "SPI configured as master:" << endl;
  cout << "\tCPHA = " << clock_phase << endl;
  cout << "\tCPOL = " << clock_polarity << endl;
  cout << "\tSpeed = " << clock_frequency << "Hz" << endl << endl;
}

uint_fast8_t SPI::WriteRead(uint8_t *tx_data, uint8_t *rx_data, uint_fast8_t len){
  uint_fast8_t i;

  cout << "SPI TX: ";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < len; i++){
    cout << hex << setw(4) << (int)*tx_data++ << " ";
    *rx_data++ = i;
  }
  cout << endl;

  return 0;
}
