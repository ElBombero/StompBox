#ifndef CONFIG_H
#define CONFIG_H

//#define USE_ROTARY_SWITCH
//#define MIDIUSB_SUPPORTED
#define USE_U8G2_DISPLAY 1
//#define USE_2LINE_I2C_DISPLAY
//#define USE_VS1053
//# define DEBUG_MIDI_SERIAL


#ifdef USE_VS1053
// VS1053 Shield pin definitions
#define VS_XCS    8 // 6 // Control Chip Select Pin (for accessing SPI Control/Status registers)
#define VS_XDCS   7 // 7 // Data Chip Select / BSYNC Pin
#define VS_DREQ   9 // 2 // Data Request Pin: Player asks for more data
#define VS_RESET  6 // 8 // Reset is active low
#endif //defined USE_VS1053



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

#ifdef USE_2LINE_I2C_DISPLAY
enum DisplayBacklightMode {
  Backlight_AlwaysOff             = 0,
  Backlight_PlayOff_ModeOn        = 1,
  Backlight_PlayFlash             = 2,
  Backlight_PlayFlash_ModeOn      = 3,
  Backlight_AlwaysOnWithTimeout   = 4,
  Backlight_AlwaysOn              = 5
};
#endif // defined USE_2LINE_I2C_DISPLAY

struct Config {
  //const char c_version[] = "1.1.0";
  const bool c_silenceInstrumentSwitching = false;
  const unsigned long c_longPressTime = 1000; // 1 sec
  const unsigned long c_maxNotePlayTime = 1000; // 1 sec
  const unsigned long c_KeyPressIgnoreTime = 100; // 100 ms
  //const bool c_flashOnTap = false;
  const bool c_skipSameMidiCommands = true;
  const unsigned long c_skipSameMidiCommandsPeriod = 1000;
  //const uint8_t c_percussionChannel = 0;
  const uint8_t c_percussionChannel = 9;
  const uint8_t c_channelsCount = 2;
  //const Midi::MidiConnectionMode c_midiConnectionMode = Midi::MidiConnectionMode::Midi_Serial_31250;
  const Midi::MidiConnectionMode c_midiConnectionMode = Midi::MidiConnectionMode::Midi_Serial_9600;
  //const unsigned int c_midiBaudRate = 31250;
  //const unsigned int c_midiBaudRate = 9600;
  const uint8_t c_volumePin = A0;
#ifdef USE_2LINE_I2C_DISPLAY
  const unsigned long c_displayBacklightOnTimeout = 5000;
  const DisplayBacklightMode c_displayBacklightMode = DisplayBacklightMode::Backlight_PlayFlash_ModeOn;
  const uint8_t c_i2cDisplayAddress = 0x27;
  const uint8_t c_i2cDisplayRows = 2;
  const uint8_t c_i2cDisplayCols = 16;
  const uint8_t c_displayContrast = 0xff;
#elif defined USE_U8G2_DISPLAY
  const uint8_t c_u8x8DisplayRows = 4;
  const uint8_t c_u8x8DisplayCols = 16;
  const uint8_t c_displayContrast = 0xff;
#endif
#ifdef USE_ROTARY_SWITCH
  const uint8_t c_rotarySwitchPin_clk = 8;
  const uint8_t c_rotarySwtichPin_dta = 7;
  const uint8_t c_rotarySwtichPin_btn = 4;
#endif // USE_ROTARY_SWITCH
  //const int  c_minMeasVolume = 0x78;
  //const int  c_maxMeasVolume = 0x348;
};

#endif // ifndef CONFIG_H