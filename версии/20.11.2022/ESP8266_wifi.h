// ESP8266_Wi_Fi.h

#ifndef _ESP8266_WI_FI_h
#define _ESP8266_WI_FI_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
/////
#include <ESP8266WiFi.h>

//Подключение/переподключение к сети wi-fi модуля esp8266
int WiFi_init();



#endif

