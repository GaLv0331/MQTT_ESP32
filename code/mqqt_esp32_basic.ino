#include <WiFi.h>
#include <PubSubClient.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>
#include "config.h"

DynamicJsonDocument sensorDataPayload(1024);
char sensorDataFormatForMqttPublish[1024];

WiFiClient MQTTclient;
PubSubClient client(MQTTclient);

long lastReconnectAttempt = 0;

boolean reconnect(){
  //if(client.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)){
  if(client.connect(MQTT_CLIENT_ID)){
    Serial.println("Attempting to connect to broker");
    client.subscribe(MQTT_TOPIC_SUBSCRIBE);
  }
  return client.connected();
}

void handleIncomingMessage(char* topic, byte* payload, unsigned int length) {
  String message;
  for(unsigned int i=0; i<length; i++){
    message += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if(String(topic) == MQTT_TOPIC_SUBSCRIBE){
    if(message == "1"){
      digitalWrite(LED_PIN, HIGH);
    }
    else if(message == "0"){
      digitalWrite(LED_PIN, LOW);
    }
  }
}


void setup() {
  Serial.begin(115200);
  Serial.println("Attempting to connect to WiFi....");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print("Couldn't connect to Wifi.");
  }
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
  String macAddress = WiFi.macAddress();
  Serial.println("MAC Address: " + macAddress);

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(handleIncomingMessage);

  lastReconnectAttempt = 0;

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    long now = millis();
    if(now - lastReconnectAttempt > 5000){
      lastReconnectAttempt = now;
      if(reconnect()){
        lastReconnectAttempt = 0;
      }
    }
  }
  else{
    Serial.println("Connected to broker --- !!");
    client.loop();

    float humidity = random(0,100);
    float temperature = random(-16,56);  

    sensorDataPayload["Temperature"] = temperature;
    sensorDataPayload["Humidity"] = humidity;

    serializeJson(sensorDataPayload, sensorDataFormatForMqttPublish);

    delay(2000);

    Serial.println("Humidity: " + String(humidity));
    Serial.println("Temperature: " + String(temperature));

    client.publish(MQTT_TOPIC_PUBLISH, sensorDataFormatForMqttPublish);
    Serial.println("Sensor data sent to broker");

    delay(5000);
  }
}
