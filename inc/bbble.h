#ifndef BBBLE_H_
#define BBBLE_H_

#include "radio.h"

#define WITH_CRC 1
#define WITHOUT_CRC 0

#define PDU_TYPE_ADV_NONCONN_IND      0x02

struct __attribute__ ((__packed__)) bbble_packet_header_s {
  uint8_t access_address[4];
};

struct __attribute__ ((__packed__)) bbble_advertising_pdu_header_s {
  uint8_t pdu_type : 4;
  uint8_t RFU0 : 2;
  uint8_t tx_addr : 1;
  uint8_t rx_addr : 1;
  uint8_t length : 6;
  uint8_t RFU1 : 2;
};

struct __attribute__ ((__packed__)) bbble_advertising_pdu_payload_s {
  struct bbble_advertising_pdu_header_s pdu_header;
  uint8_t adv_addr[6];
  uint8_t adv_data[5];
  uint8_t adv_message[RADIO_MAX_PAYLOAD - 2 - 6 - 5 - 3]; // 2 for pdu_header, 6 for adv_addr, 5 for pre message and 3 for crc
};

struct __attribute__ ((__packed__)) bbble_advertising_packet_s {
  struct bbble_packet_header_s pkt_header;
  struct bbble_advertising_pdu_payload_s pkt_pdu;
};


struct __attribute__ ((__packed__)) pcap_hdr_s {
  uint32_t magic_number;   /* magic number */
  uint16_t version_major;  /* major version number */
  uint16_t version_minor;  /* minor version number */
  int32_t  thiszone;       /* GMT to local correction */
  uint32_t sigfigs;        /* accuracy of timestamps */
  uint32_t snaplen;        /* max length of captured packets, in octets */
  uint32_t network;        /* data link type */
};

struct __attribute__ ((__packed__)) pcaprec_hdr_s {
  uint32_t ts_sec;         /* timestamp seconds */
  uint32_t ts_usec;        /* timestamp microseconds */
  uint32_t incl_len;       /* number of octets of packet saved in file */
  uint32_t orig_len;       /* actual length of packet */
};

class BBBLE{
private:
  Radio *radio;

  struct bbble_advertising_packet_s packet;
  uint_fast8_t packet_len;
  uint_fast8_t total_len;

  uint_fast8_t PopulateHeader(void);
  uint_fast8_t PopulatePayload(uint8_t *payload);
  uint_fast8_t ReverseByteAndBit(uint8_t *addr, uint_fast8_t len);
  uint_fast8_t CRC24(void);
  uint_fast8_t Whiten(void);
public:
  BBBLE(Radio *radiodev);
  uint_fast8_t Transmit(void);
  uint_fast8_t ChangeMessage(const char *message);
};

#endif
