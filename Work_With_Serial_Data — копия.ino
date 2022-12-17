//=============================== -- Обработка поступившей по UART информации от модуля MEGA -- ===========================
bool sFlag = true;
String serialReq = "";
//проверяем поступили-ли данные в порт Serial
void checkSerial() {
	while (Serial.available() > 0) {
		if (sFlag) {
			serialReq = "";
			sFlag = false;
		}
		char c = Serial.read();
		if (c == 10) { // '\n' LF //При получении символа перевода строки, считывание прерывааем и вызываем команду парсинга полученной строки
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

	//1 этап парсинга: выделение команды и параметра
void parseSerialStr() {
	if (serialReq[0] == '?') {
		parseSerialCmd();
	}
	else {
		///DEBUGLN("ESP[" + serialReq + "]");  //выводим, то, что пришло в порт 
	}
}

//********  
// 2 этап:  разборс поступившей команды и ее обработка
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
	//============ Разбор поступивших команд =============

//**********
// ?mega
		if (command == F("mega")) {//MEGA прислала подтверждение, что работает
			if (parameter == F("1")) {
				mega = MEGA_ON;
				megaTimer = millis(); //сбросим таймер выявление зависания модуля MEGA
				DEBUGLN(F("Confirmation received: MEGA - working!"));
			}
		}

//**********
// ?sendtempХ              ?sendtemp=1.23;5.78;33,33;7,77
		else if (command.substring(0, 8) == F("sendtemp")) {
			//  !???? Ошибка в этой функции!!! Не понятно как обрабатывает несколько значений!!!

			/*  Эта процедура обрабатывает сразу все температуры одной строкой с разделителем';'
			/*int iparam2 = parameter2.toFloat();
			приём float чисел через сериал
			десятичный разделитель - . (точка)
			разделитель - ; (семиколон)*//*
			//parameter// содержит последовательность значений температур через разделитель
			int n = 0; //String buf_1="";
			while (parameter.length() > 0) {
			byte dividerIndex = parameter.indexOf(';');   // ищем индекс разделителя
			String buf_1 = parameter.substring(0, dividerIndex);    // создаём строку с первым числом
			temperatures[n] = buf_1.toFloat();
			parameter = parameter.substring(dividerIndex + 1);   // остаток строки
			n++;
			}*/
			int index = command.substring(8).toInt();
			temperatures[index] = parameter.toFloat();
			DEBUGLN("temp " + command.substring(8) + ": " + String(temperatures[index]) + "");
		}

//**********
// ?test 
		else if (command == F("test")) {//Команда для проверки работы функции обработки в ESP													// ?test
			Serial.println("Put command test");
		}

//**********
// ?reqestrssi
		else if (command == F("reqestrssi")) {//Запрос уровня сигнала wi-fi
			Serial.print("?sendrssi=");
			Serial.println((long)WiFi.RSSI());
		}

//**********
// ?sendGTargetTemp
		else if (command == F("sendGTargetTemp")) {//Передача от MEGA значения глобальной целевой температуры системы, без учета расписания
			/*DEBUGLN*/Serial.println("The global target temperature is obtained: " + parameter);
			//Отправляем на сервер MQTT в field3 (GTargetTemp)
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
		else if (command == F("sendSystemParameters")) {//Передача от MEGA значения глобальной целевой температуры системы, без учета расписания
			/*DEBUGLN*/Serial.println("Получена посылка sendSystemParameters с параметрами:" + parameter);
			SysParametrs[0] = parameter[0];
			SysParametrs[1] = parameter[1];
			SysParametrs[2] = parameter[2];
			SysParametrs[3] = parameter[3];
			Serial.println("сохранены системные параметры:"); Serial.println(String(SysParametrs[0])+ SysParametrs[1]+SysParametrs[2]+SysParametrs[3]);
		}
	}
}




