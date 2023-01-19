/*
 Name:		ESP_01S_HCS_TA.ino
 Created:	02.01.2021 11:23:52
 Author:	Sergey
*/


#include <PubSubClient.h>
//#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

//���������� ��� ����� .h � .cpp
#include "myCycle.h"


//��������� ������� � �������� ������ ���������
//#define DEBUG_MAIN

#ifdef DEBUG_MAIN
	#define DEBUG(x) (Serial.print(x))
	#define DEBUGLN(x) (Serial.println(x))
	#define DEBUGR(x,r) (Serial.print(x,r))
#else
	#define DEBUG(x) 
	#define DEBUGLN(x)
	#define DEBUGR(x,r) 
#endif // DEBUG_MAIN


#define MAX_SERIAL_REQ  50		//
#define NUMBER_OF_DS18B20 18	//Number of sensor DS18B20 (16) + ������ ����������� ���� (1) + ������� ������� ����������� (1)
#define MEGA_SERIAL Serial			//Serial port for communication with MEGA

//������������ ��� ���������� ������ �������
enum sysParam{ ERR, ON, OFF, AUTO, OPEN, CLOSE, MYALG, PID};
String sysParamString[8] = { "Err","On", "Off", "Auto", "Open", "Close", "myAlg", "PID" }; //���������� ����� ��������� sysParam

///time_t SystemTime = 0; //esp system time

/* ��������� ����� */
//#define PIN_RESET_MEGA D1 //��� ��������� ������� ������ ��� ������ MEGA




//global variables
float temperatures[(NUMBER_OF_DS18B20)];	//������ ���������� �������� DS18B20 
int SysParametrs[6];											//arrey system (in MEGA) parameters and variables
/* 
[0] - BoilerPumpMode				//1 - on, 2 - off, 3 - auto
[1] - SystemPumpMode				//1 - on, 2 - off, 3 - auto
[2] - SysTempControlMode		//6 � ��� �������� �������������, 7 - PID ���������
[3] - DoorAirMode						//4 - open, 5 - close, 3 - auto
[4] - reserved
[5] - reserved

*/

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


/* Initial Timers */
myCycle cycleRequestTemperature(MS_01M, true);			// 2�, 1�, 5�(mega ��������, ������ �� ������ �������� � ������������ ������) 30 ������ �����������
myCycle cycleRequestSystemParameters(MS_10S, true);  // 3�, 30c, ������ ���������� ������ �������.
//myCycle cycleRequestTargetTemperature(120000, true);			// 2� ������ ������� ������� ����������� � ������ ��������� ����������
myCycle cycleSendingDataToThingSpeak(MS_05M, true);	// 5� ���� �������� ������ � ����������� �� ���� IoT
myCycle cycleCheckMegaAndESP(MS_03M, true);				//3��� ���� �������� � mega ����� serial ������� ������ �����������: ?esp=1
myCycle cycleMegaTimeSynchronization(MS_30M, true);				// 1��� ���� �������� ������� ������������� ������� � ����.


/*****************************************************/
void setup() {
	//Serial.setRxBufferSize(500); // �� ��������� � ESP 256 ����
	Serial.begin(9600);
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
	initWebServer(); //
	initNTP();

	//����������� ����� ��� ������ ������ MEGA
	//pinMode(PIN_RESET_MEGA, OUTPUT);
	//digitalWrite(PIN_RESET_MEGA, HIGH);

	//�������� �������� �� �����, ��� �� ������, ��� ����� �� �������
	pinMode(LED_BUILTIN, OUTPUT);
	
	//��������� �������� ������� �������� ��������� ������������� ������ Mega
	byte mega = MEGA_OFF;
	megaTimer = millis();
}
unsigned long timeBlink = millis();

/*********************************************/
void loop() {
	// Web-server listen for HTTP requests from clients
	checkWebClient();

	workClock(); //������ NTP ������

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

	//������ ���������� ������ ������� ���������
	if (cycleRequestSystemParameters.check()) {
		RequestSystemParameters(); //����� ���������� ������ �������
		cycleRequestSystemParameters.reStart();
	}

	//�������� ������� ������������� ������� � ����	
	if (cycleMegaTimeSynchronization.check()) {
		SendActualTime(); //�������� � Mega ������� �������
		cycleMegaTimeSynchronization.reStart();
	}
}


/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/

//������ ���������� � ����������� � ������ MEGA
void RequestTemperature() {
	//������ � ���� �� �������� ���� �������� ����������� 
	Serial.println(F("?reqesttemp=A"));
}

////������ ���������� � ������� ������� ����������� � ������ ���������� � ������ MEGA
//void RequestTargetTemperature() {
//	Serial.println("?reqestTargetTemp");
//}

//�������� ������ ������ mega � �������� ������� ������ ESP ����������� 
void checkMegaAndESP() {
	//�������� ������ MEGA ���������� � ����� ���������� ���������������� 
	MEGA_SERIAL.println(F("?esp=1"));

	//��������� ��� ����� �� ������ mega �� ��������� ���������� � ��� �����������. �� ���� ���� � 3 ������
	if ((millis() - megaTimer) > 180000UL) {
		mega = MEGA_OFF; 
		megaTimer = millis();
		//������ reset MEGA
		DEBUGLN("Module MEGA reseting");
		//digitalWrite(PIN_RESET_MEGA, LOW);
		//delay(300);
		//digitalWrite(PIN_RESET_MEGA, HIGH);
	}
}	// che�kMegaAndESP() 

//������ � ���� �� �������� �������� ���������� ���������� 
void RequestSystemParameters() {
	MEGA_SERIAL.println(F("?getSystemParameters"));
}