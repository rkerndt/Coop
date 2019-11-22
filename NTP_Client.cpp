  
#include "NTP_Client.h"  

int NTP_client::begin(void) {

  // kick start the real time clock
  rtc.begin();

  // claim a udp port
  if (udp_port.begin(_client_port) == 1)
  {
    _connected == true;
  }
  else
  {
    _connected == false;
    if (Serial)
    {
      Serial.print("Could not open udp on port ");
      Serial.println(_client_port);
    }
  }
  return _connected;
}

  
int NTP_client::end(void) {
    udp_port.flush();
    udp_port.stop();
    _connected = false;
}

int NTP_client::send_NTP_packet(void) {

  int result = 0;
  
  // initialize buffer with packet contents
  memset(packet_buffer, 0, NTP_PACKET_SIZE);
  packet_buffer[0] = 0b11100011; // LI, Version, Mode
  packet_buffer[1] = 0;          // Stratum, or type of clock
  packet_buffer[2] = 6;          // Polling interval
  packet_buffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packet_buffer[12] = 49;
  packet_buffer[13] = 0x4E;
  packet_buffer[14] = 49;
  packet_buffer[15] = 52;

  result = udp_port.beginPacket(ntp_server, _server_port);
  if (result == 1)
  {
    if (Serial)
    {
      Serial.print("Beginning packet to ");
      Serial.print(ntp_server);
      Serial.print(" port ");
      Serial.println(_server_port);
    }
    result = udp_port.write(packet_buffer, NTP_PACKET_SIZE);
    if ( (result == 0) && (Serial) )
    {
      Serial.print("Failed to write buffer on port ");
      Serial.println(_client_port);
    }
  }
  else if (Serial)
  {
    Serial.print("Failed beginPacket on port ");
    Serial.println(_client_port);
  }
  if (result == 1)
  {
    result = udp_port.endPacket();
    if (result == 0 && (Serial))
    {
      Serial.print("Failed to endPacket on port ");
      Serial.println(_client_port);
    }
  }
  return result;
}
  
int NTP_client::update(void) {

  unsigned int rtc_time = rtc.getEpoch();
  
  if ( (_last_update - rtc_time) >= _update_interval)
  {
    // get microseconds to use in calculating round trip time
    unsigned long sent_ntp_request = micros();

    // send the NTP packet and process the returned packet
    int result = send_NTP_packet();

    if (result == 1)
    {
      result = 0;
      int cumm_delay = 0;
      int delay_value = _timeout / 10;
      while (result == 0)
      {
        result = udp_port.parsePacket();
        delay(delay_value);
        cumm_delay += delay_value;
        if (cumm_delay > _timeout)
        {
          break;
        }
      }
    }
    
    if (result > 0)
    {
      udp_port.read(packet_buffer, NTP_PACKET_SIZE); // read the packet into the buffer
  
      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words: 
      unsigned long highWord = word(packet_buffer[40], packet_buffer[41]);
      unsigned long lowWord = word(packet_buffer[42], packet_buffer[43]);
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long ntp_time = highWord << 16 | lowWord;      
      ntp_time -= _seventy_years;

      // calculate the round trip time and calculate jitter
      unsigned int received_ntp_response = micros();
      unsigned int rtt = received_ntp_response - sent_ntp_request;

      // update jitter as an exponential average: alpha=0.2
      if (_jitter > 0)
      {
        _jitter = (_jitter * 0.8) + (rtt * 0.2);
      }
      else
      {
        _jitter = rtt;
      }

      // set timeout to 10 times jitter
      _timeout = _jitter / 100;
      
      // calculate offset, note that unless jitter is greater than 1sec it is not going to effect the offset
      // resolution of 1sec.
      _offset = ntp_time - rtc_time;
      if (_jitter > 1000000L)
      {
        _offset + (rtt / 5000000L);
      }

      // TODO: add in an adjustment algorithm, for now printout the rtt, jitter and offset values
      if (Serial)
      {
        Serial.print("Received NTP response in ");
        Serial.print(rtt);
        Serial.print(" usecs, jitter = ");
        Serial.print(_jitter);
        Serial.print(" usecs, offset = ");
        Serial.print(_offset);
        Serial.println(" secs");
      }
      // Update last time received ntp response
      _last_update = rtc_time;
    }
    else
    {
      if (Serial)
      {
        Serial.println("NTP response timeout");
      }
    }
  }
}
