#include "lamp.h"
#include "utils.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <vector>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

#define DEFAULT_SSID "Twenty  Two"
#define DEFAULT_PASSWORD "1qwertyuiop0"
#define DEFAULT_MQTT_SERVER "192.168.1.4"
#define DEFAULT_MQTT_PORT 1883
#define DEFAULT_MQTT_TOPIC "/light/command"
#define DEFAULT_LOG_TOPIC "/light/logs"
#define INTERVAL_HEALTHCHECK (1*60)

class Manager{
  private:
    std::vector<TheLamp*> device_list;

    const char* device_id; 
    unsigned long last_healthcheck;
    char* ssid;
    char* password;
    char* mqttServer;
    int mqttPort;
    char* mqttUser;
    char* mqttPassword;
    char* topic;
    char* log_topic;
    WiFiClient espClient;
    PubSubClient client;
    void reconnect();
    void callback(char* topic, byte* payload, unsigned int length);
    
  public:
    Manager();
    void setup();
    void loop();
    void add_device(int pin);
    void sendMQTTMessage(const char* message);
    void healthcheck();
    std::string parse_message(std::string input, char* separator="_");
};