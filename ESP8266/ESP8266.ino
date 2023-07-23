#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Truong*Thang";
const char* password = "22112001";

IPAddress broadcastIP(192, 168, 1, 255);
const int proxyPort1 = 9;      // Replace with your desired proxy port 1
const int proxyPort2 = 20388;  // Replace with your desired proxy port 2

WiFiUDP udp1;
WiFiUDP udp2;

void setup() {
  Serial.begin(9600);
  delay(100);

  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  udp1.begin(proxyPort1);  // Local port to listen for incoming UDP packets on port 1
  udp2.begin(proxyPort2);  // Local port to listen for incoming UDP packets on port 2
}

void loop() {
  processUDP(udp1);
  processUDP(udp2);
}

void processUDP(WiFiUDP& udp) {
  int packetSize = udp.parsePacket();

  if (packetSize) {
    IPAddress remoteIP = udp.remoteIP();
    uint16_t remotePort = udp.remotePort();
    Serial.print("Received packet from ");
    Serial.print(remoteIP);
    Serial.print(":");
    Serial.println(remotePort);

    // Read the incoming packet's data into a buffer
    char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
    int len = udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    if (udp.remoteIP() == WiFi.localIP() && (udp.remotePort() == proxyPort1 || udp.remotePort() == proxyPort2)) {
      return;
    }
    // Send the packet to the proxy server
    if (udp.localPort() == proxyPort1) {
      Serial.println("udp1 send");

      udp.beginPacket(broadcastIP, proxyPort1);

    } else if (udp.localPort() == proxyPort2) {
      Serial.println("udp2 send");

      udp.beginPacket(broadcastIP, proxyPort2);
    }

    udp.write(packetBuffer, len);
    udp.endPacket();
    Serial.println("Sended packet");
    Serial.println(udp.localPort());
  }
}
