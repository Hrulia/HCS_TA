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


#define MAX_CONNECT_TIME 40000//������������ ����� �� �������� ����������� � Wi-fi, � ��

//const char* ssid = "DIR-300";
//const char* password = "Hrulia+7";

const char* ssid = "HUAWEI-EE5E";//type your ssid
const char* password = "62429292";//type your password

//const char* ssid = "MSN";//type your ssid
//const char* password = "22222233";//type your password

//char ssid[] = "TP-Link_56A9";		// your network SSID (name)
//char password[] = "40555096";				// your network password

//const char* ssid = "TP-Link_56A9";     // �������� ����� WiFi ����
//const char* password = "40555096";// ������ �� ����� WiFi ����

// ��������� ��� Wi-fi � ������ ����� ������� 
const char* ssidAP = "HCS_Tonshaevo";
const char* passwordAP = NULL;
const IPAddress apIP = IPAddress(192,168,10,1);



//WiFiClient  wifi_client;

	// Connect or reconnect to WiFi
int WiFi_init() {
	//serializeJsonPretty(wifinet, Serial);  //������ �������� ������-��

	DEBUGLN_WF("\nConnecting to: "+String(ssid)+"");
	WiFi.mode(WIFI_STA); //�������������, ���� �� ����� ����� ������ �� ���������
	//WiFi.setPhyMode(WIFI_PHY_MODE_11N);
	WiFi.begin(ssid, password); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
	unsigned long start = millis();

	while ((WiFi.status() != WL_CONNECTED) && ((millis() - start)<MAX_CONNECT_TIME)) {//���� ����������
		DEBUG_WF(".");
			delay(500);
	}
	if (WiFi.status() != WL_CONNECTED) { // ���� �� ������� ������������ ��������� � ������ AP
		DEBUGLN_WF("WiFi up AP. IP 192.168.10.1");
		StartAPMode();
	}
	else {
		DEBUGLN_WF("WiFi connected");
		delay(500);
		DEBUG_WF("Connected to ");  //  "������������ � "
		DEBUGLN_WF(ssid);
		DEBUG_WF("Password  ");  //  "������ ����� �������"
		DEBUGLN_WF(password);
		DEBUG_WF("IP address: ");  //  "IP-�����: "
		DEBUGLN_WF(WiFi.localIP());
		DEBUG_WF("RSSI = ");
		DEBUGLN_WF(WiFi.RSSI());
		/*DEBUGLN_WF("RSSI = " + String(WiFi.RSSI()) + "");*/
	}
	return 0;
} 

void StartAPMode() {
	WiFi.disconnect(); // ��������� WIFI
	WiFi.softAPdisconnect(); // ��������� ����� �������(���� ��� ����)
	WiFi.mode(WIFI_OFF); // ��������� WIFI
	delay(500);

	// ������ ����� �� ����� ����� �������
	WiFi.mode(WIFI_AP);
	// ������ ��������� ����
	WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
	// �������� WIFI � ������ ����� ������� � ������ � �������
	// ���������� � ���������� ssidAP passwordAP
	WiFi.softAP(ssidAP, passwordAP);
}