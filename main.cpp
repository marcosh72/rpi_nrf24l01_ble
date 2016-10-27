#include <iostream>
#include "spi.h"

using namespace std;

int main(void){
  uint8_t tx[50] = "dsajdlkajdalskdjalsdjasd";
  uint8_t rx[50];

  SPI spi(4000000, SPI_MODE_0);
  spi.WriteRead(tx, rx, 10);
}
