





/* Ётот код недоработан */
	//				/*”правление через MQTT брокера ThingSpeak
	//				info:
	//				https://www.mathworks.com/help/thingspeak/use-arduino-client-to-publish-to-a-channel.html

	//				Channel ID: 1627034
	//				ѕараметры брокера MQTT дл€ канала:
	//				–егистрационные данные дл€ устройства:
	//				USERNAME "KjMLOiQ0FyYiGDwDHwIkACA"
	//				CLIENT_ID "KjMLOiQ0FyYiGDwDHwIkACA"
	//				PASSWORD "5IcKiXd+RpWddVPe7FPJ/tHJ"
	//				Topic:
	//				channels/<channelID>/publish
	//				channels/<channelID>/publish/fields/field<fieldnumber>
	//				https://ww2.mathworks.cn/help/thingspeak/mqtt-api.html?s_tid=CRUX_lftnav

	//				»спользуютс€ следующие пол€:
	//				Field1	cmdFromClient	«апрос, команда от клиентского приложени€ к устройству
	//				Field3	cmdFromDevice	—ообщение от устройства клиентам.
	//				*/

					#include <PubSubClient.h>

	//				//включить режим отладки
	//				#define DEBUG_ENABLE_MQTT 

	//				#ifdef DEBUG_ENABLE_MQTT
	//					#define DEBUG_PRINT_MQTT(x) (Serial.print(x))
	//					#define DEBUG_PRINTLN_MQTT(x) (Serial.println(x))
	//					#define DEBUGR_PRINTR_MQTT(x,r) (Serial.print(x,r))
	//				#else
	//				#define DEBUG_PRINT_MQTT(x)
	//				#define DEBUG_PRINTLN_MQTT(x) 
	//				#define DEBUGR_PRINTR_MQTT(x,r)
	//				#endif 

	//				//MQTT Devices дл€ устройства системы
	//				#ifdef DEBUG_ENABLE_MQTT
	//					//Test Device_Tonshaevo_HCS_MQTT	//MQTT  »спользуем дл€ тестировани€ на другом тестовом устройстве
	//					#define SECRET_MQTT_USERNAME "EwI2ODYwFwk4NTwXHyMQIzg"
	//					#define SECRET_MQTT_CLIENT_ID "EwI2ODYwFwk4NTwXHyMQIzg"
	//					#define SECRET_MQTT_PASSWORD "aDDjpR+kcCrxeozB6hpZ1JE/"
	//				#else
	//					//Device_Tonshaevo_HCS_MQTT
	//					#define SECRET_MQTT_USERNAME "KjMLOiQ0FyYiGDwDHwIkACA"
	//					#define SECRET_MQTT_CLIENT_ID "KjMLOiQ0FyYiGDwDHwIkACA"
	//					#define SECRET_MQTT_PASSWORD "5IcKiXd+RpWddVPe7FPJ/tHJ"
	//				#endif

	//				//MQTT клиент 2 
	//				//Client_1_Tonshaevo_HCS_MQTT (»спользуем эти данные дл€ внешних программ управлени€ устройством. Ќа ноуте MQTTX)
	//					//#define SECRET_MQTT_USERNAME "NzcPKAglLDs0GBEZAgkdGxA"
	//					//#define SECRET_MQTT_CLIENT_ID "NzcPKAglLDs0GBEZAgkdGxA"
	//					//#define SECRET_MQTT_PASSWORD "SfsDtwgOib0Nm+8CYCFystjb"

	//					//MQTT клиент 3 (мама)
	//				//username = JDoNBwwsCR8pHwkrJyQoDRo
	//				//clientId = JDoNBwwsCR8pHwkrJyQoDRo
	//				//password = WQT4X4x3l99+sr932ZiDiNDr

	//				//MQTT client_3 (MyXiaomi)
	//				//username = BwYVOgYNAzcOIgkVEyoBFww
	//				//clientId = BwYVOgYNAzcOIgkVEyoBFww
	//				//password = nqq5tkA6Qpnbl+UxQ4fAYAjc


	//				//#define MAX_TIME_CONNECT_TO_MQTT 30000	//ћаксимальное врем€ на ожидание подключени€ к брокеру, в мс


	//				char mqttUserName[] = SECRET_MQTT_USERNAME;
	//				char clientID[] = SECRET_MQTT_CLIENT_ID;
	//				char mqttPass[] = SECRET_MQTT_PASSWORD;               // Change to your MQTT API key from Account > MyProfile.
	//				long readChannelID = 1627034; 
	//				long writeChannelID = 1627034;

	//				// It is strongly recommended to use secure connections. However, certain hardware does not work with the WiFiClientSecure library.
	//				// Comment out the following #define to use non-secure MQTT connections to ThingSpeak server. 
	//				#define USESECUREMQTT

	//				#ifdef USESECUREMQTT
	//					#include <WiFiClientSecure.h>
	//					#define MQTT_PORT 8883
	//					WiFiClientSecure client;
	//					//сертификат отпечаток пальца дл€ безопасного соединени€// Here's how to get ThingSpeak server fingerprint: https://www.a2hosting.com/kb/security/ssl/a2-hostings-ssl-certificate-fingerprints
	//					const char* PROGMEM thingspeak_cert_fingerprint = "6D 2B DA A6 29 BD FB 92 4C B7 83 33 22 30 A1 3F 53 AF 51 F6";
	//				#else
	//					#define MQTT_PORT 1883
	//					WiFiClient client;
	//				#endif


	//				// const char* thingspeak_server_fingerprint = "27 18 92 dd a4 26 c3 07 09 b9 7a e6 c5 21 b9 5b 48 f7 16 e1";//for SSL

	//				//Define connection parameters and initialize a WiFiClient and PubSubClient instance.
	//				const char* server = "mqtt3.thingspeak.com"; //"ssl://mqtt3.thingspeak.com";
	//				int connectionDelayMQTT= 1;

	//				//*//
	//				long lastPublishMillis = 0;
	//				int updateInterval = 30;  //интервал, через который отправл€ем соощение в топик
	//				//*//

	//				PubSubClient mqttClient(client);                 

	//				//*//
	//				int fieldsToPublish[8] = { 1,1,0,0,0,0,0,0 };       // Change to allow multiple fields.
	//				float dataToPublish[8];                             // Holds your field data.
	//				//int changeFlag = 0;                                 // Let the main loop know there is new data to set.
	//				//*//

	//				void initMQTT(){
	//					DEBUG_PRINTLN_MQTT(F("start procedure initMQTT"));
	//					// Configure the MQTT client
	//					mqttClient.setServer(server, MQTT_PORT);						// secure port 8883.
	//					// Set the MQTT message handler function.
	//					mqttClient.setCallback(mqttSubscriptionCallback); 
	//					// Set the buffer 2048 to handle the returned JSON. NOTE: A buffer overflow of the message buffer will result in your callback not being invoked.
	//					mqttClient.setBufferSize(2048);
	//					// Use secure MQTT connections if defined.
	//					#ifdef USESECUREMQTT
	//						client.setFingerprint(thingspeak_cert_fingerprint);
	//						//client.setInsecure(); // To perform a simple SSL Encryption !!!!!!!!!!
	//					#endif
	//					// Connect to MQTT server.
	//					mqttConnect();
	//				}

	//				// Connect to MQTT server.
	//				void mqttConnect() {
	//					DEBUG_PRINTLN_MQTT(F("start procedure mqttConnect") );
	//					// Loop until connected.
	//						while (!mqttClient.connected())	{	
	//							// Connect to the MQTT broker.
	//							if (mqttClient.connect(clientID, mqttUserName, mqttPass)) {
	//								Serial.print(F("Connected to the MQTT"));Serial.print(server);	Serial.print(" at port "); Serial.print(MQTT_PORT);	Serial.println(F(" successful."));
	//							}
	//							else {
	//								Serial.print("MQTT connection failed, rc = ");
	//								// See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
	//								Serial.print(mqttClient.state());
	//								Serial.println(" Will try again in a few seconds");
	//								delay(connectionDelayMQTT * 1000);
	//							}
	//						}
	//					}

	//				//ѕроверка и поддержание св€зи с сервером mqtt и подписки на топики, проверка поступлени€ новых сообщений от MQTT брокера.
	//				void mqttloop() {
	//					DEBUG_PRINTLN_MQTT(F("start procedure mqttloop. Connected=")+ String(mqttClient.connected()));
	//					if (!mqttClient.connected()) {
	//						mqttConnect();
	//						mqttSubscribe(readChannelID);
	//					}
	//					// Call the loop to maintain connection to the server. 
	//					mqttClient.loop();
	//				}

	//				// Process incoming subscription messages, subscribe to receive updates, and publish messages.
	//				// Function to handle messages from MQTT subscription.
	//				void mqttSubscriptionCallback(char* topic, byte* payload, unsigned int length) {
	//					DEBUG_PRINTLN_MQTT(F("start procedure mqttSubscriptionCallback"));
	//					// Print the details of the message that was received to the serial monitor.
	//					Serial.print("Message arrived [");
	//					Serial.print(topic);
	//					Serial.print("] ");
	//					for (int i = 0; i < length; i++) {
	//						Serial.print((char)payload[i]);
	//					}
	//					Serial.println();
	//				}

	//				// Subscribe to ThingSpeak channel for updates.
	//				void mqttSubscribe(long subChannelID) {
	//					DEBUG_PRINTLN_MQTT(F("start procedure mqttSubscribe"));
	//					String myTopic = "channels/" + String(subChannelID) + "/subscribe/fields/field1";
	//					mqttClient.subscribe(myTopic.c_str());
	//				}

	//				// Publish messages to a ThingSpeak channel.
	//				void mqttPublish(long pubChannelID, String message) {
	//					DEBUG_PRINTLN_MQTT(F("start procedure mqttPublish"));
	//					String topicString = "channels/" + String(pubChannelID) + "/publish";

	//					topicString = "channels/1627034/publish/fields/field2";
	//					message = "24.33";

	//					mqttClient.publish(topicString.c_str(), message.c_str());


	//					DEBUG_PRINTLN_MQTT(F("publish topic string: ")+ topicString);
	//					DEBUG_PRINTLN_MQTT(F("publish topic message: ") + message);

	//				}


	//				void mqttTest() {
	//					DEBUG_PRINTLN_MQTT(F("start procedure mqttTest"));
	//					//из процедуры loop main примера 
	//					// Update ThingSpeak channel periodically. The update results in the message to the subscriber.
	//					if (abs(long(millis()) - lastPublishMillis) > updateInterval * 1000) {
	//						//mqttPublish(writeChannelID, (String("field2=") + String(WiFi.RSSI())));
	//						mqttPublish(writeChannelID, "field1=45&field2=35");
	//						lastPublishMillis = millis();
	//					}
	//				}

	//				/*
	//				// Publish messages to a ThingSpeak channel.
	//				void mqttPublish(long pubChannelID, String message) {
	//					String topicString = "channels/" + String(pubChannelID) + "/publish";
	//					mqttClient.publish(topicString.c_str(), message.c_str());
	//				}

	//				// Update ThingSpeak channel periodically. The update results in the message to the subscriber.
	//				if (abs(long(millis()) - lastPublishMillis) > updateInterval * 1000) {
	//					mqttPublish(channelID, (String("field1=") + String(WiFi.RSSI())));
	//					lastPublishMillis = millis();
	//				}


	//				*/






	//				/*

	//					//////////////////////	
	//					//////////////////////	//подписываемс€ на необходимые топики
	//					//////////////////////	if (mqttSubscribe(readChannelID, 1, 0) == 1) { //cmdFromClient - field 1	
	//					//////////////////////		DEBUG_PRINTLN_MQTT("Subscribed field 1 cmdFromClient");
	//					//////////////////////	}
	//					//////////////////////}

	//				}











	//				// 6) Use the mqttSubscriptionCallback function to handle incoming MQTT messages. The program runs smoother if the main loop performs the 
	//				// processing steps instead of the callback. In this function, use flags to cause changes in the main loop.
	//				//*
	//				*		Process messages received from subscribed channel via MQTT broker.
	//				*   topic - Subscription topic for message.
	//				*   payload - Field to subscribe to. Value 0 means subscribe to all fields.
	//				*   mesLength - Message length.
	//				/*
	//				void mqttSubscriptionCallback(char* topic, byte* payload, unsigned int mesLength) {
	//					//сохран€ем сообщение, полученное с сервера брокера во временную переменную.
	//					//char r[22];
	//					char p[mesLength + 1];
	//					memcpy(p, payload, mesLength);
	//					//p[mesLength] = NULL; 

	//					DEBUG_PRINTLN_MQTT("New message in topic. Length: " + String(mesLength));
	//					DEBUG_PRINTLN_MQTT("Topic " + String(topic));
	//					DEBUG_PRINTLN_MQTT("Message: " + String(p));
	//					//пока мы подписаны только на один топик field 1, поэтому определ€ть с какого топика пришло сообщение не нужно.


	//					//преобразуем значение из строки во float, замени
	//					//зап€тую на точку, что бы любой разделитель давал дес€тичную дробь.
	//					char *n = strrchr((char*)payload, ',');
	//					//*n = '.';
	//					if (!(n == nullptr)) { *n = '.'; }

	//					//Serial.println("Posle zameni zapiatoi na tochky message : "+ String((char*)payload));
	//
	//					float f = atof((char*)payload);
	//
	//					//если проверка допустимого диапазона целевой температуры пройдена, передаем команду модулю MEGA
	//					//на запись новой целевой температуры
	//					if (f>=20 && f<=26)
	//					{
	//						///////убрал, когда искал причину зависаний  // delay(15000);
	//						Serial.println("?SetGTargetTemp=" + String(f));
	//						DEBUG_PRINTLN_MQTT("Target temperature sent to MEGA");
	//					}
	//					else {
	//						DEBUG_PRINTLN_MQTT("Target temperature outside the permissible range");
	//					}
	//				}



	//				// 8) Use mqttSubscribe to receive updates from the LED control field. In this example, you subscribe to a field, but you can also use this function
	//				// to subscribe to the whole channel feed. Call the function with field = 0 to subscribe to the whole feed.
	//				//
	//				* Subscribe to fields of a channel.
	//				*   subChannelID - Channel to subscribe to.
	//				*   field - Field to subscribe to. Value 0 means subscribe to all fields.
	//				*   readKey - Read API key for the subscribe channel.
	//				*   unSub - Set to 1 for unsubscribe.
	//				//
	//				int mqttSubscribe(long subChannelID, int field, int unsubSub) {
	//					String myTopic;

	//					// There is no field zero, so if field 0 is sent to subscribe to, then subscribe to the whole channel feed.
	//					if (field == 0) {
	//						myTopic = "channels/" + String(subChannelID) + "/subscribe";
	//					}
	//					else {
	//						myTopic = "channels/" + String(subChannelID) + "/subscribe/fields/field" + String(field);
	//					}

	//					DEBUG_PRINT_MQTT("Subscribing to " + myTopic + " ");
	//					DEBUG_PRINTLN_MQTT("State= " + String(mqttClient.state())+String(", if 0 then it is MQTT_CONNECTED"));  // 0 : MQTT_CONNECTED - the client is connected

	//					if (unsubSub == 1) {
	//						return mqttClient.unsubscribe(myTopic.c_str());
	//					}
	//					return mqttClient.subscribe(myTopic.c_str(), 0);
	//				}


	//				// 9) The mqttUnsubscribe function is not used in the code, but you can use it to end a subscription.
	//				//
	//				//* Unsubscribe channel
	//				//*   subChannelID - Channel to unsubscribe from.
	//				//*   field - Field to unsubscribe subscribe from. The value 0 means subscribe to all fields.
	//				//*   readKey - Read API key for the subscribe channel.
	//				//
	//				int mqttUnSubscribe(long subChannelID, int field, char* readKey) {
	//					String myTopic;

	//					if (field == 0) {
	//						myTopic = "channels/" + String(subChannelID) + "/subscribe";
	//					}
	//					else {
	//						myTopic = "channels/" + String(subChannelID) + "/subscribe/fields/field" + String(field);
	//					}
	//					return mqttClient.unsubscribe(myTopic.c_str());
	//				}


	//				// 10) Use the mqttPublish function to send your angle and Wi-Fi RSSI data to a ThingSpeak channel.
	//				/**
	//				//* Publish to a channel
	//				//*   pubChannelID - Channel to publish to.
	//				//*   pubWriteAPIKey - Write API key for the channel to publish to.
	//				//*   dataArray - Binary array indicating which fields to publish to, starting with field 1.
	//				//*   fieldArray - Array of values to publish, starting with field 1.
	//				//
	//				void mqttPublish(long pubChannelID, float dataArray[], int fieldArray[]) {
	//					int index = 0;
	//					String dataString = "";

	//					//dataToPublish[ ] = float(rssi);

	//															// 
	//					while (index<8) {

	//						// Look at the field array to build the posting string to send to ThingSpeak.
	//						if (fieldArray[index]>0) {

	//							dataString += "&field" + String(index + 1) + "=" + String(dataArray[index]);
	//						}
	//						index++;
	//					}

	//					Serial.println(dataString);
	//					DEBUG_PRINTLN_MQTT(dataString);

	//					// Create a topic string and publish data to ThingSpeak channel feed.
	//					String topicString = "channels/" + String(pubChannelID) + "/publish";
	//					Serial.println("topicString " + topicString);
	//					mqttClient.publish(topicString.c_str(), dataString.c_str());
	//					Serial.println("Published to channel " + String(pubChannelID));
	//				}

	//				*/