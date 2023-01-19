/*
Modul Doc for ESP8266
part of HCS_TA
*/

//��������������
/*��������������:
���������� �������: 3.3V (2.5-3.6V)
��� �����������: 300 �� ��� ������� � �������� ������, 35 �� �� ����� ������, 80 �� � ������ ����� �������
������������ ��� ���� � 12 ��.
Flash ������ (������ ���������): 1 ��
Flash ������ (�������� ���������): 1-16 �� � ����������� �� �����������
EEPROM ������: �� 4 ��
SRAM ������: 82 ��
������� ����: 80/160 ���
GPIO: 11 �����
���: 10 �����
����������: 10 �����
���: 1 ���
I2C: 1 ����� (�����������, ���� ����� ��������� �����)
I2S: 1 �����
SPI: 1 �����
UART: 1.5 �����
WiFi �����
*/
/*
��� ������ ����������� ����� ��� ���� ������ ������ �� �������� ������, ��������� � � ���� ������ https://rabbithole.wwwdotorg.org/2017/03/28/esp8266-gpio.html. ������������� ����������� ������ �������� D1 (GPIO5) � D2 (GPIO4). ���� ���������� ��������� �������� ������-�� ��������� (����, ����������, ��� �������� �������� ��� ������� ����������), �� ����� ������������ ������ ��� ����!
�� ���� �� ����� ����� I2C, �� ���� ����� ������������� �� ����� ������ ���� ����� Wire.begin(sda, scl).
*/
/* info:
https://alexgyver.ru/lessons/esp8266/
https://wikihandbk.com/wiki/ESP8266:��������/Arduino/����������/����������_ESP8266WiFi/�����_�������
https://kit.alexgyver.ru/tutorials-category/telegram/ //��������� ���
https://wikihandbk.com/wiki/ESP8266:������/�����_HUZZAH_ESP8266_��_Adafruit //��������� ������� ��������� ��� �������
���������� ����������� ������ � ������� ������ Generic: https://wikihandbk.com/wiki/ESP8266:��������/Arduino/����������/����������_ESP8266WiFi/�����_Generic/����������_�����������_������_�_�������_������_Generic
EasyIoT The easy way to build Internet of Things: https://iot-playground.com/
Serial.setDebugOutput(true);  //��������� ������ ��������������� ����������
WiFi.printDiag(Serial);       //����� ����������� � ������
//server.setNoDelay(true); // ���������� ��������� ������
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

