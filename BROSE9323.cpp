#include <BROSE9323.h>

BROSE9323::BROSE9323(uint8_t w, uint8_t h, uint8_t pw, uint16_t ft) :
	Adafruit_GFX(w, h),
	_flip_time(ft),
	_panel_width(pw),
	_buffer_width((w + 7) / 8),
	_buffer_size(_buffer_width * h) {
	// Buffer width should fit at least [width] bits
	//_buffer_width = (w + 7) / 8;
	
	//_buffer_size = _buffer_width * h;

#if !defined(__AVR_ATmega168P__) && !defined(__AVR_ATmega168PB__) && !defined(__AVR_ATmega168__)
	_old_buffer = (uint8_t*) calloc(_buffer_size, sizeof(uint8_t));
#endif
	_new_buffer = (uint8_t*) calloc(_buffer_size, sizeof(uint8_t));
}

#ifdef ESP8266
void BROSE9323::begin(Stream* s) {
	stream = s;
	fillScreen(0);
	setCursor(0, 4);
	print("ESP Connected");
	display();
}
#else
void BROSE9323::begin(void) {
	digitalWrite(ADDR_0, 1);
	digitalWrite(ADDR_1, 1);
	digitalWrite(ADDR_2, 1);
	digitalWrite(COL_0, 1);
	digitalWrite(COL_1, 1);
	digitalWrite(COL_2, 1);
	digitalWrite(COL_3, 1);
	digitalWrite(COL_4, 1);
	digitalWrite(ROW_0, 1);
	digitalWrite(ROW_1, 1);
	digitalWrite(ROW_2, 1);
	digitalWrite(ROW_3, 1);
	digitalWrite(ROW_4, 1);
	digitalWrite(ROW_RESET, 1);
	digitalWrite(ROW_SET, 1);
	digitalWrite(COL_DATA, 1);
	digitalWrite(ENABLE, 1);
	pinMode(ADDR_0, OUTPUT);
	pinMode(ADDR_1, OUTPUT);
	pinMode(ADDR_2, OUTPUT);
	pinMode(COL_0, OUTPUT);
	pinMode(COL_1, OUTPUT);
	pinMode(COL_2, OUTPUT);
	pinMode(COL_3, OUTPUT);
	pinMode(COL_4, OUTPUT);
	pinMode(ROW_0, OUTPUT);
	pinMode(ROW_1, OUTPUT);
	pinMode(ROW_2, OUTPUT);
	pinMode(ROW_3, OUTPUT);
	pinMode(ROW_4, OUTPUT);
	pinMode(ROW_RESET, OUTPUT);
	pinMode(ROW_SET, OUTPUT);
	pinMode(COL_DATA, OUTPUT);
	pinMode(ENABLE, OUTPUT);
	_setData(1);
	_setData(0);
	fillScreen(1);
	display();
	fillScreen(0);
	drawBitmap((width() - 72) / 2, (height() - 16) / 2, _hannio_splash, 72, 16, 1);
	display();
}
#endif

void BROSE9323::display(bool force) {
#ifdef ESP8266
	stream->print("D\n");
	stream->flush();
#else
	if (_direct_mode) return;
	for (uint8_t x = 0; x < width(); x++) {
		_selectPanel(x / _panel_width);
		
		_selectColumn(x % _panel_width);
		
		for (uint8_t y = 0; y < height(); y++) {
			bool b = _new_buffer[y * _buffer_width + x / 8] & (1 << (x & 7));
#if !defined(__AVR_ATmega168P__) && !defined(__AVR_ATmega168PB__) && !defined(__AVR_ATmega168__)
			if (!force && (bool)(_old_buffer[y * _buffer_width + x / 8] & (1 << (x & 7))) == b) {
				continue;
			}
#endif
			
			_selectRow(y);
			
			_setData(_new_buffer[y * _buffer_width + x / 8] & (1 << (x & 7)));
			
			_strobe();
		}
	}
#if !defined(__AVR_ATmega168P__) && !defined(__AVR_ATmega168PB__) && !defined(__AVR_ATmega168__)
	// Store currently displayed content in old buffer
	memcpy(_old_buffer, _new_buffer, _buffer_size);
#endif
#endif
}

void BROSE9323::drawPixel(int16_t x, int16_t y, uint16_t color) {
#ifdef ESP8266
	char s[10];
	sprintf(s, "%c%02x%02x%d\n", _direct_mode ? 'S' : 's', x, y, (bool)color);
	stream->print(s);
	stream->flush();
#else
	if (y >= height() || y < 0 || x >= width() || x < 0) return;
	if (color) {
		if (_new_buffer[y * _buffer_width + x / 8] & (1 << (x & 7))) {
			return;
		}
		_new_buffer[y * _buffer_width + x / 8] |= 1 << (x & 7);
	} else {
		if (~_new_buffer[y * _buffer_width + x / 8] & (1 << (x & 7))) {
			return;
		}
		_new_buffer[y * _buffer_width + x / 8] &= ~(1 << (x & 7));
	}
	if (_direct_mode) {
		_selectPanel(x / _panel_width);
	
		_selectColumn(x % _panel_width);
	
		_selectRow(y);
	
		_setData(_new_buffer[y * _buffer_width + x / 8] & (1 << (x & 7)));
	
		_strobe();
#if !defined(__AVR_ATmega168P__) && !defined(__AVR_ATmega168PB__) && !defined(__AVR_ATmega168__)
		if (color) {
			if (_new_buffer[y * _buffer_width + x / 8] & (1 << (x & 7))) {
				return;
			}
			_new_buffer[y * _buffer_width + x / 8] |= 1 << (x & 7);
		} else {
			if (~_new_buffer[y * _buffer_width + x / 8] & (1 << (x & 7))) {
				return;
			}
			_new_buffer[y * _buffer_width + x / 8] &= ~(1 << (x & 7));
		}
#endif
	}
#endif
}

void BROSE9323::fillScreen(uint16_t color) {
#ifdef ESP8266
	stream->print(color ? "F1\n" : "F0\n");
#else
	memset(_new_buffer, color ? 0xFF : 0x00, _buffer_size);
	if (_direct_mode) {
		_setData(color);
		for (uint8_t x = 0; x < width(); x++) {
			if (x % _panel_width == 0) {
				_selectPanel(x / _panel_width);
			}
		
			_selectColumn(x % _panel_width);
		
			for (uint8_t y = 0; y < height(); y++) {
				_selectRow(y);
			
				_strobe();
			}
		}
#if !defined(__AVR_ATmega168P__) && !defined(__AVR_ATmega168PB__) && !defined(__AVR_ATmega168__)
		memset(_old_buffer, color ? 0xFF : 0x00, _buffer_size);
#endif
	}
#endif
}

void BROSE9323::setTiming(uint16_t t) {
#ifdef ESP8266
	stream->write('T');
	stream->print(t);
	stream->write('\n');
#else
	_flip_time = t;
#endif
}

void BROSE9323::setDirect(bool d) {
	_direct_mode = d;
}

#ifndef ESP8266
void BROSE9323::printBuffer(void) {
	for (uint8_t y = 0; y < height(); y++) {
		for (uint8_t x = 0; x < width(); x++) {
			Serial.write(_new_buffer[y * _buffer_width + x / 8] & (1 << (x & 7)) ? '*' : ' ');
		}
		Serial.println();
	}
}

void BROSE9323::_selectColumn(uint8_t col) {
	if (_active_col == col) return;
	_active_col = col;
	col += 1 + col / 7;
	digitalWrite(COL_0, col &  1);
	digitalWrite(COL_1, col &  2);
	digitalWrite(COL_2, col &  4);
	digitalWrite(COL_3, col &  8);
	digitalWrite(COL_4, col & 16);
}

void BROSE9323::_selectPanel(uint8_t panel) {
	if (_active_panel == panel) return;
	_active_panel = panel;
	digitalWrite(ADDR_0, panel & 1);
	digitalWrite(ADDR_1, panel & 2);
	digitalWrite(ADDR_2, panel & 4);
}

void BROSE9323::_selectRow(uint8_t row) {
	if (_active_row == row) return;
	_active_row = row;
	row += row / 7;
	digitalWrite(ROW_0, row &  1);
	digitalWrite(ROW_1, row &  2);
	digitalWrite(ROW_2, row &  4);
	digitalWrite(ROW_3, row &  8);
	digitalWrite(ROW_4, row & 16);
}

void BROSE9323::_setData(bool data) {
	if (_active_data == data) return;
	_active_data = data;
	if (data) {
		digitalWrite(ROW_RESET, 1);
		digitalWrite(ROW_SET,   0);
		digitalWrite(COL_DATA,  0);
	} else {
		digitalWrite(ROW_SET,   1);
		digitalWrite(ROW_RESET, 0);
		digitalWrite(COL_DATA,  1);
	}
}

void BROSE9323::_strobe(void) {
	digitalWrite(ENABLE, 0);
	delayMicroseconds(_flip_time);
	digitalWrite(ENABLE, 1);
}
#endif