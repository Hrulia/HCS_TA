// 
// 
// 
//инфо:
/*
onStationModeGotIP() – эта функция вызывается, когда станции присваивается IP-адрес. Присваивание IP-адреса выполняется DHCP-клиентом или при помощи функции WiFi.config().
onStationModeDisconnected() – эта функция вызывается, когда станция отключается от WiFi-сети. Причина отключения не важна. Событие будет запущено и в том случае, если отключение будет выполнено из кода при помощи функции WiFi.disconnect() из-за слабого WiFi-сигнала, и в том случае, если мы просто отключим точка доступа.
*/

//#include <ESP8266WiFi.h>
#include "ESP8266_wifi.h"

#define DEBUGWF
#ifdef DEBUGWF
	#define DEBUG_WF(x) (Serial.print(x))
	#define DEBUGLN_WF(x) (Serial.println(x))
	#define DEBUGR_WF(x,r) (Serial.print(x,r))
#else
	#define DEBUG_WF(x) 
	#define DEBUGLN_WF(x)
	#define DEBUGR_WF(x,r) 
#endif 


#define MAX_CONNECT_TIME 40000//Максимальное время на ожидание подключения к Wi-fi, в мс

//const char* ssid = "DIR-300";
//const char* password = "Hrulia+7";

const char* ssid = "HUAWEI-EE5E";//type your ssid
const char* password = "62429292";//type your password

//const char* ssid = "MSN";//type your ssid
//const char* password = "22222233";//type your password

//char ssid[] = "TP-Link_56A9";		// your network SSID (name)
//char password[] = "40555096";				// your network password

//const char* ssid = "TP-Link_56A9";     // Название Вашей WiFi сети
//const char* password = "40555096";// Пароль от Вашей WiFi сети

//int keyIndex = 0;							// your network key index number (needed only for WEP)

//WiFiClient  wifi_client;

	// Connect or reconnect to WiFi
int WiFi_init() {
	WiFi.mode(WIFI_STA); //необязательно, если до этого режим работы не изменялся
	WiFi.begin(ssid, password); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
	unsigned long start = millis();
	if (WiFi.status() != WL_CONNECTED) {
		DEBUGLN_WF("\nConnecting to: "+String(ssid)+"");
		while ((WiFi.status() != WL_CONNECTED) && ((millis() - start)<MAX_CONNECT_TIME)) {
			DEBUG_WF(".");
				delay(500);
		}
		DEBUGLN_WF();
		#ifdef DEBUG_WF
			if (WiFi.status() == WL_CONNECTED) {
				Serial.println("");
				Serial.print("Connected to ");  //  "Подключились к "
				Serial.println(ssid); 
				Serial.print("Password  ");  //  "Пароль точки доступа"
				Serial.println(password);
				Serial.print("IP address: ");  //  "IP-адрес: "
				Serial.println(WiFi.localIP());
				DEBUGLN_WF("RSSI = " + String(WiFi.RSSI())+"");
			}
			else {
				DEBUGLN_WF("Not connected! Wi-fi status: "+String(WiFi.status()));
				/*DEBUGLN_WF(WiFi.status());*/
			}
		#endif
	}
	return 0;
} //end WiFi_init

