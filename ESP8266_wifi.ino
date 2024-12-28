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

// Включение отладки в модуле
//#define DEBUGWF
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

//Define connection parameters
const char* ssid = "HUAWEI-EE5E";//type your ssid
const char* password = "62429292";//type your password
int connectionDelayWiFi = 1;

// Параметры для Wi-fi в режиме точка доступа // 05.01.24 отключена функция поднятия точки AP при отсутствии подключения к роутеру. Но введена постоянная проверка подключения и переподключение в случае необходимости.
/*const char* ssidAP = "HCS_Tonshaevo";
const char* passwordAP = NULL;
const IPAddress ipAP = IPAddress(192,168,10,1);*/


//Для разных сетей
//const char* ssid = "DIR-300";
//const char* password = "Hrulia+7";

//const char* ssid = "Redmi Note 11";//type your ssid
//const char* password = "11111122";//type your password

//char ssid[] = "TP-Link_56A9";		// your network SSID (name)
//char password[] = "40555096";				// your network password

//const char* ssid = "TP-Link_56A9";     // Название Вашей WiFi сети
//const char* password = "40555096";// Пароль от Вашей WiFi сети


//WiFiClient  wifi_client;

// Connect to WiFi.
void connectWifi(){//serializeJsonPretty(wifinet, Serial);  //просто копирнул откуда-то
	Serial.println(F("Connecting to Wi-Fi: ") + String(ssid));
	digitalWrite(LED_BUILTIN, HIGH);
	WiFi.mode(WIFI_STA); //необязательно, если до этого режим работы не изменялся
	//WiFi.setPhyMode(WIFI_PHY_MODE_11N);

	// Loop until WiFi connection is successful
	while (WiFi.waitForConnectResult(MAX_CONNECT_TIME) != WL_CONNECTED) {
		WiFi.begin(ssid, password);
		//мигнем встроенным диодиком во время задержки
		digitalWrite(LED_BUILTIN, LOW);
		delay(connectionDelayWiFi * 1000);
		digitalWrite(LED_BUILTIN, HIGH);
	}
	Serial.println(F("WiFi.status(3) : ") + String(WiFi.status()));
	Serial.print(F("WiFi connected to "));  //  "Подключились к "
	Serial.println(ssid);
	Serial.print(F("Password "));  //  "Пароль точки доступа"
	Serial.println(password);
	Serial.print(F("IP address: "));  //  "IP-адрес: "
	Serial.println(WiFi.localIP());
	Serial.print(F("RSSI = "));
	Serial.println(WiFi.RSSI());
}

// Reconnect to WiFi if it gets disconnected.
void checkWiFiConnect() {
	DEBUGLN_WF(F("WiFi.status(3): ") + String(WiFi.status()));
	if (WiFi.status() != WL_CONNECTED) {
		connectWifi();
	}
}

/*
void StartAPMode() {
	WiFi.disconnect(); // Отключаем WIFI
	WiFi.softAPdisconnect(true); // отключаем отчку доступа(если она была)
	WiFi.mode(WIFI_OFF); // отключаем WIFI
	delay(500);

	// Меняем режим на режим точки доступа
	WiFi.mode(WIFI_AP);
	// Задаем настройки сети
	WiFi.softAPConfig(ipAP, ipAP, IPAddress(255, 255, 255, 0));
	// Включаем WIFI в режиме точки доступа с именем и паролем
	// хронящихся в переменных ssidAP passwordAP
	WiFi.softAP(ssidAP, passwordAP);
}
*/

/*
typedef enum {
WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
WL_IDLE_STATUS      = 0,
WL_NO_SSID_AVAIL    = 1,
WL_SCAN_COMPLETED   = 2,
WL_CONNECTED        = 3,
WL_CONNECT_FAILED   = 4,
WL_CONNECTION_LOST  = 5,
WL_WRONG_PASSWORD   = 6,
WL_DISCONNECTED     = 7
} wl_status_t;
*/


// можно удалить
/*
// Connect or reconnect to WiFi
int initWifi() {
//serializeJsonPretty(wifinet, Serial);  //просто копирнул откуда-то

DEBUGLN_WF(F("\nConnecting to: ")+String(ssid)+F(""));
WiFi.mode(WIFI_STA); //необязательно, если до этого режим работы не изменялся
//WiFi.setPhyMode(WIFI_PHY_MODE_11N);
WiFi.begin(ssid, password); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
unsigned long start = millis();

while ((WiFi.status() != WL_CONNECTED) && ((millis() - start)<MAX_CONNECT_TIME)) {//ждем соединения
DEBUG_WF(".");
delay(500);
}
if (WiFi.status() != WL_CONNECTED) {
// Если не удалось подключиться запускаем в режиме AP
DEBUGLN_WF(F("WiFi up AP. IP 192.168.10.1"));
StartAPMode();
}
else {
DEBUGLN_WF(F("WiFi connected"));
delay(500);
DEBUG_WF(F("Connected to "));  //  "Подключились к "
DEBUGLN_WF(ssid);
DEBUG_WF(F("Password "));  //  "Пароль точки доступа"
DEBUGLN_WF(password);
DEBUG_WF(F("IP address: "));  //  "IP-адрес: "
DEBUGLN_WF(WiFi.localIP());
DEBUG_WF(F("RSSI = "));
DEBUGLN_WF(WiFi.RSSI());
//DEBUGLN_WF("RSSI = " + String(WiFi.RSSI()) + "");
	}
	return 0;
} 
*/