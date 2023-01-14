// 26.11.2022
// Функционал web-сервера для ESP_HCS_TA
//info: 
//++ https://www.joyta.ru/12628-sozdanie-prostogo-veb-servera-nodemcu-esp8266-v-arduino-ide/
// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html
// https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
// 

#include <ESP8266WebServer.h>

String prepareHtmlPage(float temperature[], int sysParam[], bool refresh=false);


String webPage = ""; 
uint16_t servPort = 80;
//WiFiServer server(80); //arduino, поддерживает несколько клиентов
ESP8266WebServer webServer(servPort); //only support one simultaneous client


	//web server starting
void initWebServer() {
	webServer.begin();


/*	IPAddress local_ip(192, 168, 1, 1);
	IPAddress gateway(192, 168, 1, 1);
	IPAddress subnet(255, 255, 255, 0);
	WiFi.softAP(ssid, password);
	WiFi.softAPConfig(local_ip, gateway, subnet); */

	Serial.println("HTTP web-server started on port "+String(servPort));

	//installing handlers for the request html web clients
	webServer.on("/", handle_OnConnect);
	webServer.on("/BoilerPumpMode", handle_BoilerPumpMode);
	webServer.on("/SystemPumpMode", handle_SystemPumpMode);
	webServer.on("/SysTempControlMode", handle_SysTempControlMode);
	webServer.on("/DoorAir", handle_DoorAir);
	webServer.on("/Parametr1", handle_Parametr1);
	webServer.on("/Parametr2", handle_Parametr2);
	webServer.on("/AutoRefresh", handle_AutoRefresh);
	webServer.onNotFound( []() {webServer.send(404, "text/plain", "Not found");});
}

// Web-server listen for HTTP requests from clients
void checkWebClient() {
	webServer.handleClient();
}



void handle_OnConnect() {
	///Serial.println("handle_OnConnect");
	webServer.send(200, "text/html", prepareHtmlPage(temperatures, SysParametrs));
}

void handle_BoilerPumpMode() {
	//Отправка команды установки режима на модуль Mega
	if (webServer.arg(0) == "Auto") {
		Serial.println(F("?setBoilerPumpMode=3"));
	}
	else if (webServer.arg(0) == "On"){
		Serial.println("?setBoilerPumpMode=1");
	}
	else if (webServer.arg(0) == "Off") {
		Serial.println("?setBoilerPumpMode=2");
	}
	webServer.send(200, "text/html", prepareHtmlPage(temperatures, SysParametrs));
}

void handle_SystemPumpMode() {
	///Serial.println("handle_SystemPumpMode");
	//Отправка команды установки режима на модуль Mega
	if (webServer.arg(1) == "Auto") {
		Serial.println("?setSystemPumpMode=3");
	}
	else if (webServer.arg(1) == "On") {
		Serial.println("?setSystemPumpMode=1");
	}
	else if (webServer.arg(1) == "Off") {
		Serial.println("?setSystemPumpMode=2");
	}
	webServer.send(200, "text/html", prepareHtmlPage(temperatures, SysParametrs));
}

void handle_SysTempControlMode() {
	///Serial.println("handle_SysTempControlMode");

	webServer.send(200, "text/html", prepareHtmlPage(temperatures, SysParametrs));
}
void handle_DoorAir() {
	///Serial.println("handle_DoorAir");

	webServer.send(200, "text/html", prepareHtmlPage(temperatures, SysParametrs));
}
void handle_Parametr1() {
	///Serial.println("handle_Parametr1");
	
	webServer.send(200, "text/html", prepareHtmlPage(temperatures, SysParametrs));
}
void handle_Parametr2() {
	///Serial.println("handle_Parametr2");
	
	webServer.send(200, "text/html", prepareHtmlPage(temperatures, SysParametrs));
}

void handle_AutoRefresh() {
	///Serial.println("handle_AutoRefresh");
	webServer.send(200, "text/html", prepareHtmlPage(temperatures, SysParametrs, true));
}

//containing the contents of the web page
String prepareHtmlPage(float temperature[], int sysParam[], bool refresh){
	static bool refreshPage=false; //флаг автоматического обновления страницы с переходом на главную
	String htmlPage;	

	if (refresh) { refreshPage = !refreshPage; /*Serial.println("TRUE");*/ }

	/*htmlPage.reserve(7000); */              // prevent ram fragmentation
/*
	htmlPage = F("HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Connection: close\r\n"  // the connection will be closed after completion of the response
		"Refresh: 5\r\n"         // refresh the page automatically every 5 sec
		"\r\n"
		"<!DOCTYPE HTML>"
		"<html>"
		"Analog input:  ");
	htmlPage += analogRead(A0);
	htmlPage += F("</html>"
		"\r\n");
	return htmlPage;
	*/
	htmlPage = F("<!DOCTYPE html>"
		"<html><head><meta name = 'viewport' content = 'width=device-width, initial-scale=1.0, user-scalable=no'>"
		// в HTML5 в тег meta достаточно добавить <meta charset="utf-8"> вместо <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		//"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>");
		"<meta charset='utf-8'>");
		refreshPage ? htmlPage += "<meta http-equiv='Refresh' content='5; URL=/'>" :"";
		htmlPage +=F("<title>HCS Control(Tonshaevo)</title>"
		"<style>"
		".txtGray{text-align:right; width:50px; background:#DDD}"
		".txtWhite{text-align:right; width:50px;}"
		".listGray{width:60px; background:#DDD}" //67
		"</style>"
		"</head>"

		"<body>"
		"<a href='/'><h1>HCS Control (Tonshaevo)</h1></a>"
		"<form>"
		"<table cellspacing='0' cellpadding='5'>"
		"<caption>Тепловые показатели системы отопления</caption>"
		"<thead><th>Температура</th><th>Значение, °C</th><th></th></thead>"
		"<tbody>"
		"<tr style='background:#DDD'>"
		"<td>Дымовые газы</td>"
		"<td>");
		htmlPage += String(temperature[16], 0);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr>"
		"<td>ТТК подача</td>"
		"<td>");
		htmlPage += String(temperature[3], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr>"
		"<td>ТТК обратка</td>"
		"<td>");
		htmlPage += String(temperature[4], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr style='background:#DDD'>"
		"<td>Теплоакк.верх</td>"
		"<td>");
		htmlPage += String(temperature[10], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr style='background:#DDD'>"
		"<td>Теплоакк.низ</td>"
		"<td>");
		htmlPage += String(temperature[11], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr>"
		"<td>Система подача</td>"
		"<td>");
		htmlPage += String(temperature[0], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr>"
		"<td>Система обратка</td>"
		"<td>");
		htmlPage += String(temperature[1], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr style='background:#DDD'>"
		"<td>Электрокотел подача</td>"
		"<td>");
		htmlPage += String(temperature[14], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr style='background:#DDD'>"
		"<td>Электрокотел обратка</td>"
		"<td>");
		htmlPage += String(temperature[15], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr>"
		"<td>Зал</td>"
		"<td>");
		htmlPage += String(temperature[12], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr>"
		"<td>Большая спальня</td>"
			"<td>--.-</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"<tr>"
		"<td>Улица</td>"
		"<td>");
		htmlPage += String(temperature[13], 1);
		htmlPage += F("</td>"
		"<td></td>"
		"<td></td>"
		"</tr>"
		"</tbody>"
		"</table>"
		"</form>"

		"<form style='position:absolute; top:80px;left:360px; height:500px; width:600px'>"
		"<table cellspacing='0' cellpadding='5'>"
		"<caption>Heating system parameters</caption>"
		"<thead>"
		"<th>Parametr</th>"
		"<th>Value</th>"
		"<th colspan='2'>Set new value</th>"
		"<th></th>"
		"<th></th>"
		"</thead>"
		"<tbody>"
		"<tr style='background:#DDD'>"
		"<td>Boiler pump mode</td>"
		"<td>");
		htmlPage += sysParamString[sysParam[0]];
		htmlPage += F("</td>"
		"<td><select name='BoilerPumpMode' class='listGray' required>"
			"<option>Auto</option>"
			"<option>On</option>"
			"<option>Off</option>"
		"</td>"
		"<td><button formaction='/BoilerPumpMode'>Send</button></td>"
		"</tr>"
		"<tr style = 'background:#DDD'>"
		"<td>System pump mode</td>"
		"<td>");
		htmlPage += sysParamString[sysParam[1]];
		htmlPage += F("</td>"
		"<td><select name='SystemPumpMode'class='listGray' required>"
			"<option>Auto</option>"
			"<option>On</option>"
			"<option>Off</option>"
		"</td>"
		"<td><button formaction='/SystemPumpMode'>Send</button></td>"
		"</tr>"
		"<tr style='background:#DDD'>"
		"<td>System T regulation mode</td>"
		"<td>");
		htmlPage += sysParamString[sysParam[2]];
		htmlPage += F("</td>"
		"<td><select name='SysTempControlMode' class='listGray' required>"
			"<option>MyAlg</option>"
		"<option>PID</option>"
		"</td>"
		"<td><button formaction='/SysTempControlMode'>Send</button></td>"
		"</tr>"
		"<tr>"
		"<td>Door air intake in stove</td>"
		"<td>");
		htmlPage += sysParamString[sysParam[3]];
		htmlPage += F("</td>"
		"<td><select name='DoorAir' style='width:60px' required>"
		"<option>Auto</option>"
		"<option>Open</option>"
		"<option>Close</option>"
		"</td>"
		"<td><button formaction='/DoorAir'>Send</button></td>"
		"</tr>"
		"<tr>"
		"<td>Parametr 1</td>"
		"<td>");
		htmlPage += sysParamString[sysParam[4]];
		htmlPage += F("</td>"
		"<td><select name='Parametr1' style='width:60px' required>"
		"<option>1</option>"
		"<option>2</option>"
		"<option>3</option>"
		"</td>"
		"<td><button formaction='/Parametr1'>Send</button></td>"
		"</tr>"
		"<tr>"
		"<td>Parametr 2</td>"
		"<td>");
		htmlPage += sysParamString[sysParam[5]];
		htmlPage += F("</td>"
			"<td><select name='Parametr2' style='width:60px' required>"
			"<option>1</option>"
			"<option>2</option>"
			"<option>3</option>"
			"</td>"
			"<td><button formaction='/Parametr2'>Send</button></td>"
			"</tr>"
			"<tr>"
			"<td><button formaction='/AutoRefresh'>");
			refreshPage ? htmlPage += "Stop": htmlPage +="Start";
			htmlPage += F(" auto refresh page</button></td>"
		"</tr>"

		"</tbody>"
		"</table>"
		"</form>"
		"<iframe width='450' height='260' style='position:absolute;top:10px;left:750px;border:1px solid #cccccc;' src='https://thingspeak.com/channels/1287359/charts/7?bgcolor=%23ffffff&color=%23d62020&days=1&dynamic=true&title=%D0%94%D1%8B%D0%BC%D0%BE%D0%B2%D1%8B%D0%B5+%D0%B3%D0%B0%D0%B7%D1%8B&type=line&xaxis=%D0%92%D1%80%D0%B5%D0%BC%D1%8F%2C+%D1%87&yaxis=%D0%A2%D0%B5%D0%BC%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80%D0%B0%2C+%C2%B0%D0%A1'></iframe>"
		//"<iframe width='450' height='260' style='position:absolute;top:280px;left:750px;border:1px solid #cccccc;' src='https://thingspeak.com/channels/1287359/charts/1?bgcolor=%23ffffff&color=%23d62020&days=1&dynamic=true&title=%D0%9F%D0%BE%D0%B4%D0%B0%D1%87%D0%B0+%D0%A2%D0%A2+%D0%BA%D0%BE%D1%82%D0%B5%D0%BB&type=line&xaxis=%D0%92%D1%80%D0%B5%D0%BC%D1%8F%2C+%D1%87&yaxis=%D0%A2%D0%B5%D0%BC%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80%D0%B0%2C+%C2%B0%D0%A1&yaxismax=80&yaxismin=25'></iframe>"
		//"<iframe width='450' height='260' style='position:absolute;top:550px;left:750px;border:1px solid #cccccc;' src ='https://thingspeak.com/channels/1287359/charts/3?bgcolor=%23ffffff&color=%23d62020&days=1&dynamic=true&title=%D0%92%D0%B5%D1%80%D1%85+%D1%82%D0%B5%D0%BF%D0%BB%D0%BE%D0%B0%D0%BA%D0%BA%D1%83%D0%BC%D1%83%D0%BB%D1%8F%D1%82%D0%BE%D1%80%D0%B0&type=line&xaxis=%D0%92%D1%80%D0%B5%D0%BC%D1%8F%2C+%D1%87&yaxis=%D0%A2%D0%B5%D0%BC%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80%D0%B0%2C+%C2%B0%D0%A1&yaxismax=60&yaxismin=25'></iframe>"
		"<iframe width='450' height='260' style='position:absolute;top:280px;left:750px;border:1px solid #cccccc;' src='https://thingspeak.com/channels/1287359/charts/2?bgcolor=%23ffffff&color=%23d62020&days=2&dynamic=true&results=80&title=%D0%9F%D0%BE%D0%B4%D0%B0%D1%87%D0%B0+%D1%81%D0%B8%D1%81%D1%82%D0%B5%D0%BC%D0%B0&type=line&xaxis=%D0%92%D1%80%D0%B5%D0%BC%D1%8F%2C+%D1%87&yaxis=%D0%A2%D0%B5%D0%BC%D0%BF%D0%B5%D1%80%D0%B0%D1%82%D1%83%D1%80%D0%B0%2C+%C2%B0%D0%A1&yaxismax=60&yaxismin=25'></iframe>"
		"</body>"
		"</html>");
			///Serial.println("STOP");
			Serial.print("Length HTML string = "); Serial.println(htmlPage.length());
	return htmlPage;
}

/*
"<div id='container' style='position:absolute;top:80px;left:730px;height:500px;width:600px'></div>"
"<script src='https://cdn.anychart.com/js/latest/anychart-bundle.min.js'></script>"
"<script>"
"anychart.onDocumentLoad(function(){"
"var data=[[1,10],[2,12],[3,18],[4,11],[5,9],[6,10],[7,12],[8,18],[9,11],[10,9]];"
"chart=anychart.line();"
"var series=chart.line(data);"
"chart.title('AnyChart Basic Sample');"
"chart.container('container');"
"chart.draw();});"
"</script>"
*/
