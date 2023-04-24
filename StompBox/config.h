#ifndef CONFIG_H
#define CONFIG_H

#include "midi.h"

/*
ToDo:
o fix display - remains of previous longer text in sections
- LED stomp and mode statuses indication/signalization
- change-instrument mode timeout (?)
- set channel volume when in change-instrument mode (instead of the device volume)
o (C)Florete 2023, Stompie v0.1.0 splashscreen
- editable configuration parameters / Menu mode (?)
- Midi:
  - USB Midi support
  - Bluetooth Midi support
  - Forward incoming Midi commands
o disable/solve long pushing both buttons / other than in change-instrument mode (enter MENU mode on both long?)
- clean code, refactor

Known issues:
- unwanted immediate instrument change on mode switch from play to instrument-change mode
*/

enum DisplayBacklightMode {
  Backlight_AlwaysOff             = 0,
  Backlight_PlayOff_ModeOn        = 1,
  Backlight_PlayFlash             = 2,
  Backlight_PlayFlash_ModeOn      = 3,
  Backlight_AlwaysOnWithTimeout   = 4,
  Backlight_AlwaysOn              = 5
};

struct Config {
  const unsigned long c_displayBacklightOnTimeout = 5000;
  const DisplayBacklightMode c_displayBacklightMode = DisplayBacklightMode::Backlight_PlayFlash_ModeOn;
  const bool c_silenceInstrumentSwitching = false;
  const unsigned long c_longPressTime = 1000;
  //const bool c_flashOnTap = false;
  const bool c_skipSameMidiCommands = true;
  const unsigned long c_skipSameMidiCommandsPeriod = 1000;
  const uint8_t c_percussionChannel = 10;
  const uint8_t c_channelsCount = 2;
  const Midi::MidiConnectionMode c_midiConnectionMode = Midi::MidiConnectionMode::Midi_Serial_31250;
  //const unsigned int c_midiBaudRate = 31250;
  //const unsigned int c_midiBaudRate = 9600;
  const uint8_t c_i2cDisplayAddress = 0x27;
  const uint8_t c_i2cDisplayRows = 2;
  const uint8_t c_i2cDisplayCols = 16;
  const uint8_t c_volumePin = A0;
  //const int  c_minMeasVolume = 0x78;
  //const int  c_maxMeasVolume = 0x348;
};

#endif // ifndef CONFIG_H