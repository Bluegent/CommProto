#include "ESP8266WiFi.h"
#include <SerialInterface.h>

const char * NetworkName = "CPEP::Tester";
const char * NetworkPassword = "COMPROTO";

IPAddress local_IP(192, 168, 1, 10);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WiFiServer server(9001);
bool started = false;
commproto::serial::SerialInterface serial;

void startWifi()
{
  if (started) {
    return;
  }
  started = true;
  Serial.println();
  Serial.println("Starting as wifi access point");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);   // subnet FF FF FF 00
  WiFi.softAP(NetworkName, NetworkPassword);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  server.begin();
  digitalWrite(LED_BUILTIN, LOW);
}

void setup() {
  serial.initClient("", 115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  if (started)
  {
    WiFiClient client = server.available();
    if (client)
    {
      Serial.println("New connection established");
      while (client.connected())
      {
        if (client.available())
        {
          char c = client.read();
          Serial.write(c);
        }
      }
    }
  }
  else
  {
    if (serial.pollSocket() > 0) {
      // read the incoming byte:
      serial.readByte();
      startWifi();
    }
  }
}
