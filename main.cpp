#include <iostream>
#include <unistd.h>
#include "bbble.h"
#include "radio.h"
#include "spi.h"

using namespace std;

int main(void){
  SPI spi(4000000, SPI_MODE_0);
  Radio radio;
  BBBLE bbble("ola mundo!");

  while(1){
    bbble.Transmit();
    usleep(1000);
  }
}
