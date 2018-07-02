# Circuit Playground MIDI Controller Multi-Tool

## Version 1.0.0

This sketch will turn your your [Circuit Playground](https://amzn.to/2tMr8Iw) (or [Circuit Playground Express](https://amzn.to/2lLNpBQ)) into a somewhat elaborate MIDI controller. You can upload this to your device using the [Arduino IDE](http://www.arduino.cc/). It was written for my [hands-on MIDI workshops](http://midi.mand.is/) to allow attendees to experiment with the [WebMIDI API](https://www.w3.org/TR/webmidi/). The multitidue of fun, built-in sensors make it an ideal tool for exploring non-musical applications for MIDI.

It has 10 different "modes" that can be enabled including:

- **Mode 1**: Converts capactive inputs to MIDI messages
- **Mode 2**: Converts the onbaord light sensor to MIDI messages
- **Mode 3**: Converts the onboard sound sensor to MIDI messages
- **Mode 4**: Relays the temperature (in celsius) via a MIDI message
- **Mode 5**: Sends a cascade of random MIDI messages with variable speed
- **Mode 6**: Converts the accelerometer X/Y/Z parameters to MIDI messages
- **Mode 7**: Converts the accelerometer "tap" detetion to a MIDI message
- **Mode 8**: Plays incoming MIDI notes over the internal speaker
- **Mode 9**: Play random notes over the internal speaker (non-MIDI mode)
- **Mode 10**: Mix RGB colors with the onboard LEDs using incoming MIDI messages

## How to Upload

- Download the [Arduino IDE](http://www.arduino.cc/)
- If Adafruit Circuit Playground is not listed among the installed boards find and install it via  **Tools > Board > Boards Manager**
- Go to **Sketch > Include Library > Manage Libraries** and install  the [MIDIUSB library](https://www.arduino.cc/en/Reference/MIDIUSB).
- Select the correct port for your device and the appropriate board (Either *Adafruit Circuit Playground* or *Adafruit Circuit Playground Express*)
- Open the sketch in this repository and upload it to your device
- Have fun hacking away!

Use this CodePen to test if the device is now being recognized as MIDI controller:

[codepen.io/georgemandis/pen/XqYNNN](https://codepen.io/georgemandis/pen/XqYNNN)

## How to Use

- To change or view the current mode your Circuit Playground is in toggle the slide switch so that one or more NeoPixel LEDs lights up and stays on.

- With the slide switch toggled you should be able to change modes by pressing the left and right buttons on your device. As the mode increases or decreases the ring of NeoPixels will light up accordingly and a pitch should play:

- To begin utilizing the mode you've selected toggle the switch back to its original state. Your device should begin functioning per your selected mode as described above
