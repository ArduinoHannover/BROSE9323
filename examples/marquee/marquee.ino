#include <BROSE9323.h>

BROSE9323 display(112, 16, 28);

/*
 * UMLAUT:
 * Ä \x8E
 * ä \x84
 * Ö \x99
 * ö \x94
 * Ü \x9A
 * ü \x81
 * ß \xE0
 */
const char text[] = "This is a test. Testing \x9Aml\x8Aute. ";
const uint8_t textsize = 2;

void setup() {
  display.begin();
  delay(2000);
  display.fillScreen(0);
  display.setTextSize(textsize);
  display.setTextWrap(false);
  display.setTextColor(1, 0);
  //display.setDirect(true); //when using setTextColor(1, 0) we don't need a separate display() and the text scrolls more evenly (on *328*)
}

void loop() {
	for (int16_t x = display.width(); x > -((int16_t) strlen(text) * textsize * 6); x--) {
		display.setCursor(x, 0);
		display.print(text);
		display.display();
	}
}