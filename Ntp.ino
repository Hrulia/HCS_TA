/* 
	NTP клиент. Получение точного времени из интернета

	Библиотека продолжает считать время даже после пропадания синхронизации
	По моим тестам esp "уходит" на ~1.7 секунды за сутки (без синхронизации). Поэтому стандартный период синхронизации выбран 1 час
*/
#include <GyverNTP.h>

#define NTP_DEBUG
#define TIME_ZONE 3 //time zone UTC + 03:00

#define NTP_DEBUG

#ifdef NTP_DEBUG
#define DEBUG_WF(x) (Serial.print(x))
#define DEBUGLN_WF(x) (Serial.println(x))
#define DEBUGR_WF(x,r) (Serial.print(x,r))
#else
#define DEBUG_WF(x) 
#define DEBUGLN_WF(x)
#define DEBUGR_WF(x,r) 
#endif 


GyverNTP ntp;

void initNTP() {

	Serial.println("\nNTP initialization");
	ntp.setGMT(TIME_ZONE);
	ntp.begin();
	//ntp.asyncMode(false);   // выключить асинхронный режим
	//ntp.ignorePing(true);   // не учитывать пинг до сервера
	//ntp.setHost(char* host);       // установить хост (по умолч. "pool.ntp.org")
	// список серверов, если "pool.ntp.org" не работает
	//"ntp1.stratum2.ru"
	//"ntp2.stratum2.ru"
	//"ntp.msk-ix.ru"

	//синхронизируем часы
	uint8_t f=ntp.updateNow(); //!!! какая-то ошибка у алекса. Функция не всегда возвращает статус
	///Serial.println(f);

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

void workClock() {
	ntp.tick();
}

//Отправляем синхронизированное по NTP время модулю mega
void SendActualTime() {
	if (/*ntp.status() == 0 &&*/ ntp.synced()){
		MEGA_SERIAL.println(F("?setTime=")+ ntp.timeString());

		DEBUGLN_WF(F("Send Time to MEGA (status NTP: ") + String(ntp.status()) + F("). Time ") + ntp.timeString());
	}
	else {
		DEBUGLN_WF(F("Don't send Time to MEGA (status NTP: ") + String(ntp.status()) + F("). Time ") + ntp.timeString());
	}
}