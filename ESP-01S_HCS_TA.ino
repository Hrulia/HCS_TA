/*
 Name:		ESP_01S_HCS_TA.ino
 Created:	02.01.2021 11:23:52
 Author:	Sergey
*/


#include <PubSubClient.h>
//#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

//подключаем мои файлы .h и .cpp
#include "myCycle.h"


//включение отладки в основном модуле программы
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
#define NUMBER_OF_DS18B20 18	//Number of sensor DS18B20 (16) + датчик температуры дыма (1) + текущая целевая температура (1)
#define MEGA_SERIAL Serial			//Serial port for communication with MEGA

//Перечисления для параметров работы системы
enum sysParam{ ERR, ON, OFF, AUTO, OPEN, CLOSE, MYALG, PID};
String sysParamString[8] = { "Err","On", "Off", "Auto", "Open", "Close", "myAlg", "PID" }; //отображает имена элементов sysParam

///time_t SystemTime = 0; //esp system time

/* настройка пинов */
//#define PIN_RESET_MEGA D1 //пин генерации сигнала сброса для модуля MEGA




//global variables
float temperatures[(NUMBER_OF_DS18B20)];	//Массив температур датчиков DS18B20 
int SysParametrs[6];											//arrey system (in MEGA) parameters and variables
/* 
[0] - BoilerPumpMode				//1 - on, 2 - off, 3 - auto
[1] - SystemPumpMode				//1 - on, 2 - off, 3 - auto
[2] - SysTempControlMode		//6 – мой алгоритм регулирования, 7 - PID регулятор
[3] - DoorAirMode						//4 - open, 5 - close, 3 - auto
[4] - reserved
[5] - reserved

*/

/* Watch dog for Mega */
// mega partner. Флаги состояния подключенного модуля Mega
#define MEGA_OFF 0
#define MEGA_ON  1
byte mega = MEGA_OFF;
unsigned long megaTimer = millis(); //Таймер проверки состояния подключенного модуля Mega


/* Параметры MQTT сервера */
extern long writeChannelID;				//ID канала ThingSpeak для записи через MQTT сервер
extern int fieldsToPublish[8];    // Change to allow multiple fields.
extern float dataToPublish[8];    // Holds your field data.


/* Initial Timers */
myCycle cycleRequestTemperature(MS_01M, true);			// 2м, 1м, 5с(mega зависала, видимо от частых запросов и переполнения буфера) 30 запрос температуры
myCycle cycleRequestSystemParameters(MS_10S, true);  // 3м, 30c, запрос параметров работы системы.
//myCycle cycleRequestTargetTemperature(120000, true);			// 2м запрос текущей целевой температуры с учетом суточного расписания
myCycle cycleSendingDataToThingSpeak(MS_05M, true);	// 5м цикл отправки данных о температуре на сайт IoT
myCycle cycleCheckMegaAndESP(MS_03M, true);				//3мин цикл отправки в mega через serial команды своего присутствия: ?esp=1
myCycle cycleMegaTimeSynchronization(MS_30M, true);				// 1час цикл отправки команды синхронизации времени в мегу.


/*****************************************************/
void setup() {
	//Serial.setRxBufferSize(500); // по умолчанию в ESP 256 Байт
	Serial.begin(9600);
	//Serial.swap(); // GPIO15/D8 (TX) и GPIO13/D7 (RX)
	//Serial.setTimeout(250);

	// информация о контроллере
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
	initThingSpeak();//Клиент передачи данных на сервер ThingSpeak
	initMQTT();
	initWebServer(); //
	initNTP();

	//настраиваем выход для сброса модуля MEGA
	//pinMode(PIN_RESET_MEGA, OUTPUT);
	//digitalWrite(PIN_RESET_MEGA, HIGH);

	//моргание диодиком на плате, что бы видеть, что плата не зависла
	pinMode(LED_BUILTIN, OUTPUT);
	
	//Начальное значение таймера проверки состояния подключенного модуля Mega
	byte mega = MEGA_OFF;
	megaTimer = millis();
}
unsigned long timeBlink = millis();

/*********************************************/
void loop() {
	// Web-server listen for HTTP requests from clients
	checkWebClient();

	workClock(); //работа NTP модуля

	//проверка поступления данных на порт Serial от модуля Mega
	checkSerial(); //проверяем как можно чаще

	//Поддержание связи и подписка на топики, проверка поступления новых сообщений от MQTT брокера
	MQTTloop();

	//Запрос данных о всех температурах из модуля MEGA
	if (cycleRequestTemperature.check()) {
		RequestTemperature();
		cycleRequestTemperature.reStart();
	}
									////Запрос данных о текущей целевой температуре с учетом расписания
									//if (cycleRequestTargetTemperature.check()) {
									//	RequestTargetTemperature();
									//	// перезапуск таймера вызова функции.
									//	cycleRequestTargetTemperature.clear();
									//	cycleRequestTargetTemperature.reStart();
									//}

	//отправки данных о температуре на сайт ThingSpeak (не чаще раза в 15 секунд)
	if (cycleSendingDataToThingSpeak.check()) {
		ThingSpeakWriteItems(temperatures);
		cycleSendingDataToThingSpeak.reStart();
	}

	// отпарвки команды своего присутствия на Mega через Serial
	if (cycleCheckMegaAndESP.check()) {
		checkMegaAndESP();  		//Проверка нормального функционирования модуля MEGA.
		cycleCheckMegaAndESP.reStart();
	}

	//запрос параметров работы системы отопления
	if (cycleRequestSystemParameters.check()) {
		RequestSystemParameters(); //запро параметров работы системы
		cycleRequestSystemParameters.reStart();
	}

	//отправки команды синхронизации времени в мегу	
	if (cycleMegaTimeSynchronization.check()) {
		SendActualTime(); //Отправка в Mega точного времени
		cycleMegaTimeSynchronization.reStart();
	}
}


/*******************************************************************************************/
/*******************************************************************************************/
/*******************************************************************************************/

//Запрос информации о температуре с модуля MEGA
void RequestTemperature() {
	//запрос к меге на передачу всех значений температуры 
	Serial.println(F("?reqesttemp=A"));
}

////Запрос информации о текущей целевой температуре с учетом расписания с модуля MEGA
//void RequestTargetTemperature() {
//	Serial.println("?reqestTargetTemp");
//}

//Контроль работы модуля mega и отправка сигнала своего ESP присутствия 
void checkMegaAndESP() {
	//Отправка модулю MEGA информации о своем нормальном функционировании 
	MEGA_SERIAL.println(F("?esp=1"));

	//Проверяем как долго от модуля mega не поступала информации о его присутствии. Не реже раза в 3 минуты
	if ((millis() - megaTimer) > 180000UL) {
		mega = MEGA_OFF; 
		megaTimer = millis();
		//делаем reset MEGA
		DEBUGLN("Module MEGA reseting");
		//digitalWrite(PIN_RESET_MEGA, LOW);
		//delay(300);
		//digitalWrite(PIN_RESET_MEGA, HIGH);
	}
}	// cheсkMegaAndESP() 

//запрос к меге на передачу значений внутренних параметров 
void RequestSystemParameters() {
	MEGA_SERIAL.println(F("?getSystemParameters"));
}