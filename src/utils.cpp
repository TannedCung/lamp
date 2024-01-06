#include "utils.h"

void setTimeFromNTP(){
    WiFiUDP udp;
    udp.begin(8888);
    byte packetBuffer[48]; // NTP time is in the first 48 bytes of the message
    udp.beginPacket(NTP_SERVER_NAME, 123); // NTP requests are to port 123
    memset(packetBuffer, 0, 48);
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0; // Stratum, or type of clock
    packetBuffer[2] = 6; // Polling Interval
    packetBuffer[3] = 0xEC; // Peer Clock Precision
    udp.write(packetBuffer, 48);
    udp.endPacket();

    delay(1000); // Give some time for the UDP packet to return

    if (udp.parsePacket()) {
        udp.read(packetBuffer, 48); // Read the packet into the buffer

        unsigned long secsSince1900;
        // NTP time is in the first 48 bytes of the message
        secsSince1900 = (unsigned long)packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long)packetBuffer[43];

        const unsigned long seventyYears = 2208988800UL;
        unsigned long epochTime = secsSince1900 - seventyYears + TIME_ZONE * 3600;
        setTime(epochTime);
        Serial.println("Time set successfully");
    } else {
        Serial.println("Failed to get time");
    }
}

const char* generateDeviceID() {
    // Get ESP8266 MAC address
    uint8_t mac[6];
    WiFi.macAddress(mac);

    // Generate device ID from MAC address
    static char deviceID[18]; // MAC address contains 6 bytes (2 hex digits each) and 5 colons
    sprintf(deviceID, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return deviceID;
}
