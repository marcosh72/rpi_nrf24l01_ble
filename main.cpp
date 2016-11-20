#include <iostream>
#include <unistd.h>
#include "bbble.h"
#include "radio.h"
#include "spi.h"
#include <bcm2835.h>

using namespace std;

int main(void){
  SPI spi(4000000, SPI_MODE_0);

  uint8_t tx[2] = {0x00, 0xff};
  uint8_t rx[2];

  spi.WriteRead(tx, rx, 2);
  cout << (int)rx[0] << endl;
  cout << (int)rx[1] << endl;

  //bcm2835_spi_end();
  //bcm2835_close();

  //Radio radio(&spi);
  //BBBLE bbble(&radio);

  //bbble.ChangeMessage("hi there!");
  //bbble.Transmit();

  //while(1){
    //bbble.Transmit();
    //usleep(1000000);
  //}

  return 0;
}
