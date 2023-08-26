/* 
	NTP ������. ��������� ������� ������� �� ���������

	���������� ���������� ������� ����� ���� ����� ���������� �������������
	�� ���� ������ esp "������" �� ~1.7 ������� �� ����� (��� �������������). ������� ����������� ������ ������������� ������ 1 ���
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
	//ntp.asyncMode(false);   // ��������� ����������� �����
	//ntp.ignorePing(true);   // �� ��������� ���� �� �������
	//ntp.setHost(char* host);       // ���������� ���� (�� �����. "pool.ntp.org")
	// ������ ��������, ���� "pool.ntp.org" �� ��������
	//"ntp1.stratum2.ru"
	//"ntp2.stratum2.ru"
	//"ntp.msk-ix.ru"

	//�������������� ����
	uint8_t f=ntp.updateNow(); //!!! �����-�� ������ � ������. ������� �� ������ ���������� ������
	///Serial.println(f);

	//�������� ������ ����� �� ������ mega
	///Serial.println(ntp.synced());

	SendActualTime();

	//������� ������� status():
	// 0 - �� ��
	// 1 - �� ������� UDP
	// 2 - �� ��������� WiFi
	// 3 - ������ ����������� � �������
	// 4 - ������ �������� ������
	// 5 - ������� ������ �������
	// 6 - ������� ������������ ����� �������
}

void workClock() {
	ntp.tick();
}

//���������� ������������������ �� NTP ����� ������ mega
void SendActualTime() {
	if (/*ntp.status() == 0 &&*/ ntp.synced()){
		MEGA_SERIAL.println(F("?setTime=")+ ntp.timeString());

		DEBUGLN_WF(F("Send Time to MEGA (status NTP: ") + String(ntp.status()) + F("). Time ") + ntp.timeString());
	}
	else {
		DEBUGLN_WF(F("Don't send Time to MEGA (status NTP: ") + String(ntp.status()) + F("). Time ") + ntp.timeString());
	}
}