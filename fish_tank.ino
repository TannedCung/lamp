#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h> // Include the Time library

// setup for network
const char* ssid = "Twenty  Two";
const char* password = "1qwertyuiop0";
const char* mqtt_server = "192.168.1.4";
const char* mqtt_topic = "/light/command";
const char* mqtt_log_topic = "/light/logs";
WiFiClient espClient;
PubSubClient client(espClient);
const char* ntpServerName = "pool.ntp.org";
const int timeZone = 7; // Change this according to your timezone offset
WiFiUDP udp;
unsigned int localPort = 8888; // Local port to listen for UDP packets

// Application setup
unsigned long currentTime;
unsigned long onlineTime;
unsigned long timeSetFromNTP;

// setup pin connect
const int lampPin = 4; // Modify this according to your setup
const int indicatorPin = 5; // Modify this according to your setup

unsigned long lastMillis = 0;
bool isLampOn = false;

void blink(){
  digitalWrite(indicatorPin, LOW);
  delay(1000);
  digitalWrite(indicatorPin, HIGH);
  delay(100);
  digitalWrite(indicatorPin, LOW);
  delay(1000);
  digitalWrite(indicatorPin, HIGH);
  delay(100);
  digitalWrite(indicatorPin, LOW);
  delay(1000);
  digitalWrite(indicatorPin, HIGH);
  delay(100);
}

void setup() {
  pinMode(lampPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(indicatorPin, OUTPUT);
  
  digitalWrite(lampPin, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(indicatorPin, LOW);

  Serial.begin(9600);
  setup_wifi();
  udp.begin(localPort);
  Serial.println("Waiting for time");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  setTimeFromNTP();
  timeSetFromNTP = now(); // Store the time set from NTP
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  sendMQTTMessage(String(currentTime).c_str());
  delay(1000);
}

void sendMQTTMessage(const char* message) {
  if (!client.connected()) {
    reconnect();
  }
  client.publish(mqtt_log_topic, message);
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0'; // Add string termination
  String receivedMessage = String((char*)payload);
  get_current_timestamp();
  sendMQTTMessage((receivedMessage + String(currentTime)).c_str());
  if (receivedMessage.equals("warm")) {
    changeLampMode("warm");
  } else if (receivedMessage.equals("cold")) {
    changeLampMode("cold");
  } else if (receivedMessage.equals("fullbrightness")) {
    changeLampMode("fullbrightness");
  } else {
  blink();
  }
}

void changeLampMode(String mode) {
  if (mode.equals("warm")) {
    // Implement logic to change to warm mode (example: turn on and off quickly)
    toggleLamp();
  } else if (mode.equals("cold")) {
    // Implement logic to change to cold mode (example: turn on and off quickly)
    toggleLamp();
  } else if (mode.equals("fullbrightness")) {
    // Implement logic to change to full brightness mode (example: turn on and off quickly)
    toggleLamp();
  }
}

void setTimeFromNTP() {
  byte packetBuffer[48]; // NTP time is in the first 48 bytes of the message
  udp.beginPacket(ntpServerName, 123); // NTP requests are to port 123
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
    unsigned long epochTime = secsSince1900 - seventyYears;

    sendMQTTMessage(("[INFO]: epochTime: " + String(epochTime) + " | seventyYears: " + String(seventyYears) + " | secsSince1900: " + String(secsSince1900)).c_str());

    setTime(epochTime);
    Serial.println("Time set successfully");
  } else {
    Serial.println("Failed to get time");
  }
}

void get_current_timestamp(){
  currentTime = now();
  onlineTime = currentTime - timeSetFromNTP;
}

void toggleLamp() {
  digitalWrite(lampPin, LOW);
  delay(100); // Adjust this delay as needed for your lamp's quick on/off requirement
  digitalWrite(lampPin, HIGH);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(lampPin, HIGH);

  }
  digitalWrite(lampPin, LOW);

  Serial.println("");
  Serial.println("WiFi connected");
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000); 
  digitalWrite(indicatorPin, HIGH);
  delay(1000); 
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000); 
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(mqtt_topic); 
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
