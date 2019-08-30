#include <digitalWriteFast.h> // use https://github.com/NicksonYap/digitalWriteFast for speedup

#include <BROSE9323.h>

BROSE9323 display(112, 16, 28);

void setup() {
	Serial.begin(38400); //Use 38400 for bitmaps, otherwise 115200 is okay.
	display.begin();
	delay(1000);
	
	display.fillScreen(1);
	uint32_t t1 = millis();
	display.fillScreen(0);
	uint32_t t2 = millis();
	Serial.println(t2 - t1);
}

void loop() {
	if (Serial.available()) {
		char cmd = Serial.read();
		switch (cmd) {
			// Ignore CR
			case 0x0A:
				break;
			// Draw Bitmap (must be n x 8 wide)
			case 'b': case 'B': //BxxyywwhhcBITMAP (xx/ww = 8 bit hex x position/width; yy/hh = 8 bit y position/height; c = 0/1 color; BITMAP = bytes as hex)
				{
					while (Serial.available() < 9) {
						delay(1);
					}
					uint8_t x = (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read());
					uint8_t y = (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read());
					uint8_t w = (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read());
					if (w % 4) {
						Serial.println("ERR: Invalid bitmap width (not dividable by 4)");
						Serial.flush();
						break;
					}
					uint8_t h = (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read());
					if (h == 0) {
						Serial.println("ERR: Invalid bitmap height (0)");
						Serial.flush();
						break;
					}
					uint8_t c = Serial.read() - '0';
					uint8_t b = 0;
					for (uint8_t _y = 0; _y < h; _y++) {
						for (uint8_t _x = 0; _x < w; _x++) {
							if ((_x & 0x3) == 0) {
								if (!Serial.available()) {
									Serial.println("WARN: OUTOFMEM");
								}
								while (!Serial.available()) {
									delay(1);
								}
								b = hex2dec(Serial.read());
							}
							if (b & (1 << (0x3 - (_x & 0x3)))) {
								display.drawPixel(x + _x, y + _y, c);
							}
						}
					}
					break;
				}
			// Display (only works on ATmega328)
			case 'd': case 'D': //D
				{
					display.display();
					break;
				}
			// Fill Screen
			case 'f': case 'F': //Fc (c = 0/1 color)
				{
					while (Serial.available() <= 2) {
						delay(1);
					}
					display.fillScreen(Serial.read() - '0');
					break;
				}
			// Print Text
			case 'p': case 'P': // PxxyycsTEXT (xx = 8 bit hex x position; yy = 8 bit y position; c = 0/1 color; s = 1/2 size; TEXT = string, newline = escaped \\n)
				{
					delay(5);
					while (Serial.available() <= 6) {
						delay(1);
					}
					display.setCursor((hex2dec(Serial.read()) << 4) | hex2dec(Serial.read()), (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read()));
					display.setTextColor(Serial.read() - '0');
					display.setTextSize(Serial.read() - '0');
					while (true) {
						if (!Serial.available()) {
							delay(1);
						}
						if (Serial.peek() == '\n') break;
						char c = Serial.read();
						if (c == '\\' && Serial.peek() == 'n') {
							display.println();
							Serial.read();
						} else {
							display.write(c);
						}
					}
					return;
				}
			// Return buffer
			case 'r': case 'R': //R
				{
					display.printBuffer();
					Serial.flush();
					break;
				}
			// Set Pixel
			case 's': case 'S': // Sxxyc (xx = 8 bit hex x position; y = 4 bit y position; c = 0/1 color)
				{
					while (Serial.available() <= 6) {
						delay(1);
					}
					display.drawPixel((hex2dec(Serial.read()) << 4) | hex2dec(Serial.read()),
					                  (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read()),
					                   hex2dec(Serial.read())
					                 );
					break;
				}
			default:
				Serial.print("0x");
				Serial.print(cmd >> 4, HEX);
				Serial.print(cmd & 3, HEX);
				Serial.println("Command not known.");
				Serial.flush();
		}
		Serial.readStringUntil('\n');
	}
}

uint8_t hex2dec(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return 0;
}
