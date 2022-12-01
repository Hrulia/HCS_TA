/*
 Name:		ESP_01S_HCS_TA.ino
 Created:	02.01.2021 11:23:52
 Author:	Sergey
*/
//Характеристики
/*Характеристики:
Напряжение питания: 3.3V (2.5-3.6V)
Ток потребления: 300 мА при запуске и передаче данных, 35 мА во время работы, 80 мА в режиме точки доступа
Максимальный ток пина – 12 мА.
Flash память (память программы): 1 МБ
Flash память (файловое хранилище): 1-16 МБ в зависимости от модификации
EEPROM память: до 4 кБ
SRAM память: 82 кБ
Частота ядра: 80/160 МГц
GPIO: 11 пинов
ШИМ: 10 пинов
Прерывания: 10 пинов
АЦП: 1 пин
I2C: 1 штука (программный, пины можно назначить любые)
I2S: 1 штука
SPI: 1 штука
UART: 1.5 штуки
WiFi связь
*/
/*
При старте контроллера почти все пины делают скачок до высокого уровня, подробнее – в этой статье https://rabbithole.wwwdotorg.org/2017/03/28/esp8266-gpio.html. Единственными “спокойными” пинами являются D1 (GPIO5) и D2 (GPIO4). Если контроллер управляет напрямую какими-то железками (реле, транзистор, или является “кнопкой” для другого устройства), то лучше использовать именно эти пины!
На этих же пинах сидит I2C, но шину можно переназначить на любые другие пины через Wire.begin(sda, scl).
*/
/* info:
https://alexgyver.ru/lessons/esp8266/
https://wikihandbk.com/wiki/ESP8266:Прошивки/Arduino/Библиотеки/Библиотека_ESP8266WiFi/Класс_станции
https://kit.alexgyver.ru/tutorials-category/telegram/ //телеграмм бот
https://wikihandbk.com/wiki/ESP8266:Модули/Плата_HUZZAH_ESP8266_от_Adafruit //служедные функции контактов при запуске
Асинхронно выполняемые задачи с помощью класса Generic: https://wikihandbk.com/wiki/ESP8266:Прошивки/Arduino/Библиотеки/Библиотека_ESP8266WiFi/Класс_Generic/Асинхронно_выполняемые_задачи_с_помощью_класса_Generic
EasyIoT The easy way to build Internet of Things: https://iot-playground.com/
Serial.setDebugOutput(true);  //включение вывода диагностической информации
WiFi.printDiag(Serial);       //вывод диагностики в сериал

*/

//=====================================================
// Для обмена по UART с модулем MEGA на данный момент пока используется тот же Serial1, который идет на USB.
// но у esp8266 есть еще один UART(фактически это просто вывод того же uart на другие ножки мк), вот только буфер у них общий, поэтому нужно переключаться с одного на другой 
// функцией Serial.swap() и при этом, на всякий случай, еще чистим буфер функцией Serial.flush() 
// подробнее тут: https://esp8266.ru/forum/threads/zachem-polzovatsja-kostylem-softserial-kogda-u-esp8266-dva-apparatnyx-uart.4749/
//

//#include <ESP8266WebServerSecure.h>
//#include <ESP8266WebServer-impl.h>
//#include "HCS_server.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>

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


/* Work with UART */
//#define MESS(mess) (Serial.print("?esplog="+String(mess))) // Командa логирования модулем mega сообщения от esp
//////задаем интерфейс, через который отправляем команды модулю MEGA
////#define UART_TO_MEGALN(x) (Serial.println(x))
#define MAX_SERIAL_REQ  50 //
#define NUMBER_OF_DS18B20 18 //Number of sensor DS18B20 (16) + датчик температуры дыма (1) + текущая целевая температура (1)



/* настройка пинов */
//#define PIN_RESET_MEGA D1 //пин генерации сигнала сброса для модуля MEGA

/* Initial Timers */
myCycle cycleRequestTemperature(MS_05S, true);			// 1м, 5с запрос температуры
//myCycle cycleRequestTargetTemperature(120000, true);			// 2м запрос текущей целевой температуры с учетом суточного расписания
myCycle cycleSendingDataToThingSpeak(MS_05M, true);	// 5м цикл отправки данных о температуре на сайт IoT
myCycle cycleCheckMegaAndESP(MS_03M, true);				//3мин цикл отправки в mega через serial команды своего присутствия: ?esp=1

//Массив температур датчиков DS18B20 
float temperatures[(NUMBER_OF_DS18B20)]; 

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


/*****************************************************/
void setup() {
	//Serial.setRxBufferSize(500); // по умолчанию в ESP 256 Байт
	Serial.begin(115200);
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
	initWebServer();

	//настраиваем выход для сброса модуля MEGA
	//pinMode(PIN_RESET_MEGA, OUTPUT);
	//digitalWrite(PIN_RESET_MEGA, HIGH);

	//моргание диодиком на плате, что бы видеть, что плата не зависла
	//pinMode(LED_BUILTIN, OUTPUT);
	
	//Начальное значение таймера проверки состояния подключенного модуля Mega
	byte mega = MEGA_OFF;
	megaTimer = millis(); 
}
unsigned long timeBlink = millis();

/*********************************************/
void loop() {
	// Web-server listen for HTTP requests from clients
	checkWebClient();

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