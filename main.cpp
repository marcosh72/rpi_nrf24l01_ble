#include <iostream>
#include "radio.h"

using namespace std;

int main(void){
  uint8_t tx[50] = "dsajdlkajdalskdjalsdjasd";

  Radio radio;
  radio.Transmit(tx, 10);
}
