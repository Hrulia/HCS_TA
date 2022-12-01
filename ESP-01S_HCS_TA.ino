/*
 Name:		ESP_01S_HCS_TA.ino
 Created:	02.01.2021 11:23:52
 Author:	Sergey
*/
//��������������
/*��������������:
���������� �������: 3.3V (2.5-3.6V)
��� �����������: 300 �� ��� ������� � �������� ������, 35 �� �� ����� ������, 80 �� � ������ ����� �������
������������ ��� ���� � 12 ��.
Flash ������ (������ ���������): 1 ��
Flash ������ (�������� ���������): 1-16 �� � ����������� �� �����������
EEPROM ������: �� 4 ��
SRAM ������: 82 ��
������� ����: 80/160 ���
GPIO: 11 �����
���: 10 �����
����������: 10 �����
���: 1 ���
I2C: 1 ����� (�����������, ���� ����� ��������� �����)
I2S: 1 �����
SPI: 1 �����
UART: 1.5 �����
WiFi �����
*/
/*
��� ������ ����������� ����� ��� ���� ������ ������ �� �������� ������, ��������� � � ���� ������ https://rabbithole.wwwdotorg.org/2017/03/28/esp8266-gpio.html. ������������� ����������� ������ �������� D1 (GPIO5) � D2 (GPIO4). ���� ���������� ��������� �������� ������-�� ��������� (����, ����������, ��� �������� �������� ��� ������� ����������), �� ����� ������������ ������ ��� ����!
�� ���� �� ����� ����� I2C, �� ���� ����� ������������� �� ����� ������ ���� ����� Wire.begin(sda, scl).
*/
/* info:
https://alexgyver.ru/lessons/esp8266/
https://wikihandbk.com/wiki/ESP8266:��������/Arduino/����������/����������_ESP8266WiFi/�����_�������
https://kit.alexgyver.ru/tutorials-category/telegram/ //��������� ���
https://wikihandbk.com/wiki/ESP8266:������/�����_HUZZAH_ESP8266_��_Adafruit //��������� ������� ��������� ��� �������
���������� ����������� ������ � ������� ������ Generic: https://wikihandbk.com/wiki/ESP8266:��������/Arduino/����������/����������_ESP8266WiFi/�����_Generic/����������_�����������_������_�_�������_������_Generic
EasyIoT The easy way to build Internet of Things: https://iot-playground.com/
Serial.setDebugOutput(true);  //��������� ������ ��������������� ����������
WiFi.printDiag(Serial);       //����� ����������� � ������

*/

//=====================================================
// ��� ������ �� UART � ������� MEGA �� ������ ������ ���� ������������ ��� �� Serial1, ������� ���� �� USB.
// �� � esp8266 ���� ��� ���� UART(���������� ��� ������ ����� ���� �� uart �� ������ ����� ��), ��� ������ ����� � ��� �����, ������� ����� ������������� � ������ �� ������ 
// �������� Serial.swap() � ��� ����, �� ������ ������, ��� ������ ����� �������� Serial.flush() 
// ��������� ���: https://esp8266.ru/forum/threads/zachem-polzovatsja-kostylem-softserial-kogda-u-esp8266-dva-apparatnyx-uart.4749/
//

//#include <ESP8266WebServerSecure.h>
//#include <ESP8266WebServer-impl.h>
//#include "HCS_server.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

//���������� ��� ����� .h � .cpp
#include "myCycle.h"
#include "ESP8266_wifi.h"

//��������� ������� � �������� ������ ���������
#define DEBUG_MAIN

#ifdef DEBUG_MAIN
	#define DEBUG(x) (Serial.print(x))
	#define DEBUGLN(x) (Serial.println(x))
	#define DEBUGR(x,r) (Serial.print(x,r))
#else
	#define DEBUG(x) 
	#define DEBUGLN(x)
	#define DEBUGR(x,r) 
#endif // DEBUG_MAIN


/* Work with UART */
//#define MESS(mess) (Serial.print("?esplog="+String(mess))) // ������a ����������� ������� mega ��������� �� esp
//////������ ���������, ����� ������� ���������� ������� ������ MEGA
////#define UART_TO_MEGALN(x) (Serial.println(x))
#define MAX_SERIAL_REQ  50 //
#define NUMBER_OF_DS18B20 18 //Number of sensor DS18B20 (16) + ������ ����������� ���� (1) + ������� ������� ����������� (1)



/* ��������� ����� */
//#define PIN_RESET_MEGA D1 //��� ��������� ������� ������ ��� ������ MEGA

/* Initial Timers */
myCycle cycleRequestTemperature(MS_05S, true);			// 1�, 5� ������ �����������
//myCycle cycleRequestTargetTemperature(120000, true);			// 2� ������ ������� ������� ����������� � ������ ��������� ����������
myCycle cycleSendingDataToThingSpeak(MS_05M, true);	// 5� ���� �������� ������ � ����������� �� ���� IoT
myCycle cycleCheckMegaAndESP(MS_03M, true);				//3��� ���� �������� � mega ����� serial ������� ������ �����������: ?esp=1

//������ ���������� �������� DS18B20 
float temperatures[(NUMBER_OF_DS18B20)]; 

/* Watch dog for Mega */
// mega partner. ����� ��������� ������������� ������ Mega
#define MEGA_OFF 0
#define MEGA_ON  1
byte mega = MEGA_OFF;
unsigned long megaTimer = millis(); //������ �������� ��������� ������������� ������ Mega


/* ��������� MQTT ������� */
extern long writeChannelID;				//ID ������ ThingSpeak ��� ������ ����� MQTT ������
extern int fieldsToPublish[8];    // Change to allow multiple fields.
extern float dataToPublish[8];    // Holds your field data.


/*****************************************************/
void setup() {
	//Serial.setRxBufferSize(500); // �� ��������� � ESP 256 ����
	Serial.begin(115200);
	//Serial.swap(); // GPIO15/D8 (TX) � GPIO13/D7 (RX)
	//Serial.setTimeout(250);

	// ���������� � �����������
	Serial.println("");
	Serial.println("ESP8266 board info:");
	Serial.print("\tChip ID: ");
	Serial.println(ESP.getFlashChipId());
	Serial.print("\tCore Version: ");
	Serial.println(ESP.getCoreVersion());
	Serial.print("\tChip Real Size: ");
	Serial.println(ESP.getFlashChipRealSize());
	Serial.print("\tChip Flash Size: ");
	Serial.println(ESP.getFlashChipSize());
	Serial.print("\tChip Flash Speed: ");
	Serial.println(ESP.getFlashChipSpeed());
	Serial.print("\tChip Speed: ");
	Serial.println(ESP.getCpuFreqMHz());
	Serial.print("\tChip Mode: ");
	Serial.println(ESP.getFlashChipMode());
	Serial.print("\tSketch Size: ");
	Serial.println(ESP.getSketchSize());
	Serial.print("\tSketch Free Space: ");
	Serial.println(ESP.getFreeSketchSpace());

	WiFi_init();
	initThingSpeak();//������ �������� ������ �� ������ ThingSpeak
	initMQTT();
	initWebServer();

	//����������� ����� ��� ������ ������ MEGA
	//pinMode(PIN_RESET_MEGA, OUTPUT);
	//digitalWrite(PIN_RESET_MEGA, HIGH);

	//�������� �������� �� �����, ��� �� ������, ��� ����� �� �������
	//pinMode(LED_BUILTIN, OUTPUT);
	
	//��������� �������� ������� �������� ��������� ������������� ������ Mega
	byte mega = MEGA_OFF;
	megaTimer = millis(); 
}
unsigned long timeBlink = millis();

/*********************************************/
void loop() {
	// Web-server listen for HTTP requests from clients
	checkWebClient();

	//�������� ����������� ������ �� ���� Serial �� ������ Mega
	checkSerial(); //��������� ��� ����� ����

	//����������� ����� � �������� �� ������, �������� ����������� ����� ��������� �� MQTT �������
	MQTTloop();

	//������ ������ � ���� ������������ �� ������ MEGA
	if (cycleRequestTemperature.check()) {
		RequestTemperature();
		cycleRequestTemperature.reStart();
	}
									////������ ������ � ������� ������� ����������� � ������ ����������
									//if (cycleRequestTargetTemperature.check()) {
									//	RequestTargetTemperature();
									//	// ���������� ������� ������ �������.
									//	cycleRequestTargetTemperature.clear();
									//	cycleRequestTargetTemperature.reStart();
									//}

	//�������� ������ � ����������� �� ���� ThingSpeak (�� ���� ���� � 15 ������)
	if (cycleSendingDataToThingSpeak.check()) {
		ThingSpeakWriteItems(temperatures);
		cycleSendingDataToThingSpeak.reStart();
	}

	// �������� ������� ������ ����������� �� Mega ����� Serial
	if (cycleCheckMegaAndESP.check()) {
		checkMegaAndESP();  		//�������� ����������� ���������������� ������ MEGA.
		cycleCheckMegaAndESP.reStart();
	}
}



/*******************************************************************************************/

//������ ���������� � ����������� � ������ MEGA
void RequestTemperature() {
	//������ � ���� �� �������� ���� �������� ����������� 
	Serial.println("?reqesttemp=A");
}

////������ ���������� � ������� ������� ����������� � ������ ���������� � ������ MEGA
//void RequestTargetTemperature() {
//	Serial.println("?reqestTargetTemp");
//}

//�������� ������ ������ mega � �������� ������� ������ ESP ����������� 
void checkMegaAndESP() {
	//�������� ������ MEGA ���������� � ����� ���������� ���������������� 
	Serial.println(F("?esp=1"));

	//��������� ��� ����� �� ������ mega �� ��������� ���������� � ��� �����������. �� ���� ���� � 3 ������
	if ((millis() - megaTimer) > 180000) {
		mega = MEGA_OFF; 
		megaTimer = millis();
		//������ reset MEGA
		DEBUGLN("Module MEGA reseting");
		//digitalWrite(PIN_RESET_MEGA, LOW);
		//delay(300);
		//digitalWrite(PIN_RESET_MEGA, HIGH);
	}
}	// che�kMegaAndESP() 