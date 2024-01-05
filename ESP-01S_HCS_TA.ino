/*
 Name:		ESP_01S_HCS_TA.ino
 Created:	02.01.2021 11:23:52
 Author:	Sergey
*/

/*//!!!! ��� ����� ��������� ����� cpp � h � ������  https://arduinoprosto.ru/q/61634/neskolko-faylov-ino-v-odnom-eskize

� *.h - �����:
	extern byte Test;

� *.cpp - �����:
	byte Test = 0;

	��, ������ ���������� Test �������� �����, ��� ��������� ��������������� *.h - ����.*/


#include <PubSubClient.h>
//#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

//���������� ��� ����� .h � .cpp
#include "myCycle.h"

/* ���������� ������� ��� � ������ �������*/

//��������� ������� � �������� ������ ���������
#define DEBUG_ENABLE_MAIN

#ifdef DEBUG_ENABLE_MAIN
	#define DEBUG_PRINT_MAIN(x) (Serial.print(x))
	#define DEBUG_PRINTLN_MAIN(x) (Serial.println(x))
	#define DEBUGR_PRINTR_MAIN(x,r) (Serial.print(x,r))
#else
	#define DEBUG_PRINT_MAIN(x) 
	#define DEBUG_PRINTLN_MAIN(x) 
	#define DEBUGR_PRINTR_MAIN(x,r) 
#endif // DEBUG_ENABLE_MAIN


#define MAX_SERIAL_REQ  50		//
#define NUMBER_OF_DS18B20 18	//Number of sensor DS18B20 (16) + ������ ����������� ���� (1) + ������� ������� ����������� (1)
#define SERIAL_TO_MEGA Serial			//Serial port for communication with MEGA

//Enumeration - ��������� ������ �������
enum sysParam{ ERR, ON, OFF, AUTO, OPEN, CLOSE, MYALG, PID};
String sysParamString[8] = { "Err","On", "Off", "Auto", "Open", "Close", "myAlg", "PID" }; //���������� ����� ��������� sysParam

///time_t SystemTime = 0; //esp system time ///������, ����� ���� ������ � ������� ntp

/* ��������� ����� */
//#define PIN_RESET_MEGA D1 //��� ��������� ������� ������ ��� ������ MEGA

//global variables
float temperatures[(NUMBER_OF_DS18B20)];	//������ ���������� �������� DS18B20 
int SysParametrs[6];											// system (in MEGA) parameters and variables array
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
myCycle cycleRequestTemperature(MS_01M, true);					// 2�, 1�, 5�(mega ��������, ������ �� ������ �������� � ������������ ������) 30 ������ �����������
myCycle cycleRequestSystemParameters(MS_20S, true);			// 3�, 30c, ������ ���������� ������ �������.
//myCycle cycleRequestTargetTemperature(120000, true);	// 2� ������ ������� ������� ����������� � ������ ��������� ����������
myCycle cycleSendingDataToThingSpeak(MS_05M, true);			// 5� ���� �������� ������ � ����������� �� ���� ThingSpeak. �����������: �� ���� ���� � 15 ������.
myCycle cycleCheckMegaAndESP(MS_03M, true);							//3��� ���� �������� � mega ����� serial ������� ������ �����������: ?esp=1
myCycle cycleMegaTimeSynchronization(MS_30M, true,true);			// 1��� ���� �������� ������� ������������� ������� � ����.


/*****************************************************/
void setup() {
	//Serial.setRxBufferSize(500); // �� ��������� � ESP 256 ����
	Serial.begin(9600);
	DEBUG_PRINTLN_MAIN(F("\n*******   Start setup()   *******"));
	//Serial.swap(); // GPIO15/D8 (TX) � GPIO13/D7 (RX)
	//Serial.setTimeout(250);
	 

	// ����� ���������� � �����������
	DEBUG_PRINTLN_MAIN("");
	DEBUG_PRINTLN_MAIN("ESP8266 board info:");
	DEBUG_PRINT_MAIN("\tChip ID: ");
	DEBUG_PRINTLN_MAIN(ESP.getFlashChipId());
	DEBUG_PRINT_MAIN("\tCore Version: ");
	DEBUG_PRINTLN_MAIN(ESP.getCoreVersion());
	DEBUG_PRINT_MAIN("\tChip Real Size: ");
	DEBUG_PRINTLN_MAIN(ESP.getFlashChipRealSize());
	DEBUG_PRINT_MAIN("\tChip Flash Size: ");
	DEBUG_PRINTLN_MAIN(ESP.getFlashChipSize());
	DEBUG_PRINT_MAIN("\tChip Flash Speed: ");
	DEBUG_PRINTLN_MAIN(ESP.getFlashChipSpeed());
	DEBUG_PRINT_MAIN("\tChip Speed: ");
	DEBUG_PRINTLN_MAIN(ESP.getCpuFreqMHz());
	DEBUG_PRINT_MAIN("\tChip Mode: ");
	DEBUG_PRINTLN_MAIN(ESP.getFlashChipMode());
	DEBUG_PRINT_MAIN("\tSketch Size: ");
	DEBUG_PRINTLN_MAIN(ESP.getSketchSize());
	DEBUG_PRINT_MAIN("\tSketch Free Space: ");
	DEBUG_PRINTLN_MAIN(ESP.getFreeSketchSpace());
	//����� ���������� ������� ESP
	//ADC_MODE (ADC_VCC); //������������� ��� ��� �������
	//ESP.getVcc() //����� �������������� ��� ��������� ���������� �������
	////� ���� ������ ����� TOUT ������ ���� ��������.
	////�� ��������� ��� �������� �� ������ � ������� TOUT pin analogRead(A0)� ESP.getVCC()����������.

	//Pins configuration
	//����������� ����� ��� ������ ������ MEGA
	//pinMode(PIN_RESET_MEGA, OUTPUT);
	//digitalWrite(PIN_RESET_MEGA, HIGH);

	//��� ����������� ���������� �� ����� ESP8266
	pinMode(LED_BUILTIN, OUTPUT);


	//������������� �������
	DEBUG_PRINTLN_MAIN(F("call connectWifi()"));
	connectWifi();

	DEBUG_PRINTLN_MAIN(F("call initThingSpeak()"));
	initThingSpeak();

	DEBUG_PRINTLN_MAIN(F("call initMQTT()"));
	initMQTT();

	DEBUG_PRINTLN_MAIN(F("call initWebServer()"));
	initWebServer(); 

	DEBUG_PRINTLN_MAIN(F("call initNTP()"));
	initNTP();

} //end setup


void loop() {
	DEBUG_PRINTLN_MAIN(F("*******   Start loop()   *******"));
	
	// check wifi connection
	DEBUG_PRINTLN_MAIN(F("call checkWiFiConnect()"));
	checkWiFiConnect();

	// Web-server listen for HTTP requests from clients
	DEBUG_PRINTLN_MAIN(F("call checkWebClient()"));
	checkWebClient();

	// ������ NTP ������. ���������� ������� c ������� � �������������� �� ������ ����������� �������.
	DEBUG_PRINTLN_MAIN(F("call ntpClockWork()"));
	ntpClockWork(); 

	//�������� ����������� ������ �� ���� Serial �� ������ Mega
	DEBUG_PRINTLN_MAIN(F("call checkSerial()"));
	checkSerial(); //��������� ��� ����� ����

	//����������� ����� � �������� �� ������, �������� ����������� ����� ��������� �� MQTT �������
	DEBUG_PRINTLN_MAIN(F("call MQTTloop()"));
	MQTTloop();


/*************************************************/
/*   ��������� ���������� �� �������� ��������   */
/*************************************************/

	//�������� � Mega2560 ������� �������
	DEBUG_PRINTLN_MAIN(F("call SendActualTime()"));
	SendActualTime();
	
	//������ ������ � ���� ������������ �� ������ MEGA
	if (cycleRequestTemperature.check()) {
		DEBUG_PRINTLN_MAIN(F("call RequestTemperature()"));
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
		DEBUG_PRINTLN_MAIN(F("call ThingSpeakWriteItems()"));
		ThingSpeakWriteItems(temperatures);
		cycleSendingDataToThingSpeak.reStart();
	}

	// �������� ������� ������ ����������� �� Mega ����� Serial
	if (cycleCheckMegaAndESP.check()) {
		DEBUG_PRINTLN_MAIN(F("call checkMegaAndESP()"));
		checkMegaAndESP();  		//�������� ����������� ���������������� ������ MEGA.
		cycleCheckMegaAndESP.reStart();
	}

	//������ ���������� ������ ������� ���������
	if (cycleRequestSystemParameters.check()) {
		DEBUG_PRINTLN_MAIN(F("call RequestSystemParameters()"));
		RequestSystemParameters(); //����� ���������� ������ �������
		cycleRequestSystemParameters.reStart();
	}

	//������ ���������� �����������, ���������������� �� ��������� ����� loop � ������, ��� ����� �� �������
	//digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); 
	DEBUG_PRINTLN_MAIN(F("call blinkBuiltInLed()"));
	blinkBuiltInLed();

	DEBUG_PRINTLN_MAIN(F("******* End loop()"));
} //end loop()


/*******************************************************************************************/
/*******************************   �������   ***********************************************/
/*******************************************************************************************/

//������ ���������� � ����������� � ������ MEGA
void RequestTemperature() {
	//������ � ���� �� �������� ���� �������� ����������� 
	SERIAL_TO_MEGA.println(F("?reqesttemp=A"));
}

////������ ���������� � ������� ������� ����������� � ������ ���������� � ������ MEGA
//void RequestTargetTemperature() {
//	Serial.println("?reqestTargetTemp");
//}

//�������� ������ ������ mega � �������� ������� ������ ESP ����������� 
void checkMegaAndESP() {
	//�������� ������ MEGA ���������� � ����� ���������� ���������������� 
	SERIAL_TO_MEGA.println(F("?esp=1"));

	//��������� ��� ����� �� ������ mega �� ��������� ���������� � ��� �����������. �� ���� ���� � 3 ������
	if ((millis() - megaTimer) > 180000UL) {
		mega = MEGA_OFF; 
		megaTimer = millis();
		//������ reset MEGA
		DEBUG_PRINTLN_MAIN(F("Module MEGA reseting"));
		//digitalWrite(PIN_RESET_MEGA, LOW);
		//delay(300);
		//digitalWrite(PIN_RESET_MEGA, HIGH);
	}
}	// che�kMegaAndESP() 

//������ � ���� �� �������� �������� ���������� ���������� 
void RequestSystemParameters() {
	SERIAL_TO_MEGA.println(F("?getSystemParameters"));
}

/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/

void blinkBuiltInLed() {
	static bool flag;
	digitalWrite(LED_BUILTIN, flag);
	flag = !flag;
}
