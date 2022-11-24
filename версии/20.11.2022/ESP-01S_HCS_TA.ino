/*
 Name:		ESP_01S_HCS_TA.ino
 Created:	02.01.2021 11:23:52
 Author:	Sergey
*/

//=====================================================
// ��� ������ �� UART � ������� MEGA �� ������ ������ ���� ������������ ����� Serial1, ������� ���� �� USB.
// �� � esp8266 ���� ��� ���� UART, ��� ������ ����� � ��� �����, ������� ����� ������������� � ������ �� ������ 
// �������� Serial.swap() � ��� ����, �� ������ ������, ��� ������ ����� �������� Serial.flush() 
// ��������� ���: https://esp8266.ru/forum/threads/zachem-polzovatsja-kostylem-softserial-kogda-u-esp8266-dva-apparatnyx-uart.4749/
//

#include <PubSubClient.h>
#include <ESP8266WiFi.h>  

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

//#define MESS(mess) (Serial.print("?esplog="+String(mess))) // ������a ����������� ������� mega ��������� �� esp

//////������ ���������, ����� ������� ���������� ������� ������ MEGA
////#define UART_TO_MEGALN(x) (Serial.println(x))

#define MAX_SERIAL_REQ  50 //
#define NUMBER_OF_DS18B20 18 //Number of sensor DS18B20 (16) + ������ ����������� ���� (1) + ������� ������� ����������� (1)

// mega partner. ����� ��������� ������������� ������ Mega
#define MEGA_OFF 0
#define MEGA_ON  1

//��������� �����
//#define PIN_RESET_MEGA D1 //��� ��������� ������� ������ ��� ������ MEGA

myCycle cycleRequestTemperature(MS_01M, true);			// 1� ������ �����������
//myCycle cycleRequestTargetTemperature(120000, true);			// 2� ������ ������� ������� ����������� � ������ ��������� ����������
myCycle cycleSendingDataToThingSpeak(MS_05M, true);	// 5� ���� �������� ������ � ����������� �� ���� IoT
myCycle cycleCheckMegaAndESP(MS_03M, true);				//3��� ���� �������� � mega ����� serial ������� ������ �����������: ?esp=1

float temperatures[(NUMBER_OF_DS18B20)]; //������ ���������� �������� DS18B20 

byte mega = MEGA_OFF;
unsigned long megaTimer = millis(); //��������� �������� ������� �������� ��������� ������������� ������ Mega

//��������� MQTT �������
extern long writeChannelID;				//ID ������ ThingSpeak ��� ������ ����� MQTT ������
extern int fieldsToPublish[8];    // Change to allow multiple fields.
extern float dataToPublish[8];    // Holds your field data.



void setup() {
	Serial.begin(115200);
	//Serial.setTimeout(250);
	//Serial.setRxBufferSize(500);
	WiFi_init();
	initThingSpeak();//������ �������� ������ �� ������ ThingSpeak
	initMQTT();

	//����������� ����� ��� ������ ������ MEGA
	//pinMode(PIN_RESET_MEGA, OUTPUT);
	//digitalWrite(PIN_RESET_MEGA, HIGH);

	//��������� �������� ������� �������� ��������� ������������� ������ Mega
	byte mega = MEGA_OFF;
	unsigned long megaTimer = millis(); 
}

void loop() {
	//�������� ����������� ������ �� ���� Serial �� ������ Mega
		checkSerial(); //��������� ��� ����� ����

	//����������� ����� � �������� �� ������, �������� ����������� ����� ��������� �� MQTT �������
		MQTTloop();

	//������ ������ � ���� ������������ �� ������ MEGA
	if (cycleRequestTemperature.check()) {
		RequestTemperature();
		// ���������� ������� ������ �������.
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
		// ���������� ������� ������ �������.
		cycleSendingDataToThingSpeak.reStart();
	}

	// �������� ������� ������ ����������� �� Mega ����� Serial
	if (cycleCheckMegaAndESP.check()) {
		//�������� ����������� ���������������� ������ MEGA.
		checkMegaAndESP();  
		// ���������� ������� ������ �������.
		cycleCheckMegaAndESP.reStart();
	}
	//delay(30000);
	///Publish();
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


//=============================== -- ��������� ����������� �� UART ���������� �� ������ MEGA -- ===========================
bool sFlag = true;
String serialReq = "";
//��������� ���������-�� ������ � ���� Serial
void checkSerial() {

	///MESS(F("checkSerial\n"));
	while (Serial.available() > 0) {
		///MESS(F("Serial availeble\n"));
		if (sFlag) {
			///MESS(F("If sFlag\n"));
			serialReq = "";
			sFlag = false;
		}
		char c = Serial.read();
		///MESS(F("read c ")); MESS(c); MESS(F("\n"));
		if (c == 10) {
			///MESS(F("c==10\n"));
			sFlag = true;
			parseSerialStr();
		}
		else if (c == 13) {
			// skip
		}
		else {
			if (serialReq.length() < MAX_SERIAL_REQ) {

				serialReq += c;
				///MESS(F("SerialMess: ")); MESS(serialReq); MESS(F("\n"));
				//Serial.println("serialReq: "+String(serialReq));
			}
		}
	} // while (Serial.available() > 0
} // checkSerial()

//������ ������, ����������� � Serial, ��������� ������ � ��������
void parseSerialStr() {
	if (serialReq[0] == '?') {
		///MESS(F("serialReq[0] == '?', call parseSerialCmd\n"));
		parseSerialCmd();
	}
	else {
		DEBUG("ESP[" + serialReq + "]");
	}
}

//������� �������, ����������� � Serial � �� ���������
void parseSerialCmd() {
	//DEBUGLN(F("parseSerialCmd"));
	String command, parameter;
	if (serialReq.indexOf(F("?")) >= 0) {
		int pBegin = serialReq.indexOf(F("?")) + 1;
		if (serialReq.indexOf(F("=")) >= 0) {
			int pParam = serialReq.indexOf(F("="));
			command = serialReq.substring(pBegin, pParam);
			parameter = serialReq.substring(pParam + 1);
		}
		else {
			command = serialReq.substring(pBegin);
			parameter = "";
		}
		/*DEBUG(F("command/parameter: "));
		DEBUG(command);
		DEBUG(F("/"));
		DEBUGLN(parameter);*/

	//============ ������ ����������� ������ =============
		// ?mega
		if (command == F("mega")) {																	
			 //MEGA �������� �������������, ��� ��������
			if (parameter == F("1")) {
				mega = MEGA_ON; 
				megaTimer = millis(); //������� ������ ��������� ��������� ������ MEGA
				DEBUGLN(F("Put: MEGA - working!"));
			}
		}

		// ?sendtemp�              ?sendtemp=1.23;5.78;33,33;7,77
		else if (command.substring(0, 8) == F("sendtemp")) {												
																																												
			/*  ��� ��������� ������������ ����� ��� ����������� ����� ������� � ������������';'
				/*int iparam2 = parameter2.toFloat();
				���� float ����� ����� ������
				���������� ����������� - . (�����)
				����������� - ; (���������)*//*
			//parameter// �������� ������������������ �������� ���������� ����� �����������
			int n = 0; //String buf_1="";
			while (parameter.length() > 0) {
				byte dividerIndex = parameter.indexOf(';');   // ���� ������ �����������
				String buf_1 = parameter.substring(0, dividerIndex);    // ������ ������ � ������ ������
				temperatures[n] = buf_1.toFloat();
				parameter = parameter.substring(dividerIndex + 1);   // ������� ������
				n++;
				}*/
			int index = command.substring(8).toInt();
			temperatures[index] = parameter.toFloat();
			DEBUGLN("temp " + command.substring(8) +": " + String(temperatures[index]) + "");
		}

		// ?test 
		else if (command == F("test")) {//������� ��� �������� ������ ������� ��������� � ESP													// ?test
			Serial.println("Put command test");
		}

		// ?reqestrssi
		else if (command == F("reqestrssi")) {//������ ������ ������� wi-fi
			Serial.print("?sendrssi=");
			Serial.println((long)WiFi.RSSI());
		}

		// ?sendGTargetTemp
		else if (command == F("sendGTargetTemp")) {//�������� �� MEGA �������� ���������� ������� ����������� �������, ��� ����� ����������
			/*DEBUGLN*/Serial.println("The global target temperature is obtained: "+ parameter);
			//���������� �� ������ MQTT � field3 (GTargetTemp)
			dataToPublish[2]= parameter.toFloat();
			fieldsToPublish[0] = 0; //field1 will be rec
			fieldsToPublish[1] = 0; //...
			fieldsToPublish[2] = 1;
			fieldsToPublish[3] = 0;
			fieldsToPublish[4] = 0;
			fieldsToPublish[5] = 0;
			fieldsToPublish[6] = 0;
			fieldsToPublish[7] = 0;

			mqttPublish(writeChannelID, dataToPublish, fieldsToPublish);
		}	
	} 
} // parseSerialCmd()
	//============================================================================


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