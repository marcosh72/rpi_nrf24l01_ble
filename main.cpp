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
  uint8_t inst_channel = 0;

  bbble.ChangeMessage("nRF24");
  bbble.Transmit();

  while(1){
    for(inst_channel = 0; inst_channel < 3; inst_channel++) {
      radio.SetRFChannel(inst_channel);
      bbble.ChangeMessage("nRF24");
      bbble.Transmit();
      bcm2835_delay(200);
    }
  }

  return 0;
}
