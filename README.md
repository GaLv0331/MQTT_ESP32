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
```
Update Config.h file:
       const char *ssid = "YOUR_WIFI_SSID";
       const char *password = "YOUR_WIFI_PASSWORD";
       const char *MQTT_BROKER = "broker.hivemq.com";
       const char *MQTT_CLIENT_ID = "nodeESP32";
```
### Step 6: Verify ESP32 → Ubuntu Communication

Compile and upload the code to your ESP32.

On the Ubuntu PC, open a terminal and subscribe to the topic the ESP32 is publishing to:

```bash
mosquitto_sub -h test.mosquitto.org -t "cdac/desd/telemetry"
```

You should see the message being received:

```
{"Temperature":45,"Humidity":82}
```

Open another terminal and publish to the topic the ESP32 is subscribing to:
```bash
mosquitto_pub -h test.mosquitto.org -t "cdac/desd/led/control" -m 1
```

You should see the message being received on ESP32 and the On-Board LED should turn ON:
```
Message arrived [cdac/desd/led/control]: 1
```

## 🐛 Debug & Monitoring

| Issue | Cause | Solution |
| :--- | :--- | :--- |
| **ESP32 not connecting** | Wrong broker IP | Use the Ubuntu Broker IP, not `localhost` |
| **Connection refused** | Broker not listening | Check the Mosquitto configuration for `listener 1883` |
| **No messages received** | Topic mismatch | Topics must match exactly (e.g., `esp32/data`) |
| **Frequent disconnects** | Weak Wi-Fi | Improve the Wi-Fi signal or increase the connection delay |

### Mosquitto Logs

View real-time Mosquitto service logs for deep debugging:

```bash
journalctl -u mosquitto -f
```
