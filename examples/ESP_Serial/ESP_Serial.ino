//#define USE_LED

#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <BROSE9323.h>
#ifdef USE_LED
#include <Adafruit_NeoPixel.h>
#endif

const char* ssid = "SSID";
const char* password = "password";

#ifdef USE_LED
//You can replace the neon tube by a LED Stripe powered by the ESP
//You may better want to use NeoPixelBus with DMA on GPIO 3 (RX)
Adafruit_NeoPixel strip(72, 2, NEO_GRBW + NEO_KHZ800);
#endif
BROSE9323 display(112, 16, 28, 300);

uint32_t next_display;

void setup() {
	Serial.begin(115200);
#ifdef USE_LED
	strip.begin();
	strip.show();
#endif
	delay(4000);
#ifdef USE_LED
	strip.setBrightness(50);
	for (uint8_t i = 0; i < strip.numPixels(); i++) {
		strip.setPixelColor(i, 0, 0, 0, 255);
	}
	strip.show();
#endif
	display.begin(&Serial);
	delay(4000);
	display.fillScreen(0);
	display.setCursor(0, 0);
	display.println("Connecting...");
	display.display();
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		display.drawPixel(0, 0, (millis() / 200) & 1);
		delay(1);
	}
	ArduinoOTA.setPort(8266);
	ArduinoOTA.setPassword((const char *)"iliketrains");
	ArduinoOTA.setHostname("BROSE9323");
	ArduinoOTA.onStart([](){
		display.fillScreen(0);
		display.setTextColor(1);
		display.setCursor((display.width() - 3 * 12) / 2, 2);
		display.setTextSize(2);
		display.print("OTA");
		display.display();
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		if (progress == total) {
			display.fillScreen(0);
			display.setTextColor(1);
			display.setCursor((display.width() - 2 * 12) / 2, 2);
			display.setTextSize(2);
			display.print("OK");
			display.display();
		} else {
			uint8_t f = progress*display.width()/total;
			display.setDirect(true);
			display.drawPixel(f, 0, 1);
			display.setDirect(false);
		}
	});
	ArduinoOTA.onError([](ota_error_t error) {
		display.fillScreen(1);
		display.setTextColor(0);
		display.setCursor((display.width() - 3 * 12) / 2, 2);
		display.setTextSize(2);
		display.print("OTA");
		if (error == OTA_AUTH_ERROR) {
			display.print("AUT");
		} else if (error == OTA_BEGIN_ERROR) {
			display.print("BEG");
		} else if (error == OTA_CONNECT_ERROR) {
			display.print("CON");
		} else if (error == OTA_RECEIVE_ERROR) {
			display.print("REC");
		} else if (error == OTA_END_ERROR) {
			display.print("END");
		}
		display.display();
	});
	ArduinoOTA.begin();
	next_display = millis() / 1000 * 1000;
}

void loop() {
	ArduinoOTA.handle();
	if (next_display < millis()) {
		char t[20];
		sprintf(t, "%02d:%02d:%02d\0", millis() / 1000 / 60 / 60, (millis() / 1000 / 60) % 60, (millis() / 1000 % 60));
		display.fillScreen(0);
		display.setTextSize(2);
		display.setCursor(0, 0);
		display.print(t);
		display.display();
		next_display += 1000;
	}
}