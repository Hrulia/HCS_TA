/*
	NTP клиент. Получение точного времени из интернета

	Библиотека продолжает считать время даже после пропадания синхронизации
	По моим тестам esp "уходит" на ~1.7 секунды за сутки (без синхронизации). Поэтому стандартный период синхронизации выбран 1 час
*/
#include "./libraries/GyverNTP/src/GyverNTP.h"

/* Оформление отладки как у Алекса Гайвера*/
//#define DEBUG_ENABLE_NTP  //режим отладки

#ifdef DEBUG_ENABLE_NTP
	#define DEBUG_PRINT_NTP(x) (Serial.print(x))
	#define DEBUG_PRINTLN_NTP(x) (Serial.println(x))
	#define DEBUGR_PRINTR_NTP(x,r) (Serial.print(x,r))
#else
	#define DEBUG_PRINT_NTP(x) 
	#define DEBUG_PRINTLN_NTP(x) 
	#define DEBUGR_PRINTR_NTP(x,r) 
#endif 


GyverNTP ntp(3); // параметры по умолчанию (GMT 0, период обновления 3600 секунд (1 час))

void initNTP() {
	DEBUG_PRINTLN_NTP(F("start procedure initNTP"));

	#ifdef DEBUG_ENABLE_NTP
		DEBUG_PRINTLN_NTP(F("ntp.status: ") + String(ntp.status()));
		DEBUG_PRINTLN_NTP(F("ntp.synced: ") + String(ntp.synced()));
		bool status = ntp.begin();
		DEBUG_PRINTLN_NTP(F("ntp.begin(): ") + String(status));
		DEBUG_PRINTLN_NTP(F("ntp.status: ") + String(ntp.status()));
		DEBUG_PRINTLN_NTP(F("ntp.synced: ") + String(ntp.synced()));
	#else
		ntp.begin();
	#endif 

	//ntp.setHost(char* host);       // установить хост (по умолч. "pool.ntp.org")
	// список серверов, если "pool.ntp.org" не работает
	//"ntp1.stratum2.ru"
	//"ntp2.stratum2.ru"
	//"ntp.msk-ix.ru"

	//синхронизируем часы принудительно
	#ifdef DEBUG_ENABLE_NTP
		uint8_t ret = ntp.updateNow(); //!!! какая-то ошибка у алекса. Функция не всегда возвращает статус
		delay(1);
		DEBUG_PRINTLN_NTP(F("ntp.updateNow(): ") + String(ret));
		DEBUG_PRINTLN_NTP(F("ntp.status: ") + String(ntp.status()));
		DEBUG_PRINTLN_NTP(F("ntp.synced: ") + String(ntp.synced()));
	#else
		ntp.updateNow();
		delay(100);
	#endif
}	
	//статусы системы status():
	// 0 - всё ок
	// 1 - не запущен UDP
	// 2 - не подключен WiFi
	// 3 - ошибка подключения к серверу
	// 4 - ошибка отправки пакета
	// 5 - таймаут ответа сервера
	// 6 - получен некорректный ответ сервера


//обновление времени по своему внутреннему таймеру
void ntpClockWork() {
	ntp.tick();
}

//Отправляем синхронизированное по NTP время модулю mega2560
void SendActualTime() {
	//Выполняем если сработал таймер
	if (cycleMegaTimeSynchronization.check()) {
		DEBUG_PRINTLN_NTP(F("start procedure SendActualTime"));
		DEBUG_PRINTLN_NTP(F("ntp.status: ") + String(ntp.status()));
		DEBUG_PRINTLN_NTP(F("ntp.synced: ") + String(ntp.synced()));

		if (ntp.synced()) {
			SERIAL_TO_MEGA.println(F("?setTime=") + ntp.timeString());
			DEBUG_PRINTLN_NTP(F("Send Time to MEGA. Time ") + ntp.timeString());
		}
		else {
			DEBUG_PRINTLN_NTP(F("Don't send Time to MEGA (ntp.status: ") + String(ntp.status()) + F("). Time ") + ntp.timeString());
		}
		cycleMegaTimeSynchronization.reStart(); //перезапустим счетчик
	}
}
