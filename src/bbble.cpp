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
  uint8_t *pkt_ptr = (uint8_t *)&packet.pkt_pdu;
  uint_fast8_t i;
  PopulatePayload((uint8_t *)&message[0]);
  PopulateHeader();
  CRC24();
  Whiten();
  for(i = 4; i < total_len; i++){
    pkt_ptr[i] = SwapBits(pkt_ptr[i]);
  }

  return 0;
}


uint_fast8_t BBBLE::PopulateHeader(void){
  packet.pkt_header.access_address[0] = 0xd6;
  packet.pkt_header.access_address[1] = 0xbe;
  packet.pkt_header.access_address[2] = 0x89;
  packet.pkt_header.access_address[3] = 0x8e;

  packet.pkt_pdu.pdu_header.pdu_type = PDU_TYPE_ADV_NONCONN_IND;
  packet.pkt_pdu.pdu_header.tx_addr = 1;
  packet.pkt_pdu.pdu_header.length = total_len - 9; // no access_address, no header and no CRC

  packet.pkt_pdu.adv_addr[0] = 0x11;
  packet.pkt_pdu.adv_addr[1] = 0x22;
  packet.pkt_pdu.adv_addr[2] = 0x33;
  packet.pkt_pdu.adv_addr[3] = 0x44;
  packet.pkt_pdu.adv_addr[4] = 0x55;
  packet.pkt_pdu.adv_addr[5] = 0x66;

  packet.pkt_pdu.adv_data[0] = 0x02;
  packet.pkt_pdu.adv_data[1] = 0x01;
  packet.pkt_pdu.adv_data[2] = 0x06;
  packet.pkt_pdu.adv_data[3] = packet_len + 1 - 5;
  packet.pkt_pdu.adv_data[4] = 0x08;

  return 0;
}

uint_fast8_t BBBLE::PopulatePayload(uint8_t *payload){
  uint8_t *pkt_ptr = (uint8_t *)&packet.pkt_pdu.adv_message[0];
  uint8_t *msg_ptr = (uint8_t *)&payload[0];
  uint_fast8_t pkt_len = 0, i;
  uint8_t custom_data[5] = {4, 0xff, 0x01, 0x02, 0x03};
  uint8_t *cdata_ptr = (uint8_t *)&custom_data[0];

  while(*msg_ptr != '\0'){
    pkt_len++;
    *pkt_ptr++ = *msg_ptr++;
  }

  for(i = 0; i < 5; i++){
    pkt_len++;
    *pkt_ptr++ = *cdata_ptr++;
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
  for(i = 0; i < (packet_len); i++){
    cout << hex << setw(4) << (int)*pkt_ptr++ << " ";
  }
  cout << endl;

  return 0;
}

uint_fast8_t BBBLE::ReverseByteAndBit(uint8_t *addr, uint_fast8_t len){
  uint8_t *pkt_ptr = (uint8_t *)addr;
  uint8_t *pkt_ptr_end = pkt_ptr + len - 1;
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
  uint8_t *pkt_ptr = (uint8_t *)&packet.pkt_pdu;
  uint8_t len = packet_len + 5 + 6 + 2; // adv_addr + pre data + pdu_header
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

  // specs page 2216
  for(int i = 0; i < 3; i++){
    ReverseByteAndBit(&dst[i], 1);
  }

  cout << "CRC:\t\t";
  cout << showbase << internal << setfill('0');
  for(i = 0; i < 3; i++){
    cout << hex << setw(4) << (int)*dst++ << " ";
  }
  cout << endl;

  return 0;
}

uint8_t BBBLE::SwapBits(uint8_t b){
  uint8_t rev = 0;

  rev |= (b >> 7) & 0x01;
  rev |= (b >> 5) & 0x02;
  rev |= (b >> 3) & 0x04;
  rev |= (b >> 1) & 0x08;
  rev |= (b << 1) & 0x10;
  rev |= (b << 3) & 0x20;
  rev |= (b << 5) & 0x40;
  rev |= (b << 7) & 0x80;

  return rev;
}

uint_fast8_t BBBLE::Whiten(void){
  uint8_t m;
  uint8_t *pkt_ptr = (uint8_t *)&packet.pkt_pdu;
  uint8_t len = total_len - 4; // no access_address
  uint8_t channel = radio->GetLEChannel();
  uint8_t whiten_coeff;

  whiten_coeff = SwapBits(channel) | 2;

  // cout << "Channel:\t";
  // cout << showbase << internal << setfill('0');
  // cout << hex << setw(4) << (int)channel << endl;
  // cout << "Whiten Coeff:\t";
  // cout << showbase << internal << setfill('0');
  // cout << hex << setw(4) << (int)whiten_coeff << endl;
  // cout << "Length:\t";
  // cout << showbase << internal << setfill('0');
  // cout << hex << setw(4) << (int)len << endl;

  while(len--){

    for(m = 1; m; m <<= 1){

      if(whiten_coeff & 0x80){

        whiten_coeff ^= 0x11;
        (*pkt_ptr) ^= m;
      }
      whiten_coeff <<= 1;
    }
    pkt_ptr++;
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
