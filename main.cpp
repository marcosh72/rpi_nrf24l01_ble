#include <iostream>
#include <unistd.h>
#include "bbble.h"
#include "radio.h"
#include "spi.h"
#include <bcm2835.h>

using namespace std;

int main(void){
  SPI spi(4000000, SPI_MODE_0);
  Radio radio(&spi);
  BBBLE bbble(&radio);

  bbble.ChangeMessage("hi there!");
  bbble.Transmit();

  while(1){
    bbble.Transmit();
    usleep(1000000);
  }

  return 0;
}
