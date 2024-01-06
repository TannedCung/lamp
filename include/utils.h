#include <WiFiUdp.h>
#include <TimeLib.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>


#define NTP_SERVER_NAME "pool.ntp.org"
#define TIME_ZONE 7

void setTimeFromNTP();
const char* generateDeviceID();