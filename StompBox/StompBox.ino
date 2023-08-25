#include <Arduino.h>
#include "midi.h"
#include "utils.h"
#include "config.h"
#ifdef USE_2LINE_I2C_DISPLAY
#include "lcd_i2c_display.h"
#elif defined USE_U8G2_DISPLAY
#include "u8g2_display.h"
# endif // defined USE_U8G2_DISPLAY
#ifdef USE_ROTARY_SWITCH
#include "rotarySwitch.h"
#endif // USE_ROTARY_SWITCH
#ifdef USE_VS1053
#include <SPI.h>
#include "VS1053b.h"
#endif //defined USE_VS1053

enum DisplaySection {
  Section_Instrument1Mode,
  Section_Instrument1Name,
  Section_Instrument2Mode,
  Section_Instrument2Name,
  Section_PlayMode,
  Section_SelInstrPrev,
  Section_SelInstrAct,
  Section_SelInstrNext,
  Section_Debug,
  Section_About_Product,
  Section_About_Version,
  Section_About_Manufacturer,
  Section_About_Copyright
};

enum OperationMode {
  Mode_Play,
  Mode_SelectInstrument
};

enum EventType {
  Event_StartPlay,
  Event_StopPlay ,
  Event_ModeChanged
};


Config config;
OperationMode g_mode = OperationMode::Mode_Play;
int g_selectInstrumentChannel;
uint8_t g_volume;
unsigned long g_displayBacklightOnTimestamp;
//const unsigned int c_midiBaudRate = 9600;
bool g_interrupted;


void ISR_1();
void ISR_2();

struct TapInput {
  TapInput(unsigned int isrPin, void (*isr)(), uint8_t initMidiKey,
    DisplaySection displaySctPlay, DisplaySection displaySctName,
    Midi::PercussionChange instrChangeOper) :
    isrGpioPin(isrPin),
    isrHandler(isr),
    state(false),
    instrumentChangeOper(instrChangeOper),
    midiKey(initMidiKey),
    handlePush(false),
    displaySectionMode(displaySctPlay),
    displaySectionName(displaySctName),
    //upCounter(0),
    modeChangeEnable(true),
    noteOnKey(c_noNote)
  {
    pinMode(isrPin, INPUT_PULLUP);
    AttachInterrupt();
  }
  void AttachInterrupt() {
    attachInterrupt(digitalPinToInterrupt(isrGpioPin), isrHandler, FALLING);
  }
  void DetachInterrupt() {
    detachInterrupt(digitalPinToInterrupt(isrGpioPin));
  }

  unsigned int isrGpioPin;
  void (*isrHandler)();
  bool state; // true = pushed
  bool modeChangeEnable;
  bool handlePush;
  uint8_t midiKey;
  //unsigned int upCounter;
  DisplaySection displaySectionMode;
  DisplaySection displaySectionName;
  unsigned long pushTimestamp;
  Midi::PercussionChange instrumentChangeOper;
  uint8_t noteOnChannel;
  uint8_t noteOnKey;
  uint8_t noteOnVelocity;
  static const uint8_t c_noNote = 0xff;
};

TapInput g_tapInput[] = {
  TapInput(2, &ISR_1, Midi::PercussionKey::Tambourine, Section_Instrument1Mode,
    Section_Instrument1Name, Midi::PercussionChange::Change_Previous),
  TapInput(3, &ISR_2, Midi::PercussionKey::Claves, Section_Instrument2Mode,
    Section_Instrument2Name, Midi::PercussionChange::Change_Next)
};

Midi* g_midi = nullptr;
#ifdef USE_2LINE_I2C_DISPLAY
I2cDisplay* g_display = nullptr;
#elif defined USE_U8G2_DISPLAY
U8x8Display* g_display = nullptr;
#endif // defined USE_U8G2_DISPLAY
#ifdef USE_ROTARY_SWITCH
RotarySwitch* g_rotarySwitch = nullptr;
#endif // USE_ROTARY_SWITCH
#ifdef USE_VS1053
VS1053b* g_vs1053b = new VS1053b(VS_XCS, VS_XDCS, VS_DREQ, VS_RESET);
#endif //defined USE_VS1053
#ifdef DEBUG_MIDI_SERIAL
char debugBuffer[32];
#else
char* debugBuffer = nullptr;
#endif // DEBUG_MIDI_SERIAL

void setup() {
  delay(100);
  pinMode(config.c_volumePin, INPUT); //INPUT_PULLUP);
  //Wire.begin();
  g_midi = new Midi(config.c_midiConnectionMode, config.c_skipSameMidiCommands, config.c_skipSameMidiCommandsPeriod, &Serial);
#ifdef USE_2LINE_I2C_DISPLAY
  g_display = new I2cDisplay(config.c_i2cDisplayAddress, config.c_i2cDisplayRows, config.c_i2cDisplayCols);
  g_display->SetSection(DisplaySection::Section_Instrument1Mode, 0, 0, 1);
  g_display->SetSection(DisplaySection::Section_Instrument1Name, 0, 1, 15);
  g_display->SetSection(DisplaySection::Section_Instrument2Mode, 1, 0, 1);
  g_display->SetSection(DisplaySection::Section_Instrument2Name, 1, 1, 15);
#elif defined USE_U8G2_DISPLAY
  g_display = new U8x8Display(new U8X8_SSD1306_128X64_NONAME_HW_I2C(U8X8_PIN_NONE), config.c_u8x8DisplayRows, config.c_u8x8DisplayCols);
  g_display->SetSection(DisplaySection::Section_Instrument1Mode, 2, 0, 0);
  g_display->SetSection(DisplaySection::Section_Instrument1Name, 2, 0, 16);
  g_display->SetSection(DisplaySection::Section_Instrument2Mode, 4, 0, 0);
  g_display->SetSection(DisplaySection::Section_Instrument2Name, 4, 0, 16);
  g_display->SetSection(DisplaySection::Section_PlayMode, 0, 0, 16); //, u8x8_font_px437wyse700b_2x2_f);
  g_display->SetSection(DisplaySection::Section_Debug, 6, 0, 16);
  g_display->SetSection(DisplaySection::Section_About_Product, 0, 0, 16); //, u8x8_font_px437wyse700b_2x2_f);
  g_display->SetSection(DisplaySection::Section_About_Version, 2, 0, 16);
  g_display->SetSection(DisplaySection::Section_About_Manufacturer, 4, 0, 16);
  g_display->SetSection(DisplaySection::Section_About_Copyright, 6, 0, 16);
  g_display->SetSection(DisplaySection::Section_SelInstrPrev, 2, 0, 0);
  g_display->SetSection(DisplaySection::Section_SelInstrAct, 4, 0, 0);
  g_display->SetSection(DisplaySection::Section_SelInstrNext, 6, 0, 0);

#endif // defined USE_U8G2_DISPLAY
  g_midi->SetDebugBuffer(debugBuffer);


#ifdef USE_ROTARY_SWITCH
  g_rotarySwitch = new RotarySwitch(config.c_rotarySwitchPin_clk, config.c_rotarySwtichPin_dta, config.c_rotarySwtichPin_btn);
#endif // USE_ROTARY_SWITCH
  
  g_interrupted = false;
  g_volume = 0x63;
  g_mode = OperationMode::Mode_Play;
  g_selectInstrumentChannel = -1;
#ifdef USE_2LINE_I2C_DISPLAY
  SetDisplayBacklight(EventType::Event_ModeChanged);
  g_display->Backlight(true);
#endif // defined USE_2LINE_I2C_DISPLAY
  g_display->SetContrast(config.c_displayContrast);
//  g_display->Write(0, 1, "Stompie v0.1.0");
//  g_display->Write(1, 0, "(C) Florete, 2023");
//  g_display->Write(0, 1, "Stompie v0.1.0");
//  g_display->Write(4, 0, "(C) Florete, 2023");
  ShowAbout(2000);
  //delay(2000);
  g_display->Backlight(false);
#ifdef USE_U8G2_DISPLAY
  g_display->WriteSection(DisplaySection::Section_PlayMode, "    PLAYBACK    ");
#endif // defined USE_U8G2_DISPLAY

#ifdef USE_VS1053
  // Enable real-time MIDI mode
  g_vs1053b->EnableMIDI();
  //vs1053b->WriteRegister(VS1053b::SCI_Registers::SCI_VOL, 0); //0x3f, 0x3f);
  delay(1000);
  g_vs1053b->SelectBank(0x09, VS1053b::MIDI_Banks::Bank_Drums1); //Bank select drums
  g_vs1053b->AssignInstrument(0x09, 30); //Set instrument number. 0xC0 is a 1 data byte command
  g_vs1053b->SelectBank(0x00, VS1053b::MIDI_Banks::Bank_Default); //Grand Piano
  g_vs1053b->AssignInstrument(0x00, 0x01); //Set instrument number. 0xC0 is a 1 data byte command
  delay(1000);

  g_vs1053b->NoteOn(0x09, 0x36, 0x63);
  g_vs1053b->NoteOn(0x09, 0x4b, 0x63);
  delay(990);
  //vs1053b->TalkMIDI(0x89, 0x26, 0x00);
  g_vs1053b->NoteOff(0x09, 0x36, 0x63);
  g_vs1053b->NoteOff(0x09, 0x4b, 0x63);
  delay(10);

  g_vs1053b->NoteOn(0x00, 0x3b, 0x63);
  g_vs1053b->NoteOn(0x00, 0x40, 0x63);
  g_vs1053b->NoteOn(0x00, 0x43, 0x63);
  delay(990);
  g_vs1053b->NoteOff(0x00, 0x3b, 0x63);
  g_vs1053b->NoteOff(0x00, 0x40, 0x63);
  g_vs1053b->NoteOff(0x00, 0x43, 0x63);
#endif //defined USE_VS1053
}

void ShowAbout(unsigned long wait_ms) {
  g_display->WriteSection(DisplaySection::Section_About_Product, "    Stompie    ");
  g_display->WriteSection(DisplaySection::Section_About_Version, "   ver. 1.1.0   ");
  g_display->WriteSection(DisplaySection::Section_About_Manufacturer, "  (C) Florete   ");
  g_display->WriteSection(DisplaySection::Section_About_Copyright, "      2023      ");
  delay(wait_ms);
  g_display->WriteSection(DisplaySection::Section_About_Product, "                ");
  g_display->WriteSection(DisplaySection::Section_About_Version, "                ");
  g_display->WriteSection(DisplaySection::Section_About_Manufacturer, "                ");
  g_display->WriteSection(DisplaySection::Section_About_Copyright, "                ");
}

#ifdef USE_2LINE_I2C_DISPLAY
void SetDisplayBacklight(EventType event) {
  bool backlight = false;
  unsigned long now = millis();
  switch(config.c_displayBacklightMode) {
    case DisplayBacklightMode::Backlight_AlwaysOn:
      backlight = true; break;
    case DisplayBacklightMode::Backlight_AlwaysOnWithTimeout:
      backlight = ((now - g_displayBacklightOnTimestamp) < config.c_displayBacklightOnTimeout);
      if(event != EventType::Event_StopPlay) {
        g_displayBacklightOnTimestamp = now;
      }
      break;
    case DisplayBacklightMode::Backlight_PlayOff_ModeOn:
      backlight = (g_mode == OperationMode::Mode_SelectInstrument); break;
    case DisplayBacklightMode::Backlight_PlayFlash:
      switch(g_mode) {
        case OperationMode::Mode_Play:
          backlight = (event == EventType::Event_StartPlay); break;
      }; break;
    case DisplayBacklightMode::Backlight_PlayFlash_ModeOn:
      switch(g_mode) {
        case OperationMode::Mode_SelectInstrument:
          backlight = true; break;
        case OperationMode::Mode_Play:
          backlight = (event == EventType::Event_StartPlay); break;
      }; break;
    default:
      break;
  }
  /*if(backlight) {
      g_displayBacklightOnTimestamp = now;
  }*/
  g_display->Backlight(backlight);
}
#endif // define USE_2LINE_I2C_DISPLAY

void UpdateDisplay(const uint16_t highlightedLines) {
  for(uint8_t i = 0; i < config.c_channelsCount; ++i) {
    bool highlightedLine = ((highlightedLines & (1 << i)) != 0);
    highlightedLine |= (g_mode == OperationMode::Mode_SelectInstrument) && (g_selectInstrumentChannel == i);
      char buffer[32];
      Midi::GetPercussionName(static_cast<Midi::PercussionKey>(g_tapInput[i].midiKey), buffer, 32);
      g_display->WriteSection(g_tapInput[i].displaySectionName, buffer, highlightedLine);
#ifdef USE_2LINE_I2C_DISPLAY
      g_display->WriteSection(g_tapInput[i].displaySectionMode,
        ((g_mode == OperationMode::Mode_Play) || (g_selectInstrumentChannel != i)) ? " " : ">");
#elif defined USE_U8G2_DISPLAY
      //g_display->WriteSection(DisplaySection::Section_PlayMode,
      //((g_mode == OperationMode::Mode_Play) || (g_selectInstrumentChannel != i)) ? "PLAYBACK" : "SEL.INSTR.");
      g_display->WriteSection(DisplaySection::Section_PlayMode, 
        ((g_mode == OperationMode::Mode_Play) ? "    PLAYBACK    " : (g_selectInstrumentChannel ? "   INSTRUMENT  >" : "<  INSTRUMENT   ")));
        // DisplaySection::Section_SelInstrPrev
#endif
  }
}

void DebugToDisplay() {
#ifdef DEBUG_MIDI_SERIAL
  g_display->WriteSection(DisplaySection::Section_Debug, debugBuffer); 
#endif // DEBUG_MIDI_SERIAL
}

uint8_t counter = 0;
void loop() {
  //bool stateChanged = false;
  uint16_t highlightedLines = 0;
  for(uint8_t i = 0; i < config.c_channelsCount; ++i) {
    unsigned long now = millis();
    //bool stateChanged = false;
    if(g_tapInput[i].state) {
      if(g_tapInput[i].handlePush) {
        switch(g_mode) {
          case OperationMode::Mode_Play: {
              g_tapInput[i].handlePush = false;
              // PLAY NOTE 
              // (NOTE OFF FIRST - IF PLAYING)
              if(g_tapInput[i].noteOnKey != TapInput::c_noNote) {
                g_midi->MidiMessage(Midi::Command_NoteOff, g_tapInput[i].noteOnChannel, g_tapInput[i].noteOnKey, g_tapInput[i].noteOnVelocity);
                DebugToDisplay();
                g_tapInput[i].noteOnKey = TapInput::c_noNote;
              }
              g_midi->MidiMessage(Midi::Command_NoteOn, config.c_percussionChannel, g_tapInput[i].midiKey, g_volume);
              DebugToDisplay();
              g_tapInput[i].noteOnChannel = config.c_percussionChannel;
              g_tapInput[i].noteOnKey = g_tapInput[i].midiKey;
              g_tapInput[i].noteOnVelocity = g_volume;
              highlightedLines |= (1 << i);
            }
            break;
        }
        }

        // STATE CHANGE ON BUTTON LONG PRESS
        if(g_tapInput[i].modeChangeEnable && ((now - g_tapInput[i].pushTimestamp) >= config.c_longPressTime)) {
          switch(g_mode) {
            case OperationMode::Mode_Play: {
              g_tapInput[i].modeChangeEnable = false;
              g_mode = OperationMode::Mode_SelectInstrument;
              g_selectInstrumentChannel = i;
              //g_tapInput[i].modeChangeEnable = false;
//                continue;
              }
              break;
            case OperationMode::Mode_SelectInstrument: {
              if(g_selectInstrumentChannel == i) {
                g_tapInput[i].modeChangeEnable = false;
                g_mode = OperationMode::Mode_Play;
                }
              }
              break;
          }
        }

        // RE-ENABLE ISR:
        if(digitalRead(g_tapInput[i].isrGpioPin) &&
          ((now - g_tapInput[i].pushTimestamp) >= config.c_KeyPressIgnoreTime)) {
            g_tapInput[i].AttachInterrupt();
            g_tapInput[i].state = false;
            g_tapInput[i].modeChangeEnable = true;
            //g_tapInput[i].upCounter = 0;
            //stateChanged = true;
//            continue;
        }
      }

      if(!g_tapInput[i].state) {
        if(g_tapInput[i].handlePush) {
          switch(g_mode) {
            case OperationMode::Mode_SelectInstrument: {
                g_tapInput[i].handlePush = false;
                // NOTE OFF IF PLAYING
                if(g_tapInput[g_selectInstrumentChannel].noteOnKey != TapInput::c_noNote) {
                  g_midi->MidiMessage(Midi::Command_NoteOff, g_tapInput[g_selectInstrumentChannel].noteOnChannel, g_tapInput[g_selectInstrumentChannel].noteOnKey, g_tapInput[g_selectInstrumentChannel].noteOnVelocity);
                  DebugToDisplay();
                  g_tapInput[g_selectInstrumentChannel].noteOnKey = TapInput::c_noNote;
                }
                Midi::PercussionKey oldInstrument = static_cast<Midi::PercussionKey>(g_tapInput[g_selectInstrumentChannel].midiKey);
                g_tapInput[g_selectInstrumentChannel].midiKey = Midi::ChangePercussion(oldInstrument, g_tapInput[i].instrumentChangeOper);
                if(!config.c_silenceInstrumentSwitching) {
                  g_midi->MidiMessage(Midi::Command_NoteOn, config.c_percussionChannel, g_tapInput[g_selectInstrumentChannel].midiKey, g_volume);
                  DebugToDisplay();
                  g_tapInput[g_selectInstrumentChannel].noteOnChannel = config.c_percussionChannel;
                  g_tapInput[g_selectInstrumentChannel].noteOnKey = g_tapInput[g_selectInstrumentChannel].midiKey;
                  g_tapInput[g_selectInstrumentChannel].noteOnVelocity = g_volume;
                }
//                continue;
              }
              break;
          }
        }
      }

      // NOTE OFF ON TIMEOUT:
      if((g_tapInput[i].noteOnKey != TapInput::c_noNote) && ((now - g_tapInput[i].pushTimestamp) >= config.c_maxNotePlayTime)) {
        g_midi->MidiMessage(Midi::Command_NoteOff, g_tapInput[i].noteOnChannel, g_tapInput[i].noteOnKey, g_tapInput[i].noteOnVelocity);
        DebugToDisplay();
        g_tapInput[i].noteOnKey = TapInput::c_noNote;
      }
  }
  
  // DISPLAY OUTPUT:
  UpdateDisplay(highlightedLines);

  /*if(stateChanged) {
    config.c_flashOnTap) {
    g_display->Backlight(true);
  }*/

  if(!(counter % 2)) { // TODO: replace hardcoded value by configured constant
#ifdef USE_ROTARY_SWITCH
    g_volume = g_rotarySwitch->GetValues();
#else
/*
    int volume = analogRead(config.c_volumePin);
    g_volume = (volume >> 3);
    / *static const float fDiff = (config.c_maxMeasVolume - config.c_minMeasVolume);
    float fVal = (float)Max(volume - config.c_minMeasVolume, 0);
    fVal = Min((size_t)(fVal * 1023.0f / fDiff), 1023);
    /g_volume = ((int)fVal >> 3);* /
*/
    g_volume = 0x7f; // TMP!!!
#endif // USE_ROTARY_SWITCH
#ifdef USE_2LINE_I2C_DISPLAY
    SetDisplayBacklight(EventType::Event_StopPlay);
#endif //defined USE_2LINE_I2C_DISPLAY
  }
  
  /*if(config.c_flashOnTap) {
    g_display->Backlight(false);
  }*/
  // TODO: Get rid of the counter, replace by comparing milliseconds
  if((counter % 20) == 0) {
    //g_midi->MidiMessage(Midi::MidiCommand::Command_ControlChange, config.c_percussionChannel, 0, 0);
    //g_midi->MidiMessage(Midi::MidiCommand::Command_ControlChange, config.c_percussionChannel, 0, 0x78);
    //g_midi->MidiMessage(Midi::MidiCommand::Command_ControlChange, config.c_percussionChannel, 0, 0x7f);
    //g_midi->MidiMessage(Midi::MidiCommand::Command_ControlChange, config.c_percussionChannel, Midi::ControlChange_Controllers::CCController_SoundBankSelMSB, 0x00);
    //g_midi->MidiMessage(Midi::MidiCommand::Command_ControlChange, config.c_percussionChannel, Midi::ControlChange_Controllers::CCController_SoundBankSelLSB, 0);
//      g_midi->MidiMessage(Midi::MidiCommand::Command_ControlChange, config.c_percussionChannel, Midi::ControlChange_Controllers::CCController_SoundBankSelMSB, 63);
//      g_midi->MidiMessage(Midi::MidiCommand::Command_ControlChange, config.c_percussionChannel, Midi::ControlChange_Controllers::CCController_SoundBankSelLSB, 32);
      //g_midi->MidiMessage(Midi::MidiCommand::Command_ProgramChange, config.c_percussionChannel, 0x78, 0);
//      g_midi->MidiMessage(Midi::MidiCommand::Command_ProgramChange, config.c_percussionChannel, 0x78, 0);
      g_midi->MidiMessage(Midi::MidiCommand::Command_ProgramChange, config.c_percussionChannel, 35, 0);
      DebugToDisplay();
      //g_midi->MidiMessage(Midi::MidiCommand::Command_PatchChange, config.c_percussionChannel, 1, 0);
      //g_midi->MidiMessage(Midi::MidiCommand::Command_ControlChange, config.c_percussionChannel, 0, 0x78);
  }
  ++counter;
  for(int i = 0; (i < 5) && !g_interrupted; ++i) { // TODO: replace hardcoded value by configured constant
    delay(1);
    g_interrupted = false;
  }
}

void ISR_Input(TapInput& tap) {
  if(!tap.state) {
    tap.DetachInterrupt();
    tap.pushTimestamp = millis();
    tap.state = true;
    tap.handlePush = true;
    g_interrupted = true;
  }
}

void ISR_1()
{
  ISR_Input(g_tapInput[0]);
}

void ISR_2()
{
  ISR_Input(g_tapInput[1]);
}


