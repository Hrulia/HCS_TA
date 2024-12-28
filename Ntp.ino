/*
	NTP ������. ��������� ������� ������� �� ���������

	���������� ���������� ������� ����� ���� ����� ���������� �������������
	�� ���� ������ esp "������" �� ~1.7 ������� �� ����� (��� �������������). ������� ����������� ������ ������������� ������ 1 ���
*/
#include "./libraries/GyverNTP/src/GyverNTP.h"

/* ���������� ������� ��� � ������ �������*/
//#define DEBUG_ENABLE_NTP  //����� �������

#ifdef DEBUG_ENABLE_NTP
	#define DEBUG_PRINT_NTP(x) (Serial.print(x))
	#define DEBUG_PRINTLN_NTP(x) (Serial.println(x))
	#define DEBUGR_PRINTR_NTP(x,r) (Serial.print(x,r))
#else
	#define DEBUG_PRINT_NTP(x) 
	#define DEBUG_PRINTLN_NTP(x) 
	#define DEBUGR_PRINTR_NTP(x,r) 
#endif 


GyverNTP ntp(3); // ��������� �� ��������� (GMT 0, ������ ���������� 3600 ������ (1 ���))

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

	//ntp.setHost(char* host);       // ���������� ���� (�� �����. "pool.ntp.org")
	// ������ ��������, ���� "pool.ntp.org" �� ��������
	//"ntp1.stratum2.ru"
	//"ntp2.stratum2.ru"
	//"ntp.msk-ix.ru"

	//�������������� ���� �������������
	#ifdef DEBUG_ENABLE_NTP
		uint8_t ret = ntp.updateNow(); //!!! �����-�� ������ � ������. ������� �� ������ ���������� ������
		delay(1);
		DEBUG_PRINTLN_NTP(F("ntp.updateNow(): ") + String(ret));
		DEBUG_PRINTLN_NTP(F("ntp.status: ") + String(ntp.status()));
		DEBUG_PRINTLN_NTP(F("ntp.synced: ") + String(ntp.synced()));
	#else
		ntp.updateNow();
		delay(100);
	#endif
}	
	//������� ������� status():
	// 0 - �� ��
	// 1 - �� ������� UDP
	// 2 - �� ��������� WiFi
	// 3 - ������ ����������� � �������
	// 4 - ������ �������� ������
	// 5 - ������� ������ �������
	// 6 - ������� ������������ ����� �������


//���������� ������� �� ������ ����������� �������
void ntpClockWork() {
	ntp.tick();
}

//���������� ������������������ �� NTP ����� ������ mega2560
void SendActualTime() {
	//��������� ���� �������� ������
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
		cycleMegaTimeSynchronization.reStart(); //������������ �������
	}
}
