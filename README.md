## 🛠️ System Architecture

The ESP32 publishes data to the Mosquitto Broker, which can then be subscribed to by any MQTT client (such as a terminal client).

| Component | Role | Platform |
| :--- | :--- | :--- |
| **ESP32** | Publisher (sends data) | Embedded System |
| **Mosquitto Broker** | Message Hub | Ubuntu PC |
| **Client** | Subscriber (receives data) | Ubuntu Terminal |

## ⚙️ Requirements

### Hardware

  * Common Wi-Fi network for both ESP32 and PC

### Software

  * MQTT Explorer

## 🚀 Setup Steps

### Step 1: Install Mosquitto on Ubuntu

Update your system packages and install the Mosquitto broker and client tools:

```bash
# Update system
sudo apt update 
sudo apt upgrade

# Install Mosquitto broker and client tools
sudo apt install mosquitto mosquitto-clients
```

Verify the installation by checking the version:

```bash
sudo systemctl status mosquitto
```

### Step 2: Start & Enable Mosquitto Service

Start the Mosquitto broker and enable it to start automatically at boot:

```bash
# Start broker
sudo systemctl start mosquitto

# Enable at boot
sudo systemctl enable mosquitto
```

Check the service status:

```bash
sudo systemctl status mosquitto
```

### Step 3: Configure Mosquitto for Network Access

Edit the configuration file to allow network connections:

```bash
sudo nano /etc/mosquitto/mosquitto.conf
```

Add the following lines to the file:

```conf
listener 1883
allow_anonymous true
```

Restart the broker to apply the changes:

```bash
sudo systemctl restart mosquitto
```

### Step 4: Test Broker Locally

Open three separate terminal windows on your Ubuntu PC:

1. **mosquitto MQTT Broker Terminal:** Check for verbose logs and flow of events
    ```bash
    mosquitto -v
    ```
2.  **Subscriber Terminal:** Listen for messages on `test/topic`:
    ```bash
    mosquitto_sub -h localhost(usually <127.0.0.1>) -t test/topic
    ```
3.  **Publisher Terminal:** Send a test message:
    ```bash
    mosquitto_pub -h localhost(usually <127.0.0.1>) -t test/topic -m "Hello MQTT"
    ```

If the message "Hello MQTT" appears in the subscriber terminal, the broker is working correctly.

### Step 5: ESP32 MQTT Publisher Code

Use the following base code, replacing the placeholder values (`YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`, and `192.168.1.10`) with your actual Wi-Fi credentials and the Ubuntu PC's IP address (broker).

Common Brokers: `"broker.hivemq.com";` `"test.mosquitto.org";`

**Below Code is of config.h:**
```cpp
#ifndef CONFIG_H
#define CONFIG_H

#define LED_PIN   2

const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

//const char *MQTT_BROKER = "broker.hivemq.com";
const char *MQTT_BROKER = "test.mosquitto.org";
const int MQTT_PORT = 1883;
const char *MQTT_CLIENT_ID = "node_esp32";

const char *MQTT_USERNAME = "desd";
const char *MQTT_PASSWORD = "desd123";

const char *MQTT_TOPIC_PUBLISH = "cdac/desd/telemetry";
const char *MQTT_TOPIC_SUBSCRIBE = "cdac/desd/led/control";

const uint8_t NEW_MAC_ADDRESS[] = {0xdc, 0x1b, 0xa1, 0x64, 0x5e, 0xbf};

#endif
```
**Below Code is of main.ino:**
```cpp
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
```

### Step 6: Verify ESP32 → Ubuntu Communication

Compile and upload the code to your ESP32.

On the Ubuntu PC, open a terminal and subscribe to the topic the ESP32 is publishing to:

```bash
mosquitto_sub -h test.mosquitto.org -t cdac/desd/telemetry
```

You should see the message being received:

```
{"Temperature":45,"Humidity":82}
```

## 🐛 Debug & Monitoring

| Issue | Cause | Solution |
| :--- | :--- | :--- |
| **ESP32 not connecting** | Wrong broker IP | Use the Ubuntu PC's IP, not `localhost` |
| **Connection refused** | Broker not listening | Check the Mosquitto configuration for `listener 1883` |
| **No messages received** | Topic mismatch | Topics must match exactly (e.g., `esp32/data`) |
| **Frequent disconnects** | Weak Wi-Fi | Improve the Wi-Fi signal or increase the connection delay |

### Mosquitto Logs

View real-time Mosquitto service logs for deep debugging:

```bash
journalctl -u mosquitto -f
```

## 📈 Future Enhancements

The project can be extended with the following features:

  * MQTT Authentication (username/password)
  * TLS Security (Port 8883)
  * Configuring QoS Levels (0, 1, 2)
  * Retained Messages
  * ESP32 Subscriber Mode (for bi-directional communication)
  * Using JSON Payloads for structured data
  * Implementing FreeRTOS-based MQTT Tasks

-----

## 🎓 Learning Outcome

By completing this project, you will achieve:

  * Practical understanding of the MQTT protocol
  * Hands-on broker configuration experience
  * Knowledge of ESP32 $\text{Wi-Fi} + \text{TCP}$ communication
  * A foundation in IoT system-level design

-----

I can also help you align this README with your ESP32 code, add more detailed diagrams, or convert this into portfolio-quality documentation, including a TLS + authentication section.

Would you like me to add the TLS and authentication steps to this guide?
