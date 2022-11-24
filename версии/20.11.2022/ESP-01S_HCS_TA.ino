/*
 Name:		ESP_01S_HCS_TA.ino
 Created:	02.01.2021 11:23:52
 Author:	Sergey
*/

//=====================================================
// Для обмена по UART с модулем MEGA на данный момент пока используется тотже Serial1, который идет на USB.
// но у esp8266 есть еще один UART, вот только буфер у них общий, поэтому нужно переключаться с одного на другой 
// функцией Serial.swap() и при этом, на всякий случай, еще чистим буфер функцией Serial.flush() 
// подробнее тут: https://esp8266.ru/forum/threads/zachem-polzovatsja-kostylem-softserial-kogda-u-esp8266-dva-apparatnyx-uart.4749/
//

#include <PubSubClient.h>
#include <ESP8266WiFi.h>  

//подключаем мои файлы .h и .cpp
#include "myCycle.h"
#include "ESP8266_wifi.h"

//включение отладки в основном модуле программы
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

//#define MESS(mess) (Serial.print("?esplog="+String(mess))) // Командa логирования модулем mega сообщения от esp

//////задаем интерфейс, через который отправляем команды модулю MEGA
////#define UART_TO_MEGALN(x) (Serial.println(x))

#define MAX_SERIAL_REQ  50 //
#define NUMBER_OF_DS18B20 18 //Number of sensor DS18B20 (16) + датчик температуры дыма (1) + текущая целевая температура (1)

// mega partner. Флаги состояния подключенного модуля Mega
#define MEGA_OFF 0
#define MEGA_ON  1

//настройка пинов
//#define PIN_RESET_MEGA D1 //пин генерации сигнала сброса для модуля MEGA

myCycle cycleRequestTemperature(MS_01M, true);			// 1м запрос температуры
//myCycle cycleRequestTargetTemperature(120000, true);			// 2м запрос текущей целевой температуры с учетом суточного расписания
myCycle cycleSendingDataToThingSpeak(MS_05M, true);	// 5м цикл отправки данных о температуре на сайт IoT
myCycle cycleCheckMegaAndESP(MS_03M, true);				//3мин цикл отправки в mega через serial команды своего присутствия: ?esp=1

float temperatures[(NUMBER_OF_DS18B20)]; //Массив температур датчиков DS18B20 

byte mega = MEGA_OFF;
unsigned long megaTimer = millis(); //Начальное значение таймера проверки состояния подключенного модуля Mega

//параметры MQTT сервера
extern long writeChannelID;				//ID канала ThingSpeak для записи через MQTT сервер
extern int fieldsToPublish[8];    // Change to allow multiple fields.
extern float dataToPublish[8];    // Holds your field data.



void setup() {
	Serial.begin(115200);
	//Serial.setTimeout(250);
	//Serial.setRxBufferSize(500);
	WiFi_init();
	initThingSpeak();//Клиент передачи данных на сервер ThingSpeak
	initMQTT();

	//настраиваем выход для сброса модуля MEGA
	//pinMode(PIN_RESET_MEGA, OUTPUT);
	//digitalWrite(PIN_RESET_MEGA, HIGH);

	//Начальное значение таймера проверки состояния подключенного модуля Mega
	byte mega = MEGA_OFF;
	unsigned long megaTimer = millis(); 
}

void loop() {
	//проверка поступления данных на порт Serial от модуля Mega
		checkSerial(); //проверяем как можно чаще

	//Поддержание связи и подписка на топики, проверка поступления новых сообщений от MQTT брокера
		MQTTloop();

	//Запрос данных о всех температурах из модуля MEGA
	if (cycleRequestTemperature.check()) {
		RequestTemperature();
		// перезапуск таймера вызова функции.
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
		// перезапуск таймера вызова функции.
		cycleSendingDataToThingSpeak.reStart();
	}

	// отпарвки команды своего присутствия на Mega через Serial
	if (cycleCheckMegaAndESP.check()) {
		//Проверка нормального функционирования модуля MEGA.
		checkMegaAndESP();  
		// перезапуск таймера вызова функции.
		cycleCheckMegaAndESP.reStart();
	}
	//delay(30000);
	///Publish();
}

/*******************************************************************************************/

//Запрос информации о температуре с модуля MEGA
void RequestTemperature() {
	//запрос к меге на передачу всех значений температуры 
	Serial.println("?reqesttemp=A");
}

////Запрос информации о текущей целевой температуре с учетом расписания с модуля MEGA
//void RequestTargetTemperature() {
//	Serial.println("?reqestTargetTemp");
//}


//=============================== -- Обработка поступившей по UART информации от модуля MEGA -- ===========================
bool sFlag = true;
String serialReq = "";
//проверяем поступили-ли данные в порт Serial
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

//Разбор строки, поступившей в Serial, выделение строки с командой
void parseSerialStr() {
	if (serialReq[0] == '?') {
		///MESS(F("serialReq[0] == '?', call parseSerialCmd\n"));
		parseSerialCmd();
	}
	else {
		DEBUG("ESP[" + serialReq + "]");
	}
}

//Парсинг команды, поступившей в Serial и ее обработка
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

	//============ Разбор поступивших команд =============
		// ?mega
		if (command == F("mega")) {																	
			 //MEGA прислала подтверждение, что работает
			if (parameter == F("1")) {
				mega = MEGA_ON; 
				megaTimer = millis(); //сбросим таймер выявление зависания модуля MEGA
				DEBUGLN(F("Put: MEGA - working!"));
			}
		}

		// ?sendtempХ              ?sendtemp=1.23;5.78;33,33;7,77
		else if (command.substring(0, 8) == F("sendtemp")) {												
																																												
			/*  Эта процедура обрабатывает сразу все температуры одной строкой с разделителем';'
				/*int iparam2 = parameter2.toFloat();
				приём float чисел через сериал
				десятичный разделитель - . (точка)
				разделитель - ; (семиколон)*//*
			//parameter// содержит последовательность значений температур через разделитель
			int n = 0; //String buf_1="";
			while (parameter.length() > 0) {
				byte dividerIndex = parameter.indexOf(';');   // ищем индекс разделителя
				String buf_1 = parameter.substring(0, dividerIndex);    // создаём строку с первым числом
				temperatures[n] = buf_1.toFloat();
				parameter = parameter.substring(dividerIndex + 1);   // остаток строки
				n++;
				}*/
			int index = command.substring(8).toInt();
			temperatures[index] = parameter.toFloat();
			DEBUGLN("temp " + command.substring(8) +": " + String(temperatures[index]) + "");
		}

		// ?test 
		else if (command == F("test")) {//Команда для проверки работы функции обработки в ESP													// ?test
			Serial.println("Put command test");
		}

		// ?reqestrssi
		else if (command == F("reqestrssi")) {//Запрос уровня сигнала wi-fi
			Serial.print("?sendrssi=");
			Serial.println((long)WiFi.RSSI());
		}

		// ?sendGTargetTemp
		else if (command == F("sendGTargetTemp")) {//Передача от MEGA значения глобальной целевой температуры системы, без учета расписания
			/*DEBUGLN*/Serial.println("The global target temperature is obtained: "+ parameter);
			//Отправляем на сервер MQTT в field3 (GTargetTemp)
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


//Контроль работы модуля mega и отправка сигнала своего ESP присутствия 
void checkMegaAndESP() {
	//Отправка модулю MEGA информации о своем нормальном функционировании 
	Serial.println(F("?esp=1"));

	//Проверяем как долго от модуля mega не поступала информации о его присутствии. Не реже раза в 3 минуты
	if ((millis() - megaTimer) > 180000) {
		mega = MEGA_OFF; 
		megaTimer = millis();
		//делаем reset MEGA
		DEBUGLN("Module MEGA reseting");
		//digitalWrite(PIN_RESET_MEGA, LOW);
		//delay(300);
		//digitalWrite(PIN_RESET_MEGA, HIGH);
	}
}	// cheсkMegaAndESP() 