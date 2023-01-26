//Restrictions for the free version of ThingSpeak:
//	1. Number of messages - 3 million / year(~8 200 / day)
//	2. Message update interval limit - Every 15 seconds
//	3. Number of channels - 4
//	4. Private channel sharing - Limited to 3 shares


//Channel Tonshaevo
unsigned long ThingSpeakChannelNumber;// = 1287359; //Номер канала Tonshaevo_HCS на сайте ThingSpeak \\последняя цифра 9/0
const char * ThingSpeakWriteAPIKey = "8JCE0XOM858Q9P7O"; 

//Включить отладочный вывод 
#define DEBUG_ThS

// always include thingspeak header file after other header files and custom macros
#include <ThingSpeak.h>	

WiFiClient  clientThingSpeak;

void initThingSpeak() {
	Serial.println("\nThingSpeak initialization");
	ThingSpeak.begin(clientThingSpeak);
}

// Write one [value] to Field [item] of a ThingSpeak Channel
int ThingSpeakWriteField(int item, float value, int channelNumber, char* writeAPIKey, int optionParametr) {
	int httpCode = ThingSpeak.writeField(channelNumber, item, value, writeAPIKey);

#ifdef DEBUG_ThS
	if (httpCode == 200) {
		Serial.println("Channel " + String(item) + " write successful.");
	}
	else {
		Serial.println("Problem writing to channel " + String(item) + ", HTTP error " + String(httpCode));
	}
#endif
	return 0;
}


//Запись нескольких значений  о температурах на сайт ThingSpeak одновременно
int ThingSpeakWriteItems(float *value ) {
	///Serial.println("********************************************************************************");
	//{ 0x28, 0x18, 0x07, 0x40, 0x04, 0x00, 0x00, 0xCB },  // [0]- t1, подача cистемы
	//{ 0x28, 0x4D, 0xF7, 0xBF, 0x04, 0x00, 0x00, 0x43 },  // [1]- t2, обратка системы
	//{ 0x28, 0x18, 0x07, 0x40, 0x04, 0x00, 0x00, 0xCB },  // [2]- t3, отвод трехходового крана ТТК
	//{ 0x28, 0x20, 0x0F, 0x40, 0x04, 0x00, 0x00, 0xB9 },  // [3]- t4, подача ТТК
	//{ 0x28, 0x07, 0xBB, 0x3F, 0x04, 0x00, 0x00, 0xE8 },  // [4]- t5, обратка ТТК
	//{ 0x28, 0x18, 0x07, 0x40, 0x04, 0x00, 0x00, 0xCB },  // [5]- t6, верх ТА подача от ТТК
	//{ 0x28, 0x18, 0x07, 0x40, 0x04, 0x00, 0x00, 0xCB },  // [6]- t7, низ ТА обратка ТТК
	//{ 0x28, 0x18, 0x07, 0x40, 0x04, 0x00, 0x00, 0xCB },  // [7]- t8, верх ТА подача в систему
	//{ 0x28, 0x18, 0x07, 0x40, 0x04, 0x00, 0x00, 0xCB },  // [8]- t9, отвод трехходового крана системы
	//{ 0x28, 0x18, 0x07, 0x40, 0x04, 0x00, 0x00, 0xCB },  // [9]- t10, низ ТА, обратка системы
	//{ 0x28, 0xFF, 0x75, 0x61, 0x68, 0x18, 0x01, 0xAC },  // [10]-t11, верх ТА
	//{ 0x28, 0xFF, 0xF6, 0xDD, 0x67, 0x18, 0x01, 0xF5 },  // [11]-t12, низ ТА
	//{ 0x28, 0xFF, 0x04, 0x60, 0x68, 0x18, 0x01, 0x83 },  // [12]-t13, температура в помещении
	//{ 0x28, 0xD9, 0xEA, 0x16, 0xA8, 0x01, 0x3C, 0x10 },  // [13]-t14, температура на улице
	//{ 0x28, 0x18, 0x07, 0x40, 0x04, 0x00, 0x00, 0xCB },  // [14]-t15, подача ЭК 
	//{ 0x28, 0x97, 0x55, 0xC0, 0x04, 0x00, 0x00, 0xCA },  // [15]-t16, обратка ЭК

	// set the fields with the values
	ThingSpeak.setField(1, value[3]);
	ThingSpeak.setField(2, value[0]);
	ThingSpeak.setField(3, value[10]);
	ThingSpeak.setField(4, value[11]);
	ThingSpeak.setField(5, value[12]);
	ThingSpeak.setField(6, value[13]);
	ThingSpeak.setField(7, value[16]);
	ThingSpeak.setField(8, value[17]);

	// set the status
	String myStatus = String("Data on temperatures sent successfully <sistem time>");
	ThingSpeak.setStatus(myStatus);

	// write to the ThingSpeak channel
	int httpCode = ThingSpeak.writeFields(ThingSpeakChannelNumber, ThingSpeakWriteAPIKey);


#ifdef DEBUG_ThS
	if (httpCode == 200) {
		Serial.println("ThingSpeak Channel update successful.");
	}
	else {
		Serial.println("Problem updat ThingSpeak, HTTP error, code " + String(httpCode)+"");
	}
#endif
	return 0;
}
