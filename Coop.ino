
/*

 Chicken Coop Controller

 Manage operation of chicken coop. Primarily open the door at sunrise and close door
 at sunset.
 
 created 18 Nov 2019
 by Rickie Kerndt

 This code is in the public domain.

 */


#include <WiFi101.h>
#include <WiFiUdp.h>
#include "SNTP_Client.h"

// network configuration
char ssid[] = "Kerndt-IoT";
char pass[] = "mx9fj9cT@rP#";
const int MAX_WIFI_RETRY_DELAY = 600000; // milliseconds

// our SNTP client
SNTP_Client ntp;


void setup()
{
 
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the presence of the wifi shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  connect_to_wifi(ssid, pass);

  // kick start the ntp client
  ntp.begin();
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connect_to_wifi(ssid, pass);
  }
  ntp.update();
}

int connect_to_wifi(char* ssid, char* pass)
{
  int retry_delay = 1000;
  int result = WiFi.status();
  
  while ( result != WL_CONNECTED)
  {
    delay(retry_delay);
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    result = WiFi.begin(ssid, pass);
    if (retry_delay < MAX_WIFI_RETRY_DELAY)
    {
      retry_delay *= 2;
    }
  }

  Serial.println("Connected to wifi");
  printWifiStatus();
  
  return result;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
