#ifndef VS1053B_H
#define VS1053B_H

#include <inttypes.h>

#define VELOCITY_MAX 0x7f
#define VELOCITY_MIN 0x00

class VS1053b
{
public:
  enum SCI_RWinstructions
  {
    SPI_READ  = 0x03,
    SPI_WRITE = 0x02
  };

  enum SCI_Registers
  {
    SCI_MODE        = 0x00,
    SCI_STATUS      = 0x01,
    SCI_BASS        = 0x02,
    SCI_CLOCKF      = 0x03,
    SCI_DECODE_TIME = 0x04,
    SCI_AUDATA      = 0x05,
    SCI_WRAM        = 0x06,
    SCI_WRAMADDR    = 0x07,
    SCI_HDAT0       = 0x08,
    SCI_HDAT1       = 0x09,
    SCI_AIADDR      = 0x0a,
    SCI_VOL         = 0x0b,
    SCI_AICTRL0     = 0x0c,
    SCI_AICTRL1     = 0x0d,
    SCI_AICTRL2     = 0x0e,
    SCI_AICTRL3     = 0x0f
  };

  enum SCI_Mode
  {
    SM_DIFF           = 0x00,
    SM_LAYER12        = 0x01,
    SM_RESET          = 0x02,
    SM_CANCEL         = 0x03,
    SM_EARSPEAKER_LO  = 0x04,
    SM_TESTS          = 0x05,
    SM_STREAM         = 0x06,
    SM_EARSPEAKER_HI  = 0x07,
    SM_DACT           = 0x08,
    SM_SDIORD         = 0x09,
    SM_SDISHARE       = 0x0a,
    SM_SDINEW         = 0x0b,
    SM_ADPCM          = 0x0c,
    SM_LINE1          = 0x0e,
    SM_CLK_RANGE      = 0x0f
  };

  enum SCI_StatusBits
  {
    SS_DO_NOT_JUMP    = 0x8000,
    SS_SWING          = 0x7000,
    SS_VCM_OVERLOAD   = 0x0800,
    SS_VCM_DISABLE    = 0x0400,
    SS_VER            = 0x00f0,
    SS_APDOWN2        = 0x0008,
    SS_APDOWN1        = 0x0004,
    SS_AD_CLOCK       = 0x0002,
    SS_REFERENCE_SEL  = 0x0001
  };

  enum SCI_BassBits
  {
    ST_AMPLITUDE      = 0xf000,
    ST_FREQLIMIT      = 0x0f00,
    SB_AMPLITUDE      = 0x00f0,
    SB_FREQLIMIT      = 0x000f
  };

  enum SCI_ClockF
  {
    SC_MULT   = 0xe000,
    SC_ADD    = 0x1800,
    SC_FREQ   = 0x07ff
  };

  enum MIDI_Banks
  {
    Bank_Default  = 0,
    Bank_Drums1   = 0x78, 
    Bank_Drums2   = 0x7f,
    Bank_Melodic  = 0x79
  };

  VS1053b(uint8_t xcs, uint8_t xdcs, uint8_t dreq, uint8_t reset);

  uint16_t ReadRegister(SCI_Registers sciRegister);
  void WriteRegister(SCI_Registers sciRegister, uint16_t word);
  void WriteRegister(SCI_Registers sciRegister, uint8_t byteH, uint8_t byteL);
  void WriteRegister(SCI_Registers sciRegister, uint16_t* words, size_t count);
  void TalkMIDI(uint8_t cmd, uint8_t data1, uint8_t data2);
  void EnableMIDI(void);
  inline void NoteOn(uint8_t channel, uint8_t note, uint8_t velocity = VELOCITY_MAX)
  {
    TalkMIDI(0x90 | channel, note, velocity);
  }
  inline void NoteOff(uint8_t channel, uint8_t note, uint8_t velocity = VELOCITY_MIN)
  {
    TalkMIDI(0x80 | channel, note, velocity);
  }
  inline void SelectBank(uint8_t channel, MIDI_Banks bank)
  {
    TalkMIDI(0xb0 | channel, 0x00, bank);
  }
  inline void AssignInstrument(uint8_t channel, uint8_t instrument)
  {
    TalkMIDI(0xc0 | channel, instrument, 0);
  }
  
private:
  void SendMIDIbyte(uint8_t data);
  void LoadUserCode(unsigned short* data);

  uint8_t m_XCS;
  uint8_t m_XDCS;
  uint8_t m_DREQ;
  uint8_t m_RESET;
};

#endif // defined VS1053B_H
