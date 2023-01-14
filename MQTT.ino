/*”правление через MQTT брокера ThingSpeak
Channel ID: 1627034
ѕараметры брокера MQTT дл€ канала:
–егистрационные данные дл€ устройства:
USERNAME "KjMLOiQ0FyYiGDwDHwIkACA"
CLIENT_ID "KjMLOiQ0FyYiGDwDHwIkACA"
PASSWORD "5IcKiXd+RpWddVPe7FPJ/tHJ"
Topic:
channels/<channelID>/publish
channels/<channelID>/publish/fields/field<fieldnumber>
https://ww2.mathworks.cn/help/thingspeak/mqtt-api.html?s_tid=CRUX_lftnav

»спользуютс€ следующие пол€:
Field1	cmdFromClient	«апрос, команда от клиентского приложени€ к устройству
Field2	cmdFromDevice	—ообщение от устройства клиентам.
*/

//#include <PubSubClient.h>

//#define DEBUGMQTT //включить вывод отладочной информации
#ifdef DEBUGMQTT
	#define DEBUG_MQTT(x) (Serial.print(x))
	#define DEBUGLN_MQTT(x) (Serial.println(x))
	#define DEBUGR_MQTT(x,r) (Serial.print(x,r))
#else
	#define DEBUG_MQTT(x) 
	#define DEBUGLN_MQTT(x)
	#define DEBUGR_MQTT(x,r) 
#endif 

//MQTT клиент 1 
//Device_Tonshaevo_HCS_MQTT
 #define SECRET_MQTT_USERNAME "KjMLOiQ0FyYiGDwDHwIkACA"
 #define SECRET_MQTT_CLIENT_ID "KjMLOiQ0FyYiGDwDHwIkACA"
 #define SECRET_MQTT_PASSWORD "5IcKiXd+RpWddVPe7FPJ/tHJ"

//MQTT клиент 2 
//Client_1_Tonshaevo_HCS_MQTT (»спользуем эти данные дл€ внешних программ управлени€ устройством)
	//#define SECRET_MQTT_USERNAME "NzcPKAglLDs0GBEZAgkdGxA"
	//#define SECRET_MQTT_CLIENT_ID "NzcPKAglLDs0GBEZAgkdGxA"
	//#define SECRET_MQTT_PASSWORD "SfsDtwgOib0Nm+8CYCFystjb"

	//MQTT клиент 3 (мама)
//username = JDoNBwwsCR8pHwkrJyQoDRo
//clientId = JDoNBwwsCR8pHwkrJyQoDRo
//password = WQT4X4x3l99+sr932ZiDiNDr


#define MAX_TIME_CONNECT_TO_MQTT 30000	//ћаксимальное врем€ на ожидание подключени€ к брокеру, в мс
/* удали, это угол поворота серво*/ //#define ANGLE_FIELD 0   
#define DATA_FIELD 1										// Data field to post the signal strength to.

const char* server = "mqtt3.thingspeak.com";
char mqttUserName[] = SECRET_MQTT_USERNAME;
char mqttPass[] = SECRET_MQTT_PASSWORD;               // Change to your MQTT API key from Account > MyProfile.
char clientID[] = SECRET_MQTT_CLIENT_ID;
long readChannelID = 1627034; 
long writeChannelID = 1627034;

// Here's how to get ThingSpeak server fingerprint: https://www.a2hosting.com/kb/security/ssl/a2-hostings-ssl-certificate-fingerprints
// const char* thingspeak_server_fingerprint = "27 18 92 dd a4 26 c3 07 09 b9 7a e6 c5 21 b9 5b 48 f7 16 e1";//for SSL

WiFiClient client;																	// Initialize the Wi-Fi client library. Uncomment for nonsecure connection.																							 //WiFiClientSecure client;                              // Uncomment for secure connection.  
PubSubClient mqttClient(client);                    // Initialize the PubSubClient library.

int fieldsToPublish[8] = { 1,1,0,0,0,0,0,0 };       // Change to allow multiple fields.
float dataToPublish[8];                             // Holds your field data.
//int changeFlag = 0;                                 // Let the main loop know there is new data to set.

//Ќастройка MQTT клиента
void initMQTT() {

	mqttClient.setServer(server, 1883);								// Set the MQTT broker details, nonsecure port. Uncomment for nonsecure connection.
	//mqttClient.setServer( server, 8883 );           // Set the MQTT broker details, secure port. Uncomment for secure connection.
	mqttClient.setCallback(mqttSubscriptionCallback); // Set the MQTT message handler function.
	DEBUGLN_MQTT("module MQTT initialization");
}

//—оздание и поддерживает активным соединение с сервером(брокером) MQTT
void MQTTloop() {
	if (!mqttClient.connected())
	{
		mqttConnect(); // Connect if MQTT client is not connected.
		
		//подписываемс€ на необходимые топики
		if (mqttSubscribe(readChannelID, 1, 0) == 1) { //cmdFromClient - field 1	
			DEBUGLN_MQTT("Subscribed field 1 cmdFromClient");
		}
	}

	mqttClient.loop(); // Call the loop to maintain connection to the server. 

	//if (changeFlag) {
	//	//если было сообщение в топике, то устанавливаетс€ флаг changeFlag
	//	changeFlag = 0;
	//	dataToPublish[ANGLE_FIELD] = 123;
	//	delay(1100);                       // Wait for ThingSpeak to publish.
	//	Serial.println("Servo value " + String("123"));
	//	mqttPublish(writeChannelID, dataToPublish, fieldsToPublish);
	//}

}

// 6) Use the mqttSubscriptionCallback function to handle incoming MQTT messages. The program runs smoother if the main loop performs the 
// processing steps instead of the callback. In this function, use flags to cause changes in the main loop.
/*
*		Process messages received from subscribed channel via MQTT broker.
*   topic - Subscription topic for message.
*   payload - Field to subscribe to. Value 0 means subscribe to all fields.
*   mesLength - Message length.
*/
void mqttSubscriptionCallback(char* topic, byte* payload, unsigned int mesLength) {
	//сохран€ем сообщение, полученное с сервера брокера во временную переменную.
	//char r[22];
	char p[mesLength + 1];
	memcpy(p, payload, mesLength);
	//p[mesLength] = NULL; 

	DEBUGLN_MQTT("New message in topic. Length: " + String(mesLength));
	DEBUGLN_MQTT("Topic " + String(topic));
	DEBUGLN_MQTT("Message: " + String(p));
	//пока мы подписаны только на один топик field 1, поэтому определ€ть с какого топика пришло сообщение не нужно.


	//преобразуем значение из строки во float, замени
	//зап€тую на точку, что бы любой разделитель давал дес€тичную дробь.
	char *n = strrchr((char*)payload, ',');
	//*n = '.';
	if (!(n == nullptr)) { *n = '.'; }

	//Serial.println("Posle zameni zapiatoi na tochky message : "+ String((char*)payload));
	
	float f = atof((char*)payload);
	
	//если проверка допустимого диапазона целевой температуры пройдена, передаем команду модулю MEGA
	//на запись новой целевой температуры
	if (f>=20 && f<=26)
	{
		/*/////убрал, когда искал причину зависаний  */ delay(15000);
		Serial.println("?SetGTargetTemp=" + String(f));
		DEBUGLN_MQTT("Target temperature sent to MEGA");
	}
	else {
		DEBUGLN_MQTT("Target temperature outside the permissible range");
	}
}

// 7) Use the MQTTConnect function to set up and maintain a connection to the MQTT.
void mqttConnect()
{
	// Loop until connected.
	unsigned long start = millis();
	while (!mqttClient.connected() && ((millis() - start)<MAX_TIME_CONNECT_TO_MQTT))
	{
		///DEBUGLN_MQTT(String(mqttUserName) + " , " + mqttPass + " , " + clientID);
		// Connect to the MQTT broker.
		DEBUGLN_MQTT(F("Attempting MQTT connection..."));
		if (mqttClient.connect(clientID, mqttUserName, mqttPass))
		{
			DEBUGLN_MQTT("Connected to MQTT with Client ID:  " + String(clientID) + " User " + String(mqttUserName) + " Pwd " + String(mqttPass));
		}
		else
		{
			DEBUG_MQTT(F("Failed connected to MQTT client, return state = "));
			// See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
			DEBUGLN_MQTT(mqttClient.state());
			DEBUGLN_MQTT(F(" Will try again in 5 seconds"));
			/*//убрал, когда искал причину зависаний */ delay(5000);
		}
	}
}

// 8) Use mqttSubscribe to receive updates from the LED control field. In this example, you subscribe to a field, but you can also use this function
// to subscribe to the whole channel feed. Call the function with field = 0 to subscribe to the whole feed.
/**
* Subscribe to fields of a channel.
*   subChannelID - Channel to subscribe to.
*   field - Field to subscribe to. Value 0 means subscribe to all fields.
*   readKey - Read API key for the subscribe channel.
*   unSub - Set to 1 for unsubscribe.
*/
int mqttSubscribe(long subChannelID, int field, int unsubSub) {
	String myTopic;

	// There is no field zero, so if field 0 is sent to subscribe to, then subscribe to the whole channel feed.
	if (field == 0) {
		myTopic = "channels/" + String(subChannelID) + "/subscribe";
	}
	else {
		myTopic = "channels/" + String(subChannelID) + "/subscribe/fields/field" + String(field);
	}

	DEBUG_MQTT("Subscribing to " + myTopic + " ");
	DEBUGLN_MQTT("State= " + String(mqttClient.state())+String(", if 0 then it is MQTT_CONNECTED"));  // 0 : MQTT_CONNECTED - the client is connected

	if (unsubSub == 1) {
		return mqttClient.unsubscribe(myTopic.c_str());
	}
	return mqttClient.subscribe(myTopic.c_str(), 0);
}


// 9) The mqttUnsubscribe function is not used in the code, but you can use it to end a subscription.
/**
* Unsubscribe channel
*   subChannelID - Channel to unsubscribe from.
*   field - Field to unsubscribe subscribe from. The value 0 means subscribe to all fields.
*   readKey - Read API key for the subscribe channel.
*/
int mqttUnSubscribe(long subChannelID, int field, char* readKey) {
	String myTopic;

	if (field == 0) {
		myTopic = "channels/" + String(subChannelID) + "/subscribe";
	}
	else {
		myTopic = "channels/" + String(subChannelID) + "/subscribe/fields/field" + String(field);
	}
	return mqttClient.unsubscribe(myTopic.c_str());
}


// 10) Use the mqttPublish function to send your angle and Wi-Fi RSSI data to a ThingSpeak channel.
/**
* Publish to a channel
*   pubChannelID - Channel to publish to.
*   pubWriteAPIKey - Write API key for the channel to publish to.
*   dataArray - Binary array indicating which fields to publish to, starting with field 1.
*   fieldArray - Array of values to publish, starting with field 1.
*/
void mqttPublish(long pubChannelID, float dataArray[], int fieldArray[]) {
	int index = 0;
	String dataString = "";

	//dataToPublish[ ] = float(rssi);

											// 
	while (index<8) {

		// Look at the field array to build the posting string to send to ThingSpeak.
		if (fieldArray[index]>0) {

			dataString += "&field" + String(index + 1) + "=" + String(dataArray[index]);
		}
		index++;
	}

	Serial.println(dataString);
	DEBUGLN_MQTT(dataString);

	// Create a topic string and publish data to ThingSpeak channel feed.
	String topicString = "channels/" + String(pubChannelID) + "/publish";
	Serial.println("topicString " + topicString);
	mqttClient.publish(topicString.c_str(), dataString.c_str());
	/*DEBUGLN_MQTT*/Serial.println("Published to channel " + String(pubChannelID));
}