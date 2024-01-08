#include "manager.h"

Manager::Manager(){
    ssid = DEFAULT_SSID;
    password = DEFAULT_PASSWORD;
    mqttServer = DEFAULT_MQTT_SERVER;
    mqttPort = DEFAULT_MQTT_PORT;
    // mqttPassword = DE÷÷÷÷
    topic = DEFAULT_MQTT_TOPIC;
    log_topic = DEFAULT_LOG_TOPIC;
    client = PubSubClient(espClient);
}

void Manager::setup(){
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  device_id = generateDeviceID();

  client.setServer(mqttServer, mqttPort);
  client.setCallback([this](char* topic, byte* payload, unsigned int length) {
    this->callback(topic, payload, length);
  });

  while (!client.connected()) {
    if (client.connect(device_id, mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe(topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying MQTT connection...");
      delay(5000);
    }
  }

}

void Manager::loop(){
  if (!client.connected()) {
      reconnect();
  }
  if (client.connected()){
    client.loop();
  }
  for (int i=0; i < device_list.size(); i++){
    device_list[i]->behave();    
  }
  healthcheck();
}

void Manager::reconnect(){
  int max_attemp = 5;
  int count = 0;
  while (!client.connected()) {
    if (count == max_attemp){
      break;
    }
    Serial.println("Attempting MQTT connection...");
    if (client.connect(device_id, mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe(topic);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying MQTT connection...");
      delay(5000);
    }
    count ++;
  }
}

void Manager::callback(char* topic, byte* payload, unsigned int length){
  payload[length] = '\0';
  std::string receivedMessage = std::string((char*)payload);
  // sendMQTTMessage(receivedMessage.c_str());
  std::string mode = parse_message(receivedMessage);
  for (int i=0; i < device_list.size(); i++){
    Serial.print("[INFO]: changing mode of device ");
    Serial.print(i);
    Serial.print(" to ");
    Serial.println(mode.c_str());
    TheLamp *this_device = device_list[i];
    this_device->change_mode(mode);
  }
}

void Manager::sendMQTTMessage(const char* message) {
  if (!client.connected()) {
    reconnect();
  }
  if (client.connected()){
    client.publish(log_topic, message);
  }
}

void Manager::add_device(int pin) {
  TheLamp *new_lamp = new TheLamp(pin);
  new_lamp->setup();
  device_list.push_back(new_lamp);
}

void Manager::healthcheck(){
  if (now() - last_healthcheck > INTERVAL_HEALTHCHECK){
    last_healthcheck = now();
    json status;
    status["device_id"] = device_id;
    for (int i=0; i < device_list.size(); i++){
      status["lamp_" + std::to_string(i)] = {{"current_mode", device_list[i]->current_mode}, {"lampPin", device_list[i]->lampPin}, {"manual_disable_at", device_list[i]->manual_disable_at}};
    }
    std::string status_str = status.dump(); 
    Serial.print("Sending status:");
    Serial.println(status_str.c_str());
    sendMQTTMessage(status_str.c_str());
  }
}

std::string Manager::parse_message(std::string input, char* separator){
  std::string command;
  std::stringstream ss(input);
  std::string token;
  std::vector<std::string> tokens;

  while (std::getline(ss, token, '_')) {
      tokens.push_back(token);
  }

  if (tokens.size() == 2){
    if (tokens[1] == "ON"){
      command = "Retain";
    } else if (tokens[1] == "OFF"){
      command = "Idle";
    }
  } else if (tokens.size() == 3){
    command = tokens[2];
  }

  return command;
}