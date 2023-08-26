// 
// 
// 
//инфо:
/*
onStationModeGotIP() – эта функция вызывается, когда станции присваивается IP-адрес. Присваивание IP-адреса выполняется DHCP-клиентом или при помощи функции WiFi.config().
onStationModeDisconnected() – эта функция вызывается, когда станция отключается от WiFi-сети. Причина отключения не важна. Событие будет запущено и в том случае, если отключение будет выполнено из кода при помощи функции WiFi.disconnect() из-за слабого WiFi-сигнала, и в том случае, если мы просто отключим точка доступа.
*/

#include <ESP8266WiFi.h>
///#include "ESP8266_wifi.h"

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

// Параметры для Wi-fi в режиме точка доступа 
const char* ssidAP = "HCS_Tonshaevo";
const char* passwordAP = NULL;
const IPAddress apIP = IPAddress(192,168,10,1);



//WiFiClient  wifi_client;

	// Connect or reconnect to WiFi
int WiFi_init() {
	//serializeJsonPretty(wifinet, Serial);  //просто копирнул откуда-то

	DEBUGLN_WF("\nConnecting to: "+String(ssid)+"");
	WiFi.mode(WIFI_STA); //необязательно, если до этого режим работы не изменялся
	//WiFi.setPhyMode(WIFI_PHY_MODE_11N);
	WiFi.begin(ssid, password); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
	unsigned long start = millis();

	while ((WiFi.status() != WL_CONNECTED) && ((millis() - start)<MAX_CONNECT_TIME)) {//ждем соединения
		DEBUG_WF(".");
			delay(500);
	}
	if (WiFi.status() != WL_CONNECTED) { // Если не удалось подключиться запускаем в режиме AP
		DEBUGLN_WF("WiFi up AP. IP 192.168.10.1");
		StartAPMode();
	}
	else {
		DEBUGLN_WF("WiFi connected");
		delay(500);
		DEBUG_WF("Connected to ");  //  "Подключились к "
		DEBUGLN_WF(ssid);
		DEBUG_WF("Password  ");  //  "Пароль точки доступа"
		DEBUGLN_WF(password);
		DEBUG_WF("IP address: ");  //  "IP-адрес: "
		DEBUGLN_WF(WiFi.localIP());
		DEBUG_WF("RSSI = ");
		DEBUGLN_WF(WiFi.RSSI());
		/*DEBUGLN_WF("RSSI = " + String(WiFi.RSSI()) + "");*/
	}
	return 0;
} 

void StartAPMode() {
	WiFi.disconnect(); // Отключаем WIFI
	WiFi.softAPdisconnect(); // отключаем отчку доступа(если она была)
	WiFi.mode(WIFI_OFF); // отключаем WIFI
	delay(500);

	// Меняем режим на режим точки доступа
	WiFi.mode(WIFI_AP);
	// Задаем настройки сети
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	// Включаем WIFI в режиме точки доступа с именем и паролем
	// хронящихся в переменных ssidAP passwordAP
	WiFi.softAP(ssidAP, passwordAP);
}