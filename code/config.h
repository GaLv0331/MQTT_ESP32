#ifndef CONFIG_H
#define CONFIG_H

#define LED_PIN   2

const char *ssid = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";

const char *MQTT_BROKER = "broker.hivemq.com";

const int MQTT_PORT = 1883;
const char *MQTT_CLIENT_ID = "nodeESP32";

const char *MQTT_USERNAME = "desd";
const char *MQTT_PASSWORD = "desd123";

const char *MQTT_TOPIC_PUBLISH = "cdac/desd/telemetry";
const char *MQTT_TOPIC_SUBSCRIBE = "cdac/desd/led/control";

const uint8_t NEW_MAC_ADDRESS[] = {0xdc, 0x1b, 0xa1, 0x64, 0x5e, 0xbf};

#endif
