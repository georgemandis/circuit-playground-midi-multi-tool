#include <frequencyToNote.h>
#include <MIDIUSB.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>

#include <Adafruit_CircuitPlayground.h>

uint8_t mode = 0;
boolean padTouched = false;
boolean previousPadStatus = false;
uint8_t pads[] = {3, 2, 0, 1, 12, 6, 9, 10};
uint8_t padValues[] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t numberOfPads = sizeof(pads) / sizeof(uint8_t);
int delaySpeed = 50;
boolean modeButtonPressed = false;
int capacitiveSensitivity = 100;
int capacitiveRead;
int randomRed = 0;
int randomGreen = 0;
int randomBlue = 0;

int red = 0;
int green = 0;
int blue = 0;

boolean tapDetected = false;

midiEventPacket_t rx;

void setup() {
  CircuitPlayground.begin();
  Serial.begin(9600);

  // for "tap" modes
  attachInterrupt(digitalPinToInterrupt(7), tapCallback, RISING);

  CircuitPlayground.setAccelRange(LIS3DH_RANGE_8_G);
  CircuitPlayground.setAccelTap(1, 100);
}

/* MIDI notes on/off
 *****************************************/


const char* pitch_name(byte pitch) {
  static const char* names[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
  return names[pitch % 12];
}

int pitch_octave(byte pitch) {
  return (pitch / 12) - 1;
}

void tapCallback() {
  if (mode == 6 {
  Serial.println(CircuitPlayground.getAccelTap());
    tapDetected = true;
    randomRed = random(0, 255);
    randomGreen = random(0, 255);
    randomBlue = random(0, 255);

    for (int x = 0; x < 10; x++) {
      CircuitPlayground.setPixelColor(x, randomRed, randomGreen, randomBlue);
    }
  }


}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

/* Indicate which "mode" we've put the device in
**************************************************/

void showMode() {
  CircuitPlayground.clearPixels();
  for (int i = 0; i <= mode; i++) {
    CircuitPlayground.setPixelColor(i, random(255), random(255), random(255));
  }
  delay(100);
}

void manageMode() {
  if (CircuitPlayground.rightButton() && !modeButtonPressed) {
    if (mode < 9) mode++;
    modeButtonPressed = true;
    CircuitPlayground.playTone(440 + (mode * 20), 100);
  }

  if (CircuitPlayground.leftButton() && !modeButtonPressed) {
    if (mode > 0) mode--;
    modeButtonPressed = true;
    CircuitPlayground.playTone(440 + (mode * 20), 100);
  }

  if (!CircuitPlayground.rightButton() && !CircuitPlayground.leftButton()) modeButtonPressed = false;
}

void loop() {

  /* Different behaviors depending on what "mode" the device is in.
  ******************************************************************/
  switch (mode) {
    // Capactive touch to MIDI mode
    case 0:
      //CircuitPlayground.clearPixels();
      padTouched = false;

      for (int i = 0; i < numberOfPads; i++) {
        capacitiveRead = CircuitPlayground.readCap(pads[i]);

        if (capacitiveRead > capacitiveSensitivity)  {
          if (padValues[i] != 1) {
            Serial.print(pads[i]);
            Serial.print(" - ");
            Serial.print(capacitiveRead);

            randomRed = random(0, 255);
            randomGreen = random(0, 255);
            randomBlue = random(0, 255);

            for (int x = 0; x < 10; x++) {
              CircuitPlayground.setPixelColor(x, randomRed, randomGreen, randomBlue);
            }

            noteOn(1, 1 + pads[i], 127 );
            MidiUSB.flush();
            padValues[i] = 1;
            Serial.println(" - Note on");
            delay(10);
          }
        } else {
          if (padValues[i] == 1) {
            Serial.print(pads[i]);
            Serial.print(" - ");
            Serial.print(capacitiveRead);

            for (int x = 0; x < 10; x++) {
              CircuitPlayground.setPixelColor(x, 0, 0, 0);
            }

            noteOff(1, 1 + pads[i], 0);
            MidiUSB.flush();
            padValues[i] = 0;
            Serial.println(" - Note off");
            delay(10);
          }
        }

        // calibrate sensitivity on the fly with on-device buttons
        if (CircuitPlayground.rightButton()) capacitiveSensitivity++;
        if (CircuitPlayground.leftButton()) capacitiveSensitivity--;

        if (CircuitPlayground.leftButton() || CircuitPlayground.rightButton()) {
          Serial.print("Updating capacitive sensitivity");
          Serial.println(capacitiveSensitivity);
        }
      }

      break;

    // Light sensor to MIDI controlchange message: channel 1, control 1
    case 1:
      Serial.println(CircuitPlayground.lightSensor());
      Serial.println(round((CircuitPlayground.lightSensor() / 1023.0) * 127));
      controlChange(1, 1, round((CircuitPlayground.lightSensor() / 1023.0) * 127));
      delay(1000);
      break;

    // Sound sensor to MIDI control change message: channel 1, control 1
    case 2:
      Serial.println(CircuitPlayground.soundSensor());
      Serial.println((CircuitPlayground.soundSensor() / 1023.0) * 127);
      controlChange(1, 1, round((CircuitPlayground.soundSensor() / 1023.0) * 127));
      delay(1000);
      break;

    // Temperature (celsius) to MIDI control change message: channel 1, control 1
    case 3:
      Serial.println(CircuitPlayground.temperature());
      controlChange(1, 1, CircuitPlayground.temperature());
      delay(1000);
      break;

    // Send random MIDI noteOn messages; channel 1. Buttons affect tempo.
    case 4:
      //CircuitPlayground.playTone(random(100, 1200), 500);
      noteOn(1, random(127), random(127));
      delay(delaySpeed);
      if (CircuitPlayground.rightButton()) delaySpeed--;
      if (CircuitPlayground.leftButton()) delaySpeed++;
      break;

    // Accelerometer: TBD
    case 5:
      Serial.print("X: ");
      Serial.println(round(CircuitPlayground.motionX() + 20 ));
      Serial.print("Y: ");
      Serial.println(round(CircuitPlayground.motionY() + 20 ));
      Serial.print("Z: ");
      Serial.println(round(CircuitPlayground.motionZ() + 20 ));
      noteOn(1, round(CircuitPlayground.motionX() + 20 ), 127);
      noteOn(1, round(CircuitPlayground.motionY() + 20 ), 127);
      noteOn(1, round(CircuitPlayground.motionZ() + 20 ), 127);
      delay(200);
      break;

    // Accelerometer detect tap: TBD
    case 6:
//      if (tapDetected) {
//        noteOn(2, 1, 127 );
//        Serial.println(CircuitPlayground.getAccelTap());
//        Serial.println("TAP!");
//        tapDetected  = false;
//        delay(100);
//      }

      break;

    // Listen for incoming MIDI messages
    case 7:
      rx = MidiUSB.read();

      //    Serial.println("Listening...");
      if (rx.header == 9) {
        Serial.print("Received: ");
        Serial.print(rx.header, DEC);
        Serial.print("-");
        Serial.print(rx.byte1, DEC);
        Serial.print("-");
        Serial.print(rx.byte2, DEC);
        Serial.print("-");
        Serial.println(rx.byte3, DEC);

        Serial.println(pitchFrequency[rx.byte2])  ;
        CircuitPlayground.playTone(pitchFrequency[rx.byte2], 50);
        CircuitPlayground.clearPixels();
      }
      break;

    // Play random tones from device
    case 8:
      int pixel, delayRandom;
      pixel = random(9);
      delayRandom = random(350) + 100;

      CircuitPlayground.playTone(random(100, 1200), 100);
      CircuitPlayground.setPixelColor(pixel, random(255), random(255), random(255));
      delay(delayRandom);
      CircuitPlayground.setPixelColor(pixel, 0, 0, 0);
      break;    // TBD

    // Go into "Color Mixer" mode
    case 9:
      rx = MidiUSB.read();

      if (rx.header == 9) {
        CircuitPlayground.clearPixels();
        Serial.print("Received: ");
        Serial.print(rx.header, DEC);
        Serial.print("-");
        Serial.print(rx.byte1, DEC);
        Serial.print("-");
        Serial.print(rx.byte2, DEC);
        Serial.print("-");
        Serial.println(rx.byte3, DEC);

        // set red value
        if (rx.byte1 == 144) red = rx.byte2 + rx.byte3;

        // set gsreen value
        if (rx.byte1 == 145) green = rx.byte2 + rx.byte3;

        // set blue value
        if (rx.byte1 == 146) blue = rx.byte2 + rx.byte3;

        for (int x = 0; x < 10; x++) {
          CircuitPlayground.setPixelColor(x, red, green, blue);
        }

      }
      break;

  }

  if (CircuitPlayground.slideSwitch()) {
    do {
      showMode();
      manageMode();
    } while (CircuitPlayground.slideSwitch());
    CircuitPlayground.clearPixels();
  }

}
