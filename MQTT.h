// MQTT.h

#ifndef _MQTT_h
#define _MQTT_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
/////

//  
// Prototypes
//

//Инициализация MQTT клиента
void initMQTT();

// Handle messages from MQTT subscription.
void mqttSubscriptionCallback(char* topic, byte* payload, unsigned int length);

// Generate a unique client ID and connect to MQTT broker.
void mqttConnect();

// Subscribe to a field or feed from a ThingSpeak channel.
int mqttSubscribe(long subChannelID, int field, int unSub);

// Publish messages to a channel feed.
void mqttPublish(long pubChannelID, float dataArray[], int fieldArray[]);

//Поддерживает активным соединение с сервером(брокером) MQTT
void MQTTloop();

////// Connect to a given Wi-Fi SSID.
////int connectWifi();

////// Measure the Wi-Fi signal strength.
////void updateRSSIValue();



#endif