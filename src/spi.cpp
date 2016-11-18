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

  if(!bcm2835_init()){
    cout << "BCM2835 init failed. Are you running as root?" << endl;
  } else{
    if(!bcm2835_spi_begin()){
      cout << "BCM2835 spi init failed. Are you running as root?" << endl;
    } else{
      bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
      bcm2835_spi_setDataMode(mode);
      bcm2835_spi_setClockDivider((int)(250000000 / clk_frequency) & ~0x01); // nearest even number
      bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
      bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
    }
  }
}

SPI::~SPI(void){
  bcm2835_spi_end();
  bcm2835_close();
}


uint_fast8_t SPI::WriteRead(uint8_t *tx_data, uint8_t *rx_data, uint_fast8_t len){
  uint_fast8_t i;

  cout << "SPI TX:\t\t";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < len; i++){
    cout << hex << setw(4) << (int)*tx_data++ << " ";
    //*rx_data++ = i;
  }
  cout << endl;

  tx_data -= len;
  bcm2835_spi_transfernb((char *)&tx_data, (char *)&rx_data, len);

  return 0;
}
