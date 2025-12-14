You got it. Here is the GitHub-ready `README.md` for the MQTT Communication setup using Mosquitto Broker on Ubuntu and ESP32 as a publisher, with the requested diagrams.

-----

# 🔗 MQTT Communication Using Mosquitto Broker & ESP32 (Ubuntu)

This project demonstrates MQTT-based communication where an **ESP32** acts as a **Publisher** and a **Mosquitto Broker** runs on an **Ubuntu PC**. The setup is highly suitable for IoT, embedded systems, and protocol benchmarking use cases.

## 📝 Overview

**MQTT (Message Queuing Telemetry Transport)** is a lightweight publish-subscribe messaging protocol. It is widely used in IoT and embedded systems due to its low bandwidth and low power consumption requirements.

## 🧠 Concepts Used

  * Publish-Subscribe Architecture
  * MQTT Broker / Client Model
  * TCP/IP Networking
  * Embedded Wi-Fi Communication

## 🛠️ System Architecture

The ESP32 publishes data to the Mosquitto Broker, which can then be subscribed to by any MQTT client (such as a terminal client).

| Component | Role | Platform |
| :--- | :--- | :--- |
| **ESP32** | Publisher (sends data) | Embedded System |
| **Mosquitto Broker** | Message Hub | Ubuntu PC |
| **Client** | Subscriber (receives data) | Ubuntu Terminal |

## ⚙️ Requirements

### Hardware

  * ESP32 Development Board
  * USB Cable
  * Ubuntu PC (20.04 / 22.04 recommended)
  * Common Wi-Fi network for both ESP32 and PC

### Software

  * Ubuntu Linux
  * Arduino IDE / PlatformIO
  * Mosquitto MQTT Broker
  * Mosquitto Clients
  * `PubSubClient` library (for ESP32)

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
mosquitto -v
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

> **Note:** Anonymous access is enabled here for learning/testing purposes only. For production, **MQTT Authentication** (username/password) is highly recommended.

Restart the broker to apply the changes:

```bash
sudo systemctl restart mosquitto
```

### Step 4: Find Ubuntu PC IP Address

Find the local IP address of your Ubuntu PC, which the ESP32 will use as the MQTT broker address:

```bash
ip a
```

> **Example:** Look for the IP address under the `inet` field (e.g., `inet 192.168.1.10`). This IP will be used as the MQTT broker address in the ESP32 code.

### Step 5: Test Broker Locally

Open two separate terminal windows on your Ubuntu PC:

1.  **Subscriber Terminal:** Listen for messages on `test/topic`:
    ```bash
    mosquitto_sub -h localhost -t test/topic
    ```
2.  **Publisher Terminal:** Send a test message:
    ```bash
    mosquitto_pub -h localhost -t test/topic -m "Hello MQTT"
    ```

If the message "Hello MQTT" appears in the subscriber terminal, the broker is working correctly.

### Step 6: ESP32 MQTT Publisher Setup

Install the **`PubSubClient`** library via the Arduino Library Manager.
The required libraries are `WiFi.h` and `PubSubClient.h`.

### Step 7: ESP32 MQTT Publisher Code

Use the following base code, replacing the placeholder values (`YOUR_WIFI_SSID`, `YOUR_WIFI_PASSWORD`, and `192.168.1.10`) with your actual Wi-Fi credentials and the Ubuntu PC's IP address:

```cpp
#include <WiFi.h> 
#include <PubSubClient.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* mqtt_server = "192.168.1.10"; // Ubuntu PC IP
const int mqtt_port = 1883;

WiFiClient espClient; 
PubSubClient client(espClient); 

void setup_wifi() {
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  } 
} 

void reconnect() { 
  while (!client.connected()) { 
    if (client.connect("ESP32_Publisher")) { // Connected 
      // Add subscribe logic here if needed
    } else { 
      delay(2000); 
    } 
  } 
} 

void setup() { 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
} 

void loop() { 
  if (!client.connected()) {
    reconnect(); 
  } 
  client.loop();

  // Publish a message to the "esp32/data" topic every second
  client.publish("esp32/data", "Hello from ESP32");
  delay(1000); 
}
```

### Step 8: Verify ESP32 → Ubuntu Communication

Compile and upload the code to your ESP32.

On the Ubuntu PC, open a terminal and subscribe to the topic the ESP32 is publishing to:

```bash
mosquitto_sub -h 192.168.1.10 -t esp32/data
```

You should see the message being received:

```
Hello from ESP32
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
