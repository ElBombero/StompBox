#ifndef MIDI_H
#define MIDI_H


#include <Arduino_BuiltIn.h>
#include <HardwareSerial.h>
//#include <MIDIUSB.h>
#include "utils.h"


class Midi
{
public:
  enum MidiConnectionMode {
    Midi_Serial_9600    = 0,
    Midi_Serial_31250   = 1,
    Midi_USB            = 2,
    Midi_Bluetooth      = 3,
    Midi_Default        = Midi_Serial_31250
  };

  enum MidiCommand {
    Command_UNDEFINED           = 0x00,
    Command_NoteOff             = 0x80,
    Command_NoteOn              = 0x90,
    Command_Aftertouch          = 0xa0,
    Command_ContinousController = 0xb0,
    Command_PatchChange         = 0xc0,
    Command_ChannelPressure     = 0xd0,
    Command_PitchBend           = 0xe0,
    Command_NonMusical          = 0xf0
  };

  enum PercussionKey {
    AcousticBassDrum      = 35,
    BassDrum1             = 36,
    SideStick             = 37,
    AcousticSnare         = 38,
    HandClap              = 39,
    ElectronicSnare       = 40,
    LowFloorTom           = 41,
    ClosedHiHat           = 42,
    HighFloorTom          = 43,
    PedalHiHat            = 44,
    LowTom                = 45,
    OpenHiHat             = 46,
    LowMidTom             = 47,
    HiMidTom              = 48,
    CrashCymbal1          = 49,
    HighTom               = 50,
    RideCymbal1           = 51,
    ChineseCymbal         = 52,
    RideBell              = 53,
    Tambourine            = 54,
    SplashCymbal          = 55,
    Cowbell               = 56,
    CrashCymbal2          = 57,
    Vibraslap             = 58,
    RideCymbal2           = 59,
    HiBongo               = 60,
    LowBongo              = 61,
    MuteHiConga           = 62,
    OpenHiConga           = 63,
    LowConga              = 64,
    HighTimbale           = 65,
    LowTimbale            = 66,
    HighAgogo             = 67,
    LowAgogo              = 68,
    Cabasa                = 69,
    Maracas               = 70,
    ShortWhistle          = 71,
    LongWhistle           = 72,
    ShortGuiro            = 73,
    LongGuiro             = 74,
    Claves                = 75,
    HiWoodBlock           = 76,
    LowWoodBlock          = 77,
    MuteCuica             = 78,
    OpenCuica             = 79,
    MuteTriangle          = 80,
    OpenTriangle          = 81,
    FIRST                 = AcousticBassDrum,
    LAST                  = OpenTriangle,
  };

  enum PercussionChange {
    Change_Previous,
    Change_Next
  };

  enum USB_CableId {
    CId_Misc                  = 0x00, // Miscellaneous function codes. Reserved for future extensions
    CId_Reserved              = 0x01, // Cable events. Reserved for future expansion.
    CId_2BStream              = 0x02, // Two-byte System Common messages like MTC, SongSelect, etc.
    CId_3BStream              = 0x03, // Three-byte System Common messages like SPP, etc
    CId_SysExStartOrContinue  = 0x04, // SysEx starts or continues
    CId_1BSysCommon           = 0x05, // Single-byte System Common Message
    CId_SysExEndFollowing1B   = 0x05, // SysEx ends with following single byte
    CId_SysExEndFollowing2B   = 0x06, // SysEx ends with following two bytes
    CId_SysExEndFollowing3B   = 0x07, // SysEx ends with following three bytes.
    CId_NoteOff               = 0x08, // Note-off
    CId_NoteOn                = 0x09, // Note-on
    CId_PolyKeyPress          = 0x0a, // Poly-KeyPress
    CId_ControlChange         = 0x0b, // Control Change
    CId_ProgramChange         = 0x0c, // Program Change
    CId_ChannelPressure       = 0x0d, // Channel Pressure
    CId_PitchBendChange       = 0x0e, // PitchBend Change
    CId_1B                    = 0x0f  // Single Byte
  };

  static const uint8_t c_VelocityMin = 0x00;
  static const uint8_t c_VelocityMax = 0x7f;
  static const uint8_t c_KeyMin = 0x00;
  static const uint8_t c_KeyMax = 0x7f;

  Midi(MidiConnectionMode connectionMode = MidiConnectionMode::Midi_Default, 
    const bool skipSameCommands = true,
    unsigned long skipSameMidiCommandsPeriod = 500,
    HardwareSerial* pSerial = &Serial) :
  m_pSerial(pSerial),
  m_skipSameCommands(skipSameCommands),
  m_skipSameMidiCommandsPeriod(skipSameMidiCommandsPeriod)
  {
    switch(connectionMode) {
      case MidiConnectionMode::Midi_Serial_9600:
        m_pSerial->begin(9600); break;
      case MidiConnectionMode::Midi_Serial_31250:
        m_pSerial->begin(31250); break;
    }
  }

  /*Midi(MidiConnectionMode connectionMode = MidiConnectionMode::Midi_Default) {
    switch(connectionMode) {
      case case MidiConnectionMode::Midi_USB:
        m_usb = new MidiUSB();
    }
  }*/

  ~Midi() {
    m_pSerial->end();
    //delete m_pBuffer;
  }

  bool MidiMessage(MidiCommand command, uint8_t channel, uint8_t key, uint8_t velocity);
  static void GetPercussionName(PercussionKey key, char* buffer, size_t bufferSize = 32);
  static PercussionKey ChangePercussion(PercussionKey oldKey, PercussionChange operation);


private:
  //Circularuint8_tBuffer* m_pBuffer;
  HardwareSerial* m_pSerial = nullptr;
  //MidiUSB* m_usb = nullptr;
  bool m_skipSameCommands;
  unsigned long m_skipSameMidiCommandsPeriod;
};

#endif // defined MIDI_H
