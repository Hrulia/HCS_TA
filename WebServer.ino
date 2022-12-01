// 26.11.2022
// Функционал web-сервера для ESP_HCS_TA
//info: 
//++ https://www.joyta.ru/12628-sozdanie-prostogo-veb-servera-nodemcu-esp8266-v-arduino-ide/
// https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/server-examples.html
// https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
// 

#include <ESP8266WebServer.h>

String webPage = ""; 
uint16_t servPort = 80;
//WiFiServer server(80); //arduino, поддерживает несколько клиентов
ESP8266WebServer webServer(servPort); //only support one simultaneous client


	//web server starting
void initWebServer() {
	webServer.begin();
	Serial.println("HTTP web-server started on port "+String(servPort));

	//installing handlers for the request html web clients
	webServer.on("/", handle_OnConnect);
	webServer.on("/BoilerPumpMode", handle_BoilerPumpMode);
	webServer.on("/SystemPumpMode", handle_SystemPumpMode);
	webServer.on("/SysTempControlMode", handle_SysTempControlMode);
	webServer.on("/DoorAir", handle_DoorAir);
	webServer.on("/Parametr1", handle_Parametr1);
	webServer.on("/Parametr2", handle_Parametr2);
	webServer.onNotFound( []() {webServer.send(404, "text/plain", "Not found");});
}

// Web-server listen for HTTP requests from clients
void checkWebClient() {
	webServer.handleClient();
}



void handle_OnConnect() {
	Serial.println("handle_OnConnect");
	webServer.send(200, "text/html", prepareHtmlPage(temperatures, true));
	delay(3000);
	webServer.send(404, "text/plain", "Not found");
}
void handle_BoilerPumpMode() {
	Serial.println("handle_BoilerPumpMode");
	webServer.send(200, "text/html", prepareHtmlPage(nullptr, true));
}
void handle_SystemPumpMode() {
	Serial.println("handle_SystemPumpMode");
	webServer.send(200, "text/html", prepareHtmlPage(false, true));
}
void handle_SysTempControlMode() {
	Serial.println("handle_SysTempControlMode");
	webServer.send(200, "text/html", prepareHtmlPage(nullptr, true));
}
void handle_DoorAir() {
	Serial.println("handle_DoorAir");
	webServer.send(200, "text/html", prepareHtmlPage(nullptr, false));
}
void handle_Parametr1() {
	Serial.println("handle_Parametr1");
	webServer.send(200, "text/html", prepareHtmlPage(nullptr, false));
}
void handle_Parametr2() {
	Serial.println("handle_Parametr2");
	webServer.send(200, "text/html", prepareHtmlPage(nullptr, false));
}

//containing the contents of the web page
String prepareHtmlPage(float temperature[], uint8_t led2stat){
	String htmlPage;
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
		"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"
		"<title>HCS Control(Tonshaevo)</title>"
		"<style>"
		".txtGray{text-align:right; width:50px; background:#DDD}"
		".txtWhite{text-align:right; width:50px;}"
		".listGray{width:60px; background:#DDD}"
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
		"<td></td>"
		"<td><select name='BoilerPumpMode' class='listGray' required>"
		"<option>Auto</option>"
		"<option>ON</option>"
		"<option>OFF</option>"
		"</td>"
		"<td><button formaction='/BoilerPumpMode'>Send</td>"
		"</tr>"
		"<tr style = 'background:#DDD'>"
		"<td>System pump mode</td>"
		"<td></td>"
		"<td><select name='SystemPumpMode'class='listGray' required>"
		"<option>Auto</option>"
		"<option>ON</option>"
		"<option>OFF</option>"
		"</td>"
		"<td><button formaction='/SystemPumpMode'>Send</td>"
		"</tr>"
		"<tr style='background:#DDD'>"
		"<td>System T regulation mode</td>"
		"<td></td>"
		"<td><select name='SysTempControlMode' class='listGray' required>"
		"<option>My</option>"
		"<option>PID</option>"
		"</td>"
		"<td><button formaction='/SysTempControlMode'>Send</td>"
		"</tr>"
		"<tr>"
		"<td>Door air intake in stove</td>"
		"<td></td>"
		"<td><select name='DoorAir' style='width:60px' required>"
		"<option>Auto</option>"
		"<option>Open</option>"
		"<option>Close</option>"
		"</td>"
		"<td><button formaction='/DoorAir'>Send</td>"
		"</tr>"
		"<tr>"
		"<td>Parametr 1</td>"
		"<td></td>"
		"<td><select name='Parametr1' style='width:60px' required>"
		"<option>1</option>"
		"<option>2</option>"
		"<option>3</option>"
		"</td>"
		"<td><button formaction='/Parametr1'>Send</td>"
		"</tr>"
		"<tr>"
		"<td>Parametr 2</td>"
		"<td></td>"
		"<td><select name='Parametr2' style='width:60px' required>"
		"<option>1</option>"
		"<option>2</option>"
		"<option>3</option>"
		"</td>"
		"<td><button formaction='/Parametr2'>Send</td>"
		"</tr>"
		"</tbody>"
		"</table>"
		"</form>"

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
		"</body>"
		"</html>");

	return htmlPage;
}
