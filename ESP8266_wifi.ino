// 
// 
// 
//����:
/*
onStationModeGotIP() � ��� ������� ����������, ����� ������� ������������� IP-�����. ������������ IP-������ ����������� DHCP-�������� ��� ��� ������ ������� WiFi.config().
onStationModeDisconnected() � ��� ������� ����������, ����� ������� ����������� �� WiFi-����. ������� ���������� �� �����. ������� ����� �������� � � ��� ������, ���� ���������� ����� ��������� �� ���� ��� ������ ������� WiFi.disconnect() ��-�� ������� WiFi-�������, � � ��� ������, ���� �� ������ �������� ����� �������.
*/

#include <ESP8266WiFi.h>
///#include "ESP8266_wifi.h"

// ��������� ������� � ������
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


#define MAX_CONNECT_TIME 40000//������������ ����� �� �������� ����������� � Wi-fi, � ��

//Define connection parameters
const char* ssid = "HUAWEI-EE5E";//type your ssid
const char* password = "62429292";//type your password
int connectionDelayWiFi = 1;

// ��������� ��� Wi-fi � ������ ����� ������� // 05.01.24 ��������� ������� �������� ����� AP ��� ���������� ����������� � �������. �� ������� ���������� �������� ����������� � ��������������� � ������ �������������.
/*const char* ssidAP = "HCS_Tonshaevo";
const char* passwordAP = NULL;
const IPAddress ipAP = IPAddress(192,168,10,1);*/


//��� ������ �����
//const char* ssid = "DIR-300";
//const char* password = "Hrulia+7";

//const char* ssid = "Redmi Note 11";//type your ssid
//const char* password = "11111122";//type your password

//char ssid[] = "TP-Link_56A9";		// your network SSID (name)
//char password[] = "40555096";				// your network password

//const char* ssid = "TP-Link_56A9";     // �������� ����� WiFi ����
//const char* password = "40555096";// ������ �� ����� WiFi ����


//WiFiClient  wifi_client;

// Connect to WiFi.
void connectWifi(){//serializeJsonPretty(wifinet, Serial);  //������ �������� ������-��
	Serial.println(F("Connecting to Wi-Fi: ") + String(ssid));
	digitalWrite(LED_BUILTIN, HIGH);
	WiFi.mode(WIFI_STA); //�������������, ���� �� ����� ����� ������ �� ���������
	//WiFi.setPhyMode(WIFI_PHY_MODE_11N);

	// Loop until WiFi connection is successful
	while (WiFi.waitForConnectResult(MAX_CONNECT_TIME) != WL_CONNECTED) {
		WiFi.begin(ssid, password);
		//������ ���������� �������� �� ����� ��������
		digitalWrite(LED_BUILTIN, LOW);
		delay(connectionDelayWiFi * 1000);
		digitalWrite(LED_BUILTIN, HIGH);
	}
	Serial.println(F("WiFi.status(3) : ") + String(WiFi.status()));
	Serial.print(F("WiFi connected to "));  //  "������������ � "
	Serial.println(ssid);
	Serial.print(F("Password "));  //  "������ ����� �������"
	Serial.println(password);
	Serial.print(F("IP address: "));  //  "IP-�����: "
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
	WiFi.disconnect(); // ��������� WIFI
	WiFi.softAPdisconnect(true); // ��������� ����� �������(���� ��� ����)
	WiFi.mode(WIFI_OFF); // ��������� WIFI
	delay(500);

	// ������ ����� �� ����� ����� �������
	WiFi.mode(WIFI_AP);
	// ������ ��������� ����
	WiFi.softAPConfig(ipAP, ipAP, IPAddress(255, 255, 255, 0));
	// �������� WIFI � ������ ����� ������� � ������ � �������
	// ���������� � ���������� ssidAP passwordAP
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


// ����� �������
/*
// Connect or reconnect to WiFi
int initWifi() {
//serializeJsonPretty(wifinet, Serial);  //������ �������� ������-��

DEBUGLN_WF(F("\nConnecting to: ")+String(ssid)+F(""));
WiFi.mode(WIFI_STA); //�������������, ���� �� ����� ����� ������ �� ���������
//WiFi.setPhyMode(WIFI_PHY_MODE_11N);
WiFi.begin(ssid, password); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
unsigned long start = millis();

while ((WiFi.status() != WL_CONNECTED) && ((millis() - start)<MAX_CONNECT_TIME)) {//���� ����������
DEBUG_WF(".");
delay(500);
}
if (WiFi.status() != WL_CONNECTED) {
// ���� �� ������� ������������ ��������� � ������ AP
DEBUGLN_WF(F("WiFi up AP. IP 192.168.10.1"));
StartAPMode();
}
else {
DEBUGLN_WF(F("WiFi connected"));
delay(500);
DEBUG_WF(F("Connected to "));  //  "������������ � "
DEBUGLN_WF(ssid);
DEBUG_WF(F("Password "));  //  "������ ����� �������"
DEBUGLN_WF(password);
DEBUG_WF(F("IP address: "));  //  "IP-�����: "
DEBUGLN_WF(WiFi.localIP());
DEBUG_WF(F("RSSI = "));
DEBUGLN_WF(WiFi.RSSI());
//DEBUGLN_WF("RSSI = " + String(WiFi.RSSI()) + "");
	}
	return 0;
} 
*/