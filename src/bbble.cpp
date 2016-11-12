#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/time.h>
#include "bbble.h"

using namespace std;

BBBLE::BBBLE(Radio *radiodev){
  radio = radiodev;
}

uint_fast8_t BBBLE::ChangeMessage(const char *message){
  PopulatePayload((uint8_t *)&message[0]);
  PopulateHeader();
  //ReverseByteAndBit((uint8_t *)packet.pkt_pdu, total_len - 4 - 3); // only data and 
  CRC24();
  //Whiten();
  //ReverseByteAndBit(WITH_CRC);

  return 0;
}


uint_fast8_t BBBLE::PopulateHeader(void){
  packet.pkt_header.access_address[0] = 0xd6;
  packet.pkt_header.access_address[1] = 0xbe;
  packet.pkt_header.access_address[2] = 0x89;
  packet.pkt_header.access_address[3] = 0x8e;

  packet.pkt_pdu.pdu_header.pdu_type = PDU_TYPE_ADV_NONCONN_IND;
  packet.pkt_pdu.pdu_header.length = total_len - 6; // no access_address and no header

  packet.pkt_pdu.adv_addr[0] = 0x01;
  packet.pkt_pdu.adv_addr[1] = 0x23;
  packet.pkt_pdu.adv_addr[2] = 0x45;
  packet.pkt_pdu.adv_addr[3] = 0x67;
  packet.pkt_pdu.adv_addr[4] = 0x89;
  packet.pkt_pdu.adv_addr[5] = 0xab;

  packet.pkt_pdu.adv_data[0] = 0x02;
  packet.pkt_pdu.adv_data[1] = 0x01;
  packet.pkt_pdu.adv_data[2] = 0x01;
  packet.pkt_pdu.adv_data[3] = packet_len + 1;
  packet.pkt_pdu.adv_data[4] = 0x09;

  return 0;
}

uint_fast8_t BBBLE::PopulatePayload(uint8_t *payload){
  uint8_t *pkt_ptr = (uint8_t *)&packet.pkt_pdu.adv_message[0];
  uint8_t *msg_ptr = (uint8_t *)&payload[0];
  uint_fast8_t pkt_len = 0, i;

  while(*msg_ptr != '\0'){
    pkt_len++;
    *pkt_ptr++ = *msg_ptr++;
  }
  packet_len = pkt_len;
  total_len = sizeof(packet) - (sizeof(packet.pkt_pdu.adv_message) - pkt_len) + 3; // CRC
  pkt_ptr = (uint8_t *)&packet.pkt_pdu.adv_message[0];

  cout << "Message:\t\"";
  for(i = 0; i < packet_len; i++){
    cout << *pkt_ptr++;
  }
  cout << "\"" << endl;
  cout << "Packet len:\t" << (int)packet_len << endl << endl;
  pkt_ptr = (uint8_t *)&packet.pkt_pdu.adv_message[0];

  cout << "Message TX:\t";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < (packet_len + 6); i++){
    cout << hex << setw(4) << (int)*pkt_ptr++ << " ";
  }
  cout << endl;

  return 0;
}

uint_fast8_t BBBLE::ReverseByteAndBit(uint8_t *addr, uint_fast8_t len){
  uint8_t *pkt_ptr = (uint8_t *)addr;
  uint8_t *pkt_ptr_end = pkt_ptr + len;
  uint_fast8_t i;
  uint8_t aux = 0, rev = 0;

  for(i = 0; i < len; i++){
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

  pkt_ptr = (uint8_t *)addr;
  for(i = 0; i < (len >> 1); i++){
    aux = *pkt_ptr;
    *pkt_ptr = *pkt_ptr_end;
    *pkt_ptr_end = aux;

    pkt_ptr++;
    pkt_ptr_end--;
  }

  return 0;
}

uint_fast8_t BBBLE::CRC24(void){
  uint8_t v, t, d;
  uint8_t *pkt_ptr = (uint8_t *)&packet.pkt_pdu.adv_message[0];
  uint8_t len = packet_len;
  uint8_t *dst = &packet.pkt_pdu.adv_message[packet_len];
  uint_fast8_t i;

  for(i = 0; i < 3; i++){
    dst[i] = 0x55;
  }

  while(len--){

    d = *pkt_ptr++;

    for(v = 0; v < 8; v++, d >>= 1){
      t = dst[0] >> 7;

      dst[0] <<= 1;
      if(dst[1] & 0x80) dst[0] |= 1;
      dst[1] <<= 1;
      if(dst[2] & 0x80) dst[1] |= 1;
      dst[2] <<= 1;

      if(t != (d & 1)){
        dst[2] ^= 0x5B;
        dst[1] ^= 0x06;
      }
    }
  }

  cout << "CRC:\t\t";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < 3; i++){
    cout << hex << setw(4) << (int)*dst++ << " ";
  }
  cout << endl;

  return 0;
}

uint_fast8_t BBBLE::Whiten(void){
  uint8_t  m;
  uint8_t *pkt_ptr = (uint8_t *)&packet.pkt_pdu;
  uint8_t *data = pkt_ptr;
  uint8_t len = packet.pkt_pdu.pdu_header.length;
  uint8_t channel = radio->GetRFChannel();
  uint8_t whiten_coeff = 0;

  whiten_coeff |= (channel >> 7) & 0x01;
  whiten_coeff |= (channel >> 5) & 0x02;
  whiten_coeff |= (channel >> 3) & 0x04;
  whiten_coeff |= (channel >> 1) & 0x08;
  whiten_coeff |= (channel << 1) & 0x10;
  whiten_coeff |= (channel << 3) & 0x20;
  whiten_coeff |= (channel << 5) & 0x40;
  whiten_coeff |= (channel << 7) & 0x80;

  whiten_coeff |= 2;

  while(len--){

    for(m = 1; m; m <<= 1){

      if(whiten_coeff & 0x80){

        whiten_coeff ^= 0x11;
        (*data) ^= m;
      }
      whiten_coeff <<= 1;
    }
    data++;
  }

  return 0;
}

uint_fast8_t BBBLE::Transmit(void){
  struct pcap_hdr_s pcap_hdr;
  struct pcaprec_hdr_s pcaprec_hdr;

  pcap_hdr.magic_number = 0xa1b2c3d4;
  pcap_hdr.version_major = 2;
  pcap_hdr.version_minor = 4;
  pcap_hdr.thiszone = 0;
  pcap_hdr.sigfigs = 0;
  pcap_hdr.snaplen = 65535;
  pcap_hdr.network = 251; // LINKTYPE_BLUETOOTH_LE_LL

  struct timeval tv;
  gettimeofday(&tv, nullptr);
  pcaprec_hdr.ts_sec = tv.tv_sec;
  pcaprec_hdr.ts_usec = tv.tv_usec;
  pcaprec_hdr.incl_len = total_len;
  pcaprec_hdr.orig_len = total_len;

  uint8_t *ptr;
  ofstream fpcap;
  fpcap.open("out.pcap", ios::binary);
  ptr = (uint8_t *)&pcap_hdr;
  for(unsigned int i = 0; i < sizeof(pcap_hdr_s); i++){
    fpcap << *ptr++;
  }
  ptr = (uint8_t *)&pcaprec_hdr;
  for(unsigned int i = 0; i < sizeof(pcaprec_hdr_s); i++){
    fpcap << *ptr++;
  }
  ptr = (uint8_t *)&packet;
  for(int i = 0; i < total_len; i++){
    fpcap << *ptr++;
  }
  fpcap.close();

  radio->Transmit((uint8_t *)&packet.pkt_pdu, total_len - 4); // no access_address
  return 0;
}
