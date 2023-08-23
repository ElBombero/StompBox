#include <SPI.h>
#include "VS1053b.h"
#include "VS1053b_plugins.h"

VS1053b::VS1053b(uint8_t xcs, uint8_t xdcs, uint8_t dreq, uint8_t reset) :
  m_XCS(xcs),
  m_XDCS(xdcs),
  m_DREQ(dreq),
  m_RESET(reset)
{
  pinMode(m_DREQ, INPUT_PULLUP);
  pinMode(m_XCS, OUTPUT);
  pinMode(m_XDCS, OUTPUT);
  digitalWrite(m_XCS, HIGH);  //Deselect Control
  digitalWrite(m_XDCS, HIGH); //Deselect Data
  pinMode(m_RESET, OUTPUT);

  //Setup SPI for VS1053
  pinMode(10, OUTPUT); // Pin 10 must be set as an output for the SPI communication to work
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);

  //From page 12 of datasheet, max SCI reads are CLKI/7. Input clock is 12.288MHz. 
  //Internal clock multiplier is 1.0x after power up. 
  //Therefore, max SPI speed is 1.75MHz. We will use 1MHz to be safe.
  SPI.setClockDivider(SPI_CLOCK_DIV16); //Set SPI bus speed to 1MHz (16MHz / 16 = 1MHz)
  SPI.transfer(0xff); //Throw a dummy byte at the bus

  delayMicroseconds(1);
  digitalWrite(m_RESET, HIGH); //Bring up VS1053
  
  WriteRegister(VS1053b::SCI_Registers::SCI_VOL, 0); //0x3f, 0x3f);
}

void VS1053b::WriteRegister(SCI_Registers sciRegister, uint16_t word)
{
  WriteRegister(sciRegister, (uint8_t)((word & 0xff00) >> 8), (uint8_t)(word & 0x00ff));
}

void VS1053b::WriteRegister(SCI_Registers sciRegister, uint16_t* words, size_t count)
{
  while(!digitalRead(m_DREQ)) /*delayMicroseconds(1)*/; //Wait for DREQ to go high indicating IC is available
  digitalWrite(m_XCS, LOW); //Select control
  //SCI consists of instruction byte, address byte, and 16-bit data word.
  SPI.transfer(SPI_WRITE); //Write instruction
  SPI.transfer((uint8_t)sciRegister);
  for(size_t i = 0; i < count; ++i)
  {
    while(!digitalRead(m_DREQ)) /*delayMicroseconds(1)*/; //Wait for DREQ to go high indicating command is complete
    SPI.transfer((uint8_t)((words[i] & 0xff00) >> 8)); // write higher byte
    SPI.transfer((uint8_t)(words[i] & 0x00ff));        // write lower byte
  }
  //while(!digitalRead(m_DREQ)) /*delayMicroseconds(1)*/; //Wait for DREQ to go high indicating command is complete
  digitalWrite(m_XCS, HIGH); //Deselect Control
}

//Write to VS10xx register
//SCI: Data transfers are always 16bit. When a new SCI operation comes in 
//DREQ goes low. We then have to wait for DREQ to go high again.
//XCS should be low for the full duration of operation.
void VS1053b::WriteRegister(SCI_Registers sciRegister, uint8_t byteH, uint8_t byteL)
{
  while(!digitalRead(m_DREQ)) /*delayMicroseconds(1)*/; //Wait for DREQ to go high indicating IC is available
  digitalWrite(m_XCS, LOW); //Select control
  //SCI consists of instruction byte, address byte, and 16-bit data word.
  SPI.transfer(SPI_WRITE); //Write instruction
  SPI.transfer((uint8_t)sciRegister);
  SPI.transfer(byteH);
  SPI.transfer(byteL);
  //while(!digitalRead(m_DREQ)) /*delayMicroseconds(1)*/; //Wait for DREQ to go high indicating command is complete
  digitalWrite(m_XCS, HIGH); //Deselect Control
}

//Read from VS10xx register
//SCI: Data transfers are always 16bit. When a new SCI operation comes in 
//DREQ goes low. We then have to wait for DREQ to go high again.
//XCS should be low for the full duration of operation.
uint16_t VS1053b::ReadRegister(SCI_Registers sciRegister)
{
  uint16_t data = 0x0000;
  while(!digitalRead(m_DREQ)) /*delayMicroseconds(1)*/; //Wait for DREQ to go high indicating IC is available
  digitalWrite(m_XCS, LOW); //Select control
  //SCI consists of instruction byte, address byte, and 16-bit data word.
  SPI.transfer(SPI_READ); //Read instruction
  SPI.transfer((uint8_t)sciRegister);
  data |= (SPI.transfer(0xff) << 8);
  data |= (SPI.transfer(0xff) & 0x00ff);
  //while(!digitalRead(m_DREQ)) /*delayMicroseconds(1)*/; //Wait for DREQ to go high indicating command is complete
  digitalWrite(m_XCS, HIGH); //Deselect Control
  return data;
}


void VS1053b::TalkMIDI(uint8_t cmd, uint8_t data1, uint8_t data2)
{
  //
  // Wait for chip to be ready (Unlikely to be an issue with real time MIDI)
  //
  while (!digitalRead(m_DREQ))
  {
    /*delayMicroseconds(10);*/
  }
  digitalWrite(m_XDCS, LOW);

  SendMIDIbyte(cmd);
  
  //Some commands only have one data byte. All cmds less than 0xBn have 2 data bytes 
  //(sort of: http://253.ccarh.org/handout/midiprotocol/)
  if(((cmd & 0xf0) <= 0xb0) || ((cmd & 0xf0) >= 0xe0))
  {
    SendMIDIbyte(data1);
    SendMIDIbyte(data2);
  }
  else
  {
    SendMIDIbyte(data1);
  }

  digitalWrite(m_XDCS, HIGH);
}

void VS1053b::SendMIDIbyte(uint8_t data)
{
  SPI.transfer(0);
  SPI.transfer(data);
  //SPI.transfer16((uint16_t)data << 8);
}

void VS1053b::EnableMIDI(void)
{
  /*const uint16_t sVS1053b_Realtime_MIDI_Plugin[] = { // Compressed plugin 
    0x0007, 0x0001, 0x8050, 0x0006, 0x0014, 0x0030, 0x0715, 0xb080, //    0
    0x3400, 0x0007, 0x9255, 0x3d00, 0x0024, 0x0030, 0x0295, 0x6890, //    8
    0x3400, 0x0030, 0x0495, 0x3d00, 0x0024, 0x2908, 0x4d40, 0x0030, //   10
    0x0200, 0x000a, 0x0001, 0x0050
  };*/

  LoadUserCode(pluginRtMidi);
  /*int i = 0;

  while (i < sizeof(sVS1053b_Realtime_MIDI_Plugin)/sizeof(uint16_t))
  {
    uint16_t addr, n, val;
    addr = sVS1053b_Realtime_MIDI_Plugin[i++];
    n = sVS1053b_Realtime_MIDI_Plugin[i++];
    while (n--)
    {
      val = sVS1053b_Realtime_MIDI_Plugin[i++];
      WriteRegister((SCI_Registers)(addr & 0xff), (val >> 8), (val & 0xff));
    }
  }*/
}

void VS1053b::LoadUserCode(unsigned short* data)
{
  int i = 0;
  while(i<sizeof(data)/sizeof(data[0]))
  {
    unsigned short addr, n, val;
    addr = data[i++];
    n = data[i++];
    if(n & 0x8000U)
    { /* RLE run, replicate n samples */
      n &= 0x7FFF;
      val = data[i++];
      while(n--)
      {
        WriteRegister((SCI_Registers)addr, val);
      }
    }
    else
    {           /* Copy run, copy n samples */
      while(n--)
      {
        val = data[i++];
        WriteRegister((SCI_Registers)addr, val);
      }
    }
  }
}