/*
 Name:		ESP_01S_HCS_TA.ino
 Created:	02.01.2021 11:23:52
 Author:	Sergey
*/

/*//!!!! как нужно добавлять файлы cpp и h в проект  https://arduinoprosto.ru/q/61634/neskolko-faylov-ino-v-odnom-eskize

в *.h - файле:
	extern byte Test;

в *.cpp - файле:
	byte Test = 0;

	Всё, теперь переменная Test доступна везде, где подключен соответствующий *.h - файл.*/


#include <PubSubClient.h>
//#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

//подключаем мои файлы .h и .cpp
#include "myCycle.h"

/* Оформление отладки как у Алекса Гайвера*/

//включение отладки в основном модуле программы
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
#define NUMBER_OF_DS18B20 18	//Number of sensor DS18B20 (16) + датчик температуры дыма (1) + текущая целевая температура (1)
#define SERIAL_TO_MEGA Serial			//Serial port for communication with MEGA

//Enumeration - параметры работы системы
enum sysParam{ ERR, ON, OFF, AUTO, OPEN, CLOSE, MYALG, PID};
String sysParamString[8] = { "Err","On", "Off", "Auto", "Open", "Close", "myAlg", "PID" }; //отображает имена элементов sysParam

///time_t SystemTime = 0; //esp system time ///удаляй, время есть теперь в объекте ntp

/* настройка пинов */
//#define PIN_RESET_MEGA D1 //пин генерации сигнала сброса для модуля MEGA

//global variables
float temperatures[(NUMBER_OF_DS18B20)];	//Массив температур датчиков DS18B20 
int SysParametrs[6];											// system (in MEGA) parameters and variables array
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
myCycle cycleRequestTemperature(MS_01M, true);					// 2м, 1м, 5с(mega зависала, видимо от частых запросов и переполнения буфера) 30 запрос температуры
myCycle cycleRequestSystemParameters(MS_20S, true);			// 3м, 30c, запрос параметров работы системы.
//myCycle cycleRequestTargetTemperature(120000, true);	// 2м запрос текущей целевой температуры с учетом суточного расписания
myCycle cycleSendingDataToThingSpeak(MS_05M, true);			// 5м цикл отправки данных о температуре на сайт ThingSpeak. Ограничение: не чаще раза в 15 секунд.
myCycle cycleCheckMegaAndESP(MS_03M, true);							//3мин цикл отправки в mega через serial команды своего присутствия: ?esp=1
myCycle cycleMegaTimeSynchronization(MS_30M, true,true);			// 1час цикл отправки команды синхронизации времени в мегу.


/*****************************************************/
void setup() {
	//Serial.setRxBufferSize(500); // по умолчанию в ESP 256 Байт
	Serial.begin(9600);
	DEBUG_PRINTLN_MAIN(F("\n*******   Start setup()   *******"));
	//Serial.swap(); // GPIO15/D8 (TX) и GPIO13/D7 (RX)
	//Serial.setTimeout(250);
	 

	// Вывод информации о контроллере
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
	//Вывод напряжения питания ESP
	//ADC_MODE (ADC_VCC); //перенастроить АЦП при запуске
	//ESP.getVcc() //может использоваться для измерения напряжения питания
	////В этом режиме вывод TOUT должен быть отключен.
	////по умолчанию АЦП настроен на чтение с помощью TOUT pin analogRead(A0)и ESP.getVCC()недоступен.

	//Pins configuration
	//настраиваем выход для сброса модуля MEGA
	//pinMode(PIN_RESET_MEGA, OUTPUT);
	//digitalWrite(PIN_RESET_MEGA, HIGH);

	//Пин встроенного светодиода на плате ESP8266
	pinMode(LED_BUILTIN, OUTPUT);


	//Инициализация модулей
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

	// работа NTP модуля. Обновление времени c сервера с периодичностью по своему внутреннему таймеру.
	DEBUG_PRINTLN_MAIN(F("call ntpClockWork()"));
	ntpClockWork(); 

	//проверка поступления данных на порт Serial от модуля Mega
	DEBUG_PRINTLN_MAIN(F("call checkSerial()"));
	checkSerial(); //проверяем как можно чаще

	//Поддержание связи и подписка на топики, проверка поступления новых сообщений от MQTT брокера
	DEBUG_PRINTLN_MAIN(F("call MQTTloop()"));
	MQTTloop();


/*************************************************/
/*   процедуры вызываемые по сработке таймеров   */
/*************************************************/

	//Отправка в Mega2560 точного времени
	DEBUG_PRINTLN_MAIN(F("call SendActualTime()"));
	SendActualTime();
	
	//Запрос данных о всех температурах из модуля MEGA
	if (cycleRequestTemperature.check()) {
		DEBUG_PRINTLN_MAIN(F("call RequestTemperature()"));
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
		DEBUG_PRINTLN_MAIN(F("call ThingSpeakWriteItems()"));
		ThingSpeakWriteItems(temperatures);
		cycleSendingDataToThingSpeak.reStart();
	}

	// отпарвки команды своего присутствия на Mega через Serial
	if (cycleCheckMegaAndESP.check()) {
		DEBUG_PRINTLN_MAIN(F("call checkMegaAndESP()"));
		checkMegaAndESP();  		//Проверка нормального функционирования модуля MEGA.
		cycleCheckMegaAndESP.reStart();
	}

	//запрос параметров работы системы отопления
	if (cycleRequestSystemParameters.check()) {
		DEBUG_PRINTLN_MAIN(F("call RequestSystemParameters()"));
		RequestSystemParameters(); //запро параметров работы системы
		cycleRequestSystemParameters.reStart();
	}

	//мигнем встроенным светодиодом, просигнализируем об окончании цикла loop и видеть, что плата не зависла
	//digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); 
	DEBUG_PRINTLN_MAIN(F("call blinkBuiltInLed()"));
	blinkBuiltInLed();

	DEBUG_PRINTLN_MAIN(F("******* End loop()"));
} //end loop()


/*******************************************************************************************/
/*******************************   Функции   ***********************************************/
/*******************************************************************************************/

//Запрос информации о температуре с модуля MEGA
void RequestTemperature() {
	//запрос к меге на передачу всех значений температуры 
	SERIAL_TO_MEGA.println(F("?reqesttemp=A"));
}

////Запрос информации о текущей целевой температуре с учетом расписания с модуля MEGA
//void RequestTargetTemperature() {
//	Serial.println("?reqestTargetTemp");
//}

//Контроль работы модуля mega и отправка сигнала своего ESP присутствия 
void checkMegaAndESP() {
	//Отправка модулю MEGA информации о своем нормальном функционировании 
	SERIAL_TO_MEGA.println(F("?esp=1"));

	//Проверяем как долго от модуля mega не поступала информации о его присутствии. Не реже раза в 3 минуты
	if ((millis() - megaTimer) > 180000UL) {
		mega = MEGA_OFF; 
		megaTimer = millis();
		//делаем reset MEGA
		DEBUG_PRINTLN_MAIN(F("Module MEGA reseting"));
		//digitalWrite(PIN_RESET_MEGA, LOW);
		//delay(300);
		//digitalWrite(PIN_RESET_MEGA, HIGH);
	}
}	// cheсkMegaAndESP() 

//запрос к меге на передачу значений внутренних параметров 
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
