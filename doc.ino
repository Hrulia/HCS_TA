/*
Modul Doc for ESP8266
part of HCS_TA
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
//server.setNoDelay(true); // отключение алгоритма Нейгла
*/

/*
ESP.restart()
ESP.deepSleep(microseconds, mode)

WiFi.mode(m): set mode to WIFI_AP, WIFI_STA, or WIFI_AP_STA
WiFi.softAP(ssid, password)
WiFi.printDiag(Serial)

Serial.print("ESP.getResetReason:       "); Serial.println(ESP.getResetReason());
Serial.print("getFlashChipSizeByChipId: "); Serial.println(getFlashChipSizeByChipId());
Serial.print("ESP.getVcc:               "); Serial.println(ESP.getVcc()); // ADC_MODE(ADC_VCC);
Serial.print("ESP.getCycleCount:        "); Serial.println(ESP.getCycleCount());
*/

