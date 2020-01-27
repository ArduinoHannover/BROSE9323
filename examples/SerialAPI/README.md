# Serial relay API

You may use this program (or a modified one) to control your display via computer or wifi (use ESP_Serial.ino for ESP8266).

## Functions

Each command has to be terminated by `\n` newline. `\r` carriage return will be ignored.

### Fill screen

`f0`/`F0` to fill black or `f1`/`F1` to fill yellow.

### Set pixel

`sxxyyc` set pixel on position hex {`xx`,`yy`} to color `c` (0/1).
To set pixel {16,11} to yellow use `s100B1` (x = 16<sub>dec</sub> / 10<sub>hex</sub>, y = 11<sub>dec</sub> / 0B<sub>hex</sub>, c = 1)

If you want to set a pixel directly to the desired color without calling display use uppercase `S` (`S100B1`).

### Display

`d`/`D` refreshes the display content.

### Print text

`pxxyycsTEXT` print text at position hex {`xx`,`yy`} with color `c` (0/1) and size `s` (1/2).
`p` can be either upper or lower case.

So to write "Test" in yellow at {2,1} in big font use `p020112Test` (x = 2<sub>dec</sub> / 02<sub>hex</sub>, y = 1<sub>dec</sub> / 01<sub>hex</sub>, c = 1, s = 2)

### Display content

Just write `d`/ `D` to update display content.

### Show current buffer content

Just for debugging purposes you can send `r`/`R` which will print a bitmap to the console.

### Update timing

If you want to alter the timing send `txxx` where `xxx` is the flip time in decimal microseconds.
So `t300` will be 300µs.
`t` can be either upper or lower case.

### Bitmaps

You can send a bitmap with `bxxyywwhhcBITMAP` where `xx`/`yy` are position x/y in hex and `ww`/`hh` width and height also as hex.
With has to be a multiple of 8.
Bitmap is hex encoded binary.
So `b000008021AA55` will show the following (`*` is yellow set by function) at position {0,0}:
```
* * * * 
 * * * *
 ```
 Due to the low buffersize of the Arduino your milage may vary.
 Small images will transfer okay, while updating the complete display will probably fail.
`b` can be either upper or lower case.
