# BROSE9323 Library

Library for use with BROSE9323 based FlipDot Controller Boards.

## Connection

Remove the Siemens PLCC.

On the left side of the PLCC (where the big flat band cable is) are 1206 resistors you can solder your wires on.

Power can be taken from the 7805 at the top (upside down, so left leg is 5V, middle is GND).

Use a Arduino Nano (or similar with ATmega328/168[P][B]).

Solder wires to the resistors to the following pins (counting from left)

< FlipDot connection cable - D3 D4 D5 D6 D7 D8 D9 D10 D11 D12 D13 A0 A1 A2 A3 A4 A5 - PLCC >

## Software

If you use a 168 there is only a single buffer available due to memory.
With an 328 you have two buffers available, so if you call `display()`, only changed pixels will be updated.
With an 168 all pixels will be set.

You have the option to activate direct write, without using `display()`, so if any pixel is changed, it will be updated immeadiatly on the display.
To activate this use `setDirect(true)`.

Debugging is available using `printBuffer()`, which writes the buffer to be displayed to your console (graphically).

Constructor is `width, height, panelwidth, fliptime` - `fliptime` is optional and defaults to 280 µs.
Longer time may help some dots to flip, but slowes down the whole display routine.
Very high durations may lead to permanent damage (coil burnout).
Height is limited by regular connector to 21px.
Panelwidth is limited by FP2800 to 28px.
Overall width is limited to 8 Panels, 28px each, so 224px.

Otherwise it's a regular [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) based display driver.