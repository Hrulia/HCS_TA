//=============================== -- ��������� ����������� �� UART ���������� �� ������ MEGA -- ===========================
bool sFlag = true;
String serialReq = "";
//��������� ���������-�� ������ � ���� Serial
void checkSerial() {
	while (Serial.available() > 0) {
		if (sFlag) {
			serialReq = "";
			sFlag = false;
		}
		char c = Serial.read();
		if (c == 10) { // '\n' LF //��� ��������� ������� �������� ������, ���������� ���������� � �������� ������� �������� ���������� ������
			sFlag = true;
			parseSerialStr();
		}
		else if (c == 13) { //'\r' CR// skip								
			}
			else {
				if (serialReq.length() < MAX_SERIAL_REQ) {

					serialReq += c;
				}
			}
	} // while (Serial.available() > 0
} // checkSerial()

	//1 ���� ��������: ��������� ������� � ���������
void parseSerialStr() {
	if (serialReq[0] == '?') {
		parseSerialCmd();
	}
	else {
		///DEBUGLN("ESP[" + serialReq + "]");  //�������, ��, ��� ������ � ���� 
	}
}

//********  
// 2 ����:  ������� ����������� ������� � �� ���������
void parseSerialCmd() {
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
	
	//============================================================================
	//============ ������ ����������� ������ =============

//**********
// ?mega
		if (command == F("mega")) {//MEGA �������� �������������, ��� ��������
			if (parameter == F("1")) {
				mega = MEGA_ON;
				megaTimer = millis(); //������� ������ ��������� ��������� ������ MEGA
				DEBUGLN(F("Confirmation received: MEGA - working!"));
			}
		}

//**********
// ?sendtemp�              ?sendtemp=1.23;5.78;33,33;7,77
		else if (command.substring(0, 8) == F("sendtemp")) {
			//  !???? ������ � ���� �������!!! �� ������� ��� ������������ ��������� ��������!!!

			/*  ��� ��������� ������������ ����� ��� ����������� ����� ������� � ������������';'
			/*int iparam2 = parameter2.toFloat();
			���� float ����� ����� ������
			���������� ����������� - . (�����)
			����������� - ; (���������)*//*
			//parameter// �������� ������������������ �������� ���������� ����� �����������
			int n = 0; //String buf_1="";
			while (parameter.length() > 0) {
			byte dividerIndex = parameter.indexOf(';');   // ���� ������ �����������
			String buf_1 = parameter.substring(0, dividerIndex);    // ������ ������ � ������ ������
			temperatures[n] = buf_1.toFloat();
			parameter = parameter.substring(dividerIndex + 1);   // ������� ������
			n++;
			}*/
			int index = command.substring(8).toInt();
			temperatures[index] = parameter.toFloat();
			DEBUGLN("temp " + command.substring(8) + ": " + String(temperatures[index]) + "");
		}

//**********
// ?test 
		else if (command == F("test")) {//������� ��� �������� ������ ������� ��������� � ESP													// ?test
			Serial.println("Put command test");
		}

//**********
// ?reqestrssi
		else if (command == F("reqestrssi")) {//������ ������ ������� wi-fi
			Serial.print("?sendrssi=");
			Serial.println((long)WiFi.RSSI());
		}

//**********
// ?sendGTargetTemp
		else if (command == F("sendGTargetTemp")) {//�������� �� MEGA �������� ���������� ������� ����������� �������, ��� ����� ����������
			/*DEBUGLN*/Serial.println("The global target temperature is obtained: " + parameter);
			//���������� �� ������ MQTT � field3 (GTargetTemp)
			dataToPublish[2] = parameter.toFloat();
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
		//**********
		// ?sendSystemParameters=XYZK
		else if (command == F("sendSystemParameters")) {//�������� �� MEGA �������� ���������� ������� ����������� �������, ��� ����� ����������
			/*DEBUGLN*/Serial.println("�������� ������� sendSystemParameters � �����������:" + parameter);
			SysParametrs[0] = parameter[0];
			SysParametrs[1] = parameter[1];
			SysParametrs[2] = parameter[2];
			SysParametrs[3] = parameter[3];
			Serial.println("��������� ��������� ���������:"); Serial.println(String(SysParametrs[0])+ SysParametrs[1]+SysParametrs[2]+SysParametrs[3]);
		}
	}
}




