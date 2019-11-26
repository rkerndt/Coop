
/*******************************************************************************
 * A simple yet eloquant sntp client to keep the Andruino real time clock reasonably
 * accurate.
 */

#ifndef SNTP_CLIENT_H
#define SNTP_CLIENT_H

#include <Arduino.h>
#include <WiFi101.h>                                          
#include <WiFiUdp.h>
#include <RTCZero.h>
#include <IPAddress.h>
#include <time.h>

class SNTP_Client {

private:

  // position of fields in ntp packet
  static const int loc_packet_header = 0;
  static const int loc_root_delay = 32;
  static const int loc_root_dispersion = 64;
  static const int loc_reference_id = 96;
  static const int loc_reference_timestamp = 128;
  static const int loc_originate_timestamp = 192;
  static const int loc_receive_timestamp = 256;
  static const int loc_transmit_timestamp = 320;
  static const int loc_key_identifier = 384;
  static const int loc_message_digest = 416;

  // byte masts for ntp header fields
  static const unsigned short LI_mask = 0b1100000;
  static const unsigned short VN_mask = 0b0011000;
  static const unsigned short mode_mask = 0b0000111;

  // ntp packet format byte positions
  static const unsigned short loc_header = 0;
  static const unsigned short loc_stratum_pos = 1;
  static const unsigned short poll_pos = 2;
  static const unsigned short precision = 3;

  static const unsigned long _seventy_years = 2208988800UL;
  static const size_t NTP_PACKET_SIZE = 544; // max ntp packet with options
  unsigned int _client_port = 2390;
  unsigned int _server_port = 123;
  unsigned int _timeout = 1000;
  time_t _update_interval = 60;
  unsigned int _jitter = 0;
  unsigned int _offset = 0;
  unsigned int _adjust_factor = 3;
  bool _connected = false;
  time_t _last_update = 0;
  bool _set_time_at_begin = true;
  time_t _reference_timestamp = 0;   // time server was last updated/corrected
  time_t _originate_timestamp = 0;   // request sent by client
  time_t _receive_timestamp = 0;     // request received by server
  time_t _transmit_timestamp = 0;    // request transmitted by server
  time_t _destination_timestamp = 0; // reply received by client
  
  byte _ntp_server_ip[4] = {192, 168, 40, 5};
  IPAddress _ntp_server;
  byte packet_buffer[NTP_PACKET_SIZE];

  WiFiUDP udp_port;
  RTCZero rtc;
  
public:
  SNTP_Client(void) {
    _ntp_server = IPAddress(_ntp_server_ip);
  }

  int begin(void);
  int end(void);
  int update(void);

private:
  int send_NTP_packet(void);
  unsigned long parse_NTP_packet(void);
  void print_byte_array(byte*);                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
};

#endif // SNTP_CLIENT_H
