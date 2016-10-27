#include <iostream>
#include <iomanip>
#include "bbble.h"

using namespace std;

BBBLE::BBBLE(const char *message){
  PopulateHeader();
  PopulatePayload((uint8_t *)&message[0]);
  ReverseByteAndBit();
}

uint_fast8_t BBBLE::PopulateHeader(void){
  packet[0] = 0x40;

  return 0;
}

uint_fast8_t BBBLE::PopulatePayload(uint8_t *payload){
  uint8_t *pkt_ptr = (uint8_t *)&packet[2];
  uint8_t *msg_ptr = (uint8_t *)&payload[0];
  uint_fast8_t pkt_len = 0, i;

  while(*msg_ptr != '\0'){
    pkt_len++;
    *pkt_ptr++ = *msg_ptr++;
  }
  packet_len = pkt_len;
  packet[1] = packet_len;
  pkt_ptr = (uint8_t *)&packet[2];

  cout << "Message:\t\"";
  for(i = 0; i < packet_len; i++){
    cout << *pkt_ptr++;
  }
  cout << "\"" << endl;
  cout << "Packet len:\t" << (int)packet_len << endl << endl;
  pkt_ptr = (uint8_t *)&packet[0];

  cout << "Message TX:\t";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < (packet_len + 2); i++){
    cout << hex << setw(4) << (int)*pkt_ptr++ << " ";
  }
  cout << endl;

  return 0;
}

uint_fast8_t BBBLE::ReverseByteAndBit(void){
  uint8_t *pkt_ptr = (uint8_t *)&packet[0];
  uint8_t *pkt_ptr_end = (uint8_t *)&packet[packet_len + 1];
  uint_fast8_t i;
  uint8_t aux = 0, rev = 0;

  for(i = 0; i < (packet_len + 2); i++){ // header + length + packet
    aux = *pkt_ptr;
    rev |= (aux >> 7) & 0x01;
    rev |= (aux >> 5) & 0x02;
    rev |= (aux >> 3) & 0x04;
    rev |= (aux >> 1) & 0x08;
    rev |= (aux << 1) & 0x10;
    rev |= (aux << 3) & 0x20;
    rev |= (aux << 5) & 0x40;
    rev |= (aux << 7) & 0x80;
    *pkt_ptr = rev;

    pkt_ptr++;
    rev = 0;
    aux = 0;
  }

  pkt_ptr = (uint8_t *)&packet[2]; // payload beginning
  for(i = 0; i < (packet_len >> 1); i++){
    aux = *pkt_ptr;
    *pkt_ptr = *pkt_ptr_end;
    *pkt_ptr_end = aux;

    pkt_ptr++;
    pkt_ptr_end--;
  }

  pkt_ptr = (uint8_t *)&packet[0];
  cout << "Reversed TX:\t";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < (packet_len + 2); i++){
    cout << hex << setw(4) << (int)*pkt_ptr++ << " ";
  }
  cout << endl;

  return 0;
}

uint_fast8_t BBBLE::Transmit(void){
  return 0;
}
