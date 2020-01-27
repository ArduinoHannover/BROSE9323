#include <BROSE9323.h>

BROSE9323 display(112, 16, 28);

uint32_t refresh_at;
const uint32_t REFRESH_TIME = 120000L;

//#define DEBUG
void setup() {
	Serial.begin(115200);
	display.begin();
	delay(1000);
	
	display.fillScreen(1);
	display.display();
	display.fillScreen(0);
	uint32_t t1 = millis();
	display.display();
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
					// May lead to buffer overflow, you may need to segment the bitmap into multiple frames
					// Or just use the ser pixel function
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
			// Display
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
					uint8_t x = (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read());
					uint8_t y = (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read());
					uint8_t c = Serial.read() - '0';
					display.setCursor(x, y);
					display.setTextColor(c, !c);
					display.setTextSize(Serial.read() - '0');
					while (true) {
						if (!Serial.available()) {
							delay(1);
						}
						if (Serial.peek() == '\n') break;
						if (Serial.peek() == '\r') {Serial.read();continue;};
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
			// Return buffer (show pixels on console)
			case 'r': case 'R': //R
				{
					display.printBuffer();
					Serial.flush();
					break;
				}
			// Set Pixel
			case 'S':
				display.setDirect(true);
			case 's': // Sxxyyc (xx = 8 bit hex x position; y = 8 bit y position; c = 0/1 color)
				{
					while (Serial.available() <= 6) {
						delay(1);
					}
					uint8_t x = (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read());
					uint8_t y = (hex2dec(Serial.read()) << 4) | hex2dec(Serial.read());
					bool color = hex2dec(Serial.read());
					display.drawPixel(x, y, color);
					display.setDirect(false);
					break;
				}
			// Set Timing
			case 'T': case 't': // Tttt (ttt = decimal timing in us)
				{
					while (Serial.available() <= 6) {
						delay(1);
					}
					uint16_t t = atoi(Serial.readStringUntil('\n').c_str());
					display.setTiming(t);
					break;
				}
			default:
#ifdef DEBUG
				Serial.print("0x");
				Serial.print(cmd >> 4, HEX);
				Serial.print(cmd & 3, HEX);
				Serial.println(" - command not known.");
				Serial.flush();
#endif
				break;
		}
	}
	// Refresh content every REFRESH_TIME (by default 2 min)
	if (refresh_at < millis()) {
		refresh_at = millis() + REFRESH_TIME;
		display.display(true);
	}
}

uint8_t hex2dec(char c) {
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	return 0;
}