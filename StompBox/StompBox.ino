#include <Arduino.h>
#include "midi.h"
#include "utils.h"
#include "display.h"
#include "config.h"

enum DisplaySection {
  Section_Instrument1Mode = 0,
  Section_Instrument1Name = 1,
  Section_Instrument2Mode = 2,
  Section_Instrument2Name = 3
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


void ISR_1();
void ISR_2();

struct TapInput {
  TapInput(unsigned int isrPin, void (*isr)(), uint8_t initMidiKey,
    DisplaySection lcdSctPlay, DisplaySection lcdSctName,
    Midi::PercussionChange instrChangeOper) :
    isrGpioPin(isrPin),
    midiKey(initMidiKey),
    isrHandler(isr),
    state(false),
    lcdSectionMode(lcdSctPlay),
    lcdSectionName(lcdSctName),
    instrumentChangeOper(instrChangeOper),
    upCounter(0),
    modeChangeEnable(true)
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

  bool modeChangeEnable;
  uint8_t midiKey;
  unsigned int isrGpioPin;
  bool state;
  unsigned int upCounter;
  DisplaySection lcdSectionMode;
  DisplaySection lcdSectionName;
  unsigned long pushTimestamp;
  Midi::PercussionChange instrumentChangeOper;
  void (*isrHandler)();
};

TapInput g_tapInput[] = {
  TapInput(2, &ISR_1, Midi::PercussionKey::Tambourine, Section_Instrument1Mode,
    Section_Instrument1Name, Midi::PercussionChange::Change_Previous),
  TapInput(3, &ISR_2, Midi::PercussionKey::Claves, Section_Instrument2Mode,
    Section_Instrument2Name, Midi::PercussionChange::Change_Next)
};
Midi* g_midi = nullptr;
I2cDisplay* g_lcd = nullptr;

void setup() {
  pinMode(config.c_volumePin, INPUT); //INPUT_PULLUP);
  //Wire.begin();
  g_midi = new Midi(config.c_midiConnectionMode, config.c_skipSameMidiCommands, config.c_skipSameMidiCommandsPeriod, &Serial);
  g_lcd = new I2cDisplay(config.c_i2cDisplayAddress, config.c_i2cDisplayRows, config.c_i2cDisplayCols);
  
  g_lcd->SetSection(DisplaySection::Section_Instrument1Mode, 0, 0, 1);
  g_lcd->SetSection(DisplaySection::Section_Instrument1Name, 0, 1, 15);
  g_lcd->SetSection(DisplaySection::Section_Instrument2Mode, 1, 0, 1);
  g_lcd->SetSection(DisplaySection::Section_Instrument2Name, 1, 1, 15);
  g_volume = 63;
  g_mode = OperationMode::Mode_Play;
  g_selectInstrumentChannel = -1;
  SetDisplayBacklight(EventType::Event_ModeChanged);
  g_lcd->Backlight(true);
  g_lcd->Write(0, 1, "Stompie v0.1.0");
  g_lcd->Write(1, 0, "(C) Florete, 2023");
  delay(2000);
  g_lcd->Backlight(false);
}

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
  g_lcd->Backlight(backlight);
}

uint8_t counter = 0;
void loop() {
  //bool stateChanged = false;
  for(uint8_t i = 0; i < config.c_channelsCount; ++i) {
    bool stateChanged = false;
    if(g_tapInput[i].state) {
      if(!g_tapInput[i].upCounter) {
        switch(g_mode) {
          case OperationMode::Mode_Play:
            g_lcd->WriteSection(g_tapInput[i].lcdSectionMode, "~");
            SetDisplayBacklight(EventType::Event_StartPlay);
            break;
          /*case OperationMode::Mode_SelectInstrument:
            Midi::PercussionKey oldInstrument = g_tapInput[g_selectInstrumentChannel].midiKey;
            g_tapInput[g_selectInstrumentChannel].midiKey = Midi::ChangePercussion(oldInstrument, g_tapInput[i].instrumentChangeOper);
            if(!config.c_silenceInstrumentSwitching) {
              g_midi->MidiMessage(Midi::Command_NoteOn, config.c_percussionChannel, 
              g_tapInput[g_selectInstrumentChannel].midiKey, g_volume);
            }
            char buffer[16];
            Midi::GetPercussionName(g_tapInput[i].midiKey, buffer, 16);
            g_lcd->WriteSection(g_tapInput[i].lcdSectionName, buffer);
            break;*/
        }
      }
      if(digitalRead(g_tapInput[i].isrGpioPin)) {
        if(g_tapInput[i].upCounter++ >= 2) { // TODO: replace hardcoded value by configured constant
          g_tapInput[i].AttachInterrupt();
          g_tapInput[i].state = false;
          g_tapInput[i].upCounter = 0;
          stateChanged = true;
        }
      }
      else {
        g_tapInput[i].upCounter = 1;
      }
      if(g_tapInput[i].modeChangeEnable && ((millis() - g_tapInput[i].pushTimestamp) >= config.c_longPressTime))
      {
        switch(g_mode) {
          case OperationMode::Mode_Play:
            g_mode = OperationMode::Mode_SelectInstrument;
            g_selectInstrumentChannel = i;
            g_lcd->WriteSection(g_tapInput[i].lcdSectionMode, ">");
            SetDisplayBacklight(EventType::Event_ModeChanged);
            g_tapInput[i].modeChangeEnable = false;
            break;
          case OperationMode::Mode_SelectInstrument:
            if(g_selectInstrumentChannel == i) {
              g_mode = OperationMode::Mode_Play;
              g_lcd->WriteSection(g_tapInput[i].lcdSectionMode, " ");
              SetDisplayBacklight(EventType::Event_ModeChanged);
            }
            else {
              // TBD .. handle this event
            }
            g_tapInput[i].modeChangeEnable = false;
            break;
        }
      }
      else if(stateChanged)
      //else if(g_tapInput[i].upCounter) //stateChanged)
      {
        switch(g_mode) {
          /*case OperationMode::Mode_Play:
            g_lcd->WriteSection(g_tapInput[i].lcdSectionMode, "~");
            SetDisplayBacklight(EventType::Event_StartPlay);
            break;*/
          case OperationMode::Mode_SelectInstrument:
            Midi::PercussionKey oldInstrument = static_cast<Midi::PercussionKey>(g_tapInput[g_selectInstrumentChannel].midiKey);
            g_tapInput[g_selectInstrumentChannel].midiKey = Midi::ChangePercussion(oldInstrument, g_tapInput[i].instrumentChangeOper);
            if(!config.c_silenceInstrumentSwitching) {
              g_midi->MidiMessage(Midi::Command_NoteOn, config.c_percussionChannel, 
              g_tapInput[g_selectInstrumentChannel].midiKey, g_volume);
            }
            char buffer[16];
            Midi::GetPercussionName(static_cast<Midi::PercussionKey>(g_tapInput[i].midiKey), buffer, 16);
            g_lcd->WriteSection(g_tapInput[i].lcdSectionName, buffer);
            break;
        }
      }
//}
      /*if(g_mode == OperationMode::Mode_Play) {
        g_lcd->WriteSection(g_tapInput[i].lcdSectionMode, "~");
      }*/
    }
  }
  /*if(stateChanged) {
    config.c_flashOnTap) {
    g_lcd->Backlight(true);
  }*/
  delay(25); // TODO: replace hardcoded value by configured constant

  if(!(counter % 2)) { // TODO: replace hardcoded value by configured constant
    int volume = analogRead(config.c_volumePin);
    g_volume = (volume >> 3);
    /*static const float fDiff = (config.c_maxMeasVolume - config.c_minMeasVolume);
    float fVal = (float)Max(volume - config.c_minMeasVolume, 0);
    fVal = Min((size_t)(fVal * 1023.0f / fDiff), 1023);
    /g_volume = ((int)fVal >> 3);*/
    bool displayOff = true;
    for(uint8_t i = 0; i < config.c_channelsCount; ++i) {
      if(!g_tapInput[i].upCounter) {
        char buffer[16];
        Midi::GetPercussionName(static_cast<Midi::PercussionKey>(g_tapInput[i].midiKey), buffer, 16);
        g_lcd->WriteSection(g_tapInput[i].lcdSectionName, buffer);
        g_lcd->WriteSection(g_tapInput[i].lcdSectionMode,
          ((g_mode == OperationMode::Mode_Play) || (g_selectInstrumentChannel != i)) ? " " : ">");
      }
      else {
        displayOff = false;
      }
    }
    if(displayOff) {
      SetDisplayBacklight(EventType::Event_StopPlay);
    }
  }
  
  /*if(config.c_flashOnTap) {
    g_lcd->Backlight(false);
  }*/
  ++counter;
}

void ISR_Input(TapInput& tap) {
  if(!tap.state) {
    tap.DetachInterrupt();
    switch(g_mode) {
      case OperationMode::Mode_Play:
        if(g_midi != nullptr) {
          g_midi->MidiMessage(Midi::Command_NoteOn, config.c_percussionChannel, tap.midiKey, g_volume);
        }
        break;
      case OperationMode::Mode_SelectInstrument:
        /*Midi::PercussionKey oldInstrument = g_tapInput[g_selectInstrumentChannel].midiKey;
        g_tapInput[g_selectInstrumentChannel].midiKey = Midi::ChangePercussion(oldInstrument, tap.instrumentChangeOper);
        g_midi->MidiMessage(Midi::Command_NoteOn, config.c_percussionChannel, 
          g_tapInput[g_selectInstrumentChannel].midiKey, g_volume);*/
        break;
    }
    tap.pushTimestamp = millis();
    tap.state = true;
    tap.upCounter = 0;
    tap.modeChangeEnable = true;
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


