
/*******************************************************************************
 * A simple yet crude ntp client to keep the Andruino real time clock reasonably
 * accurate.
 */

#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

#include <Arduino.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <RTCZero.h>


#define NTP_PACKET_SIZE 48

class NTP_client {

private:

  const unsigned long _seventy_years = 2208988800UL;
  unsigned int _client_port = 2390;
  unsigned int _server_port = 123;
  unsigned int _timeout = 1000;
  unsigned int _update_interval = 60000;
  unsigned int _jitter = 0;
  unsigned int _offset = 0;
  unsigned int _adjust_factor = 3;
  bool _connected = false;
  unsigned long _last_update = 0;
  
  IPAddress ntp_server(192, 168, 40, 5);
  byte packet_buffer[NTP_PACKET_SIZE];

  WiFiUDP udp_port;
  RTCZero rtc;
  
public:
  NTP_client(void) {}

  int begin(void);
  int end(void);
  int update(void);


  int send_NTP_packet(void);
  unsigned long parse_NTP_packet(void);                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
};

#endif // NTP_CLIENT_H
