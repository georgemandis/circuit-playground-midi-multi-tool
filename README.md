# Circuit Playground MIDI Controller Multi-Tool

## Version 1.0.1

This sketch turns a [Circuit Playground](https://www.adafruit.com/product/3000) Classic or
[Express](https://www.adafruit.com/product/3333) into a ten-mode MIDI controller. Upload it with the
[Arduino IDE](http://www.arduino.cc/) or arduino-cli. It was written for my
[hands-on MIDI workshops](http://midi.mand.is/) so attendees could try the
[Web MIDI API](https://www.w3.org/TR/webmidi/) against a board full of sensors, which makes it a good
tool for non-musical MIDI.

One sensor is on duty at a time. Everything the board sends is on channel 2 (status `0x91` for notes,
`0xB1` for control changes); the colour mixer listens on channels 1 to 3.

| Mode | Pixels lit | What it does |
|---|---|---|
| 1 | 1 | Cap touch. Each of the eight pads is a note: note on, velocity 127, when touched, note off on release. Note = 1 + the pad's pin number (table below). The pixels flash a random colour on each touch. The buttons nudge the touch sensitivity up and down. |
| 2 | 2 | Light sensor. CC 1 with the level scaled to 0..127, once a second. |
| 3 | 3 | Sound sensor. CC 1 with the level scaled to 0..127, once a second. |
| 4 | 4 | Temperature. CC 1 with degrees Celsius as the value, once a second. |
| 5 | 5 | Random notes. A note on with a random note and velocity every 50 ms. Right button speeds up, left slows down. No note offs. |
| 6 | 6 | Accelerometer. Three note ons every 200 ms, x then y then z, note = m/s² + 20, velocity 127. 20 is zero g, about 30 is one g. |
| 7 | 7 | Tap. A tap flashes all ten pixels a random colour and prints to the serial port. **Sends no MIDI.** The send is in the code but commented out; see below. |
| 8 | 8 | Speaker. Plays any incoming note on, any channel, through the built-in speaker at the note's pitch for 50 ms. |
| 9 | 9 | Random tones and random pixel flashes. No MIDI in or out. |
| 10 | 10 | Colour mixer. An incoming note on sets one colour on all ten pixels: channel 1 red, channel 2 green, channel 3 blue, each to note + velocity (0..254). |

Touch pads, pin to note:

| pin | 3 | 2 | 0 | 1 | 12 | 6 | 9 | 10 |
|---|---|---|---|---|---|---|---|---|
| note | 4 | 3 | 1 | 2 | 13 | 7 | 10 | 11 |

### Mode 7

Earlier versions of this README said mode 7 "converts the accelerometer tap detection to a MIDI
message". It does not. `setup()` arms the tap detector and attaches an interrupt, and in mode 7 the
interrupt flashes the pixels and sets a flag, but the block in `case 6` that would send
`noteOn(2, 1, 127)` is commented out under "Accelerometer detect tap: TBD". To get a note from a tap,
uncomment that block, or use a touch pad in mode 1.

## How to upload

### Arduino IDE

- Download the [Arduino IDE](http://www.arduino.cc/).
- Tools > Board > Boards Manager: "Arduino AVR Boards" includes the Circuit Playground Classic. For
  the Express add `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json` under
  Preferences > Additional boards manager URLs, then install "Adafruit SAMD Boards".
- Sketch > Include Library > Manage Libraries: install
  [MIDIUSB](https://www.arduino.cc/en/Reference/MIDIUSB) and "Adafruit Circuit Playground".
- Pick the board (Adafruit Circuit Playground, or Adafruit Circuit Playground Express) and its port,
  open the sketch, Upload.

### arduino-cli

```sh
arduino-cli core install arduino:avr
arduino-cli lib install MIDIUSB "Adafruit Circuit Playground"
arduino-cli compile --fqbn arduino:avr:circuitplay32u4cat .
arduino-cli upload  --fqbn arduino:avr:circuitplay32u4cat -p /dev/cu.usbmodem14101 .
```

For the Express: `arduino-cli core install adafruit:samd` after adding the Adafruit index URL, then
`--fqbn adafruit:samd:adafruit_circuitplayground_m0`.

Check that the board shows up as a MIDI device with this CodePen:
[codepen.io/georgemandis/pen/XqYNNN](https://codepen.io/georgemandis/pen/XqYNNN), or with the
Explorer at [midi.mand.is](https://midi.mand.is).

## How to use

- To see or change the mode, flip the slide switch. One or more NeoPixels light and stay lit: the
  count is the mode number.
- With the switch flipped, the right button goes up a mode and the left button down. A tone plays on
  each press.
- Flip the switch back. The mode starts. The buttons and the switch never send MIDI themselves.
