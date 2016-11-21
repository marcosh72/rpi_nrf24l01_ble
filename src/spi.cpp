#include <iostream>
#include <iomanip>
#include "spi.h"

using namespace std;

SPI::SPI(uint32_t clk_frequency, uint_fast8_t mode){
  uint_fast16_t clock_divider;
  clock_divider = (uint_fast16_t)((250000000 / clk_frequency) & ~0x01) + 2;
  clock_frequency = (uint32_t)(250000000 / clock_divider);

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
  cout << "\tClk Div = " << clock_divider << endl;
  cout << "\tSpeed = " << clock_frequency << "Hz" << endl << endl;

  if(!bcm2835_init()){
    cout << "BCM2835 init failed. Are you running as root?" << endl;
  } else{
    if(!bcm2835_spi_begin()){
      cout << "BCM2835 spi init failed. Are you running as root?" << endl;
    } else{
      bcm2835_gpio_fsel(RPI_GPIO_P1_15, BCM2835_GPIO_FSEL_OUTP);
      bcm2835_gpio_write(RPI_GPIO_P1_15, LOW);
      bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
      bcm2835_spi_setDataMode(mode);
      bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_2048); // nearest even number
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
    cout << hex << setw(4) << (int)tx_data[i] << " ";
  }
  cout << endl;

  bcm2835_spi_writenb((char *)&tx_data[0], len);

  return 0;
}
