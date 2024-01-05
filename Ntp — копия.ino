/* 
	NTP клиент. Получение точного времени из интернета

	Библиотека продолжает считать время даже после пропадания синхронизации
	По моим тестам esp "уходит" на ~1.7 секунды за сутки (без синхронизации). Поэтому стандартный период синхронизации выбран 1 час
*/
#include "./libraries/GyverNTP/src/GyverNTP.h"

/* Оформление отладки как у Алекса Гайвера*/

#define DEBUG_ENABLE_NTP  //режим отладки

#ifdef DEBUG_ENABLE_NTP
	#define DEBUG_PRINT_NTP(x) (Serial.print(x))
	#define DEBUG_PRINTLN_NTP(x) (Serial.println(x))
	#define DEBUGR_PRINTR_NTP(x,r) (Serial.print(x,r))
#else
	#define DEBUG_PRINT_NTP(x) 
	#define DEBUG_PRINTLN_NTP(x) 
	#define DEBUGR_PRINTR_NTP(x,r) 
#endif 


GyverNTP ntp; // параметры по умолчанию (GMT 0, период обновления 3600 секунд (1 час))

void initNTP() {

	Serial.println("\nNTP initialization");

#ifdef DEBUG_ENABLE_NTP
	bool status = ntp.begin();
	DEBUG_PRINTLN_NTP(String("Статус ntp.begin - " + status));
#else
	ntp.begin();
#endif 

	//ntp.setHost(char* host);       // установить хост (по умолч. "pool.ntp.org")
	// список серверов, если "pool.ntp.org" не работает
	//"ntp1.stratum2.ru"
	//"ntp2.stratum2.ru"
	//"ntp.msk-ix.ru"

	//синхронизируем часы
	uint8_t f=ntp.updateNow(); //!!! какая-то ошибка у алекса. Функция не всегда возвращает статус
	Serial.println(f);

	//отправим точное время на модуль mega
	///Serial.println(ntp.synced());

	SendActualTime();

	//статусы системы status():
	// 0 - всё ок
	// 1 - не запущен UDP
	// 2 - не подключен WiFi
	// 3 - ошибка подключения к серверу
	// 4 - ошибка отправки пакета
	// 5 - таймаут ответа сервера
	// 6 - получен некорректный ответ сервера
}

//обновление времени по своему внутреннему таймеру
void ntpClockWork() {
	ntp.tick(); 
}

//Отправляем синхронизированное по NTP время модулю mega
void SendActualTime() {
	DEBUG_PRINTLN_NTP(F("start SendActualTime procedure. Sys status: ") + String(ntp.status()));
	DEBUG_PRINTLN_NTP(F("flag synced is ") + String(ntp.synced()));
	if (/*ntp.status() == 0 &&*/ ntp.synced()){
		SERIAL_FOR_MEGA.println("?setTime="+ ntp.timeString());

		DEBUG_PRINTLN_NTP(F("Send Time to MEGA (status NTP: ") + String(ntp.status()) + F("). Time ") + ntp.timeString());
	}
	else {
		DEBUG_PRINTLN_NTP(F("Don't send Time to MEGA (status NTP: ") + String(ntp.status()) + F("). Time ") + ntp.timeString());
	}
}