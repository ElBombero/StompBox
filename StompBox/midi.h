#include <Arduino_BuiltIn.h>


#include <HardwareSerial.h>
#include "utils.h"


class Midi
{
public:
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

  static const uint8_t c_VelocityMin = 0x00;
  static const uint8_t c_VelocityMax = 0x7f;
  static const uint8_t c_KeyMin = 0x00;
  static const uint8_t c_KeyMax = 0x7f;

  Midi(HardwareSerial* pSerial = &Serial,
    unsigned long baudRate = 31250,
    const bool skipSameCommands = true,
    unsigned long skipSameMidiCommandsPeriod = 500) :
    m_pSerial(pSerial),
    m_skipSameCommands(skipSameCommands),
    m_skipSameMidiCommandsPeriod(skipSameMidiCommandsPeriod)
   {
    m_pSerial->begin(baudRate);
  }

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
  bool m_skipSameCommands;
  unsigned long m_skipSameMidiCommandsPeriod;
};