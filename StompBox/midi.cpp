#include <Arduino.h>
#include "midi.h"

bool Midi::MidiMessage(MidiCommand command, uint8_t channel, uint8_t key, uint8_t velocity) {
  static MidiCommand lastCommand = MidiCommand::Command_UNDEFINED;
  static unsigned long lastCommandTime = 0;
  size_t written = 0;
  if(!m_pSerial->availableForWrite()) {
    return false;
  }
  // Only write command if it differs from the last one written
  unsigned long now = millis();
  if((lastCommand != command) ||
    !m_skipSameCommands ||
    ((now - lastCommandTime) >= m_skipSameMidiCommandsPeriod)) {
    written = m_pSerial->write(command | (0x0f & channel));
    if(written != 1) {
      return false;
    }
    lastCommand = command;
    lastCommandTime = now;
  }
  
  if(command != MidiCommand::Command_NonMusical) {
    written = m_pSerial->write(0x7f & key);
    if(written != 1) {
      return false;
    }
    if(command != MidiCommand::Command_ChannelPressure) {
      written = m_pSerial->write(0x7f & velocity);
      if(written != 1) {
        return false;
      }
    }
  }
  return true;
}

Midi::PercussionKey Midi::ChangePercussion(Midi::PercussionKey oldKey, Midi::PercussionChange operation) {
  int newKey = (int)oldKey;
  if(operation == Change_Previous) {
    --newKey;
    if( newKey < (int)PercussionKey::FIRST) {
      newKey = (int)PercussionKey::LAST;
    }
  }
  else if(operation == Change_Next) {
    ++newKey;
    if( newKey > (int)PercussionKey::LAST) {
      newKey = (int)PercussionKey::FIRST;
    }
  }
  return (Midi::PercussionKey)newKey;
}

void Midi::GetPercussionName(Midi::PercussionKey key, char* bufferOut, size_t bufferSize) {
  char buffer[32];
  switch(key) {
    case PercussionKey::AcousticBassDrum:
      sprintf(buffer, "Acoustic Bass Drum"); break;
    case PercussionKey::BassDrum1:
      sprintf(buffer, "Bass Drum 1"); break;
    case PercussionKey::SideStick:
      sprintf(buffer, "Side Stick"); break;
    case PercussionKey::AcousticSnare:
      sprintf(buffer, "Acoustic Snare"); break;
    case PercussionKey::HandClap:
      sprintf(buffer, "Hand Clap"); break;
    case PercussionKey::ElectronicSnare:
      sprintf(buffer, "Electronic Snare"); break;
    case PercussionKey::LowFloorTom:
      sprintf(buffer, "Low Floor Tom"); break;
    case PercussionKey::ClosedHiHat:
      sprintf(buffer, "Closed Hi-Hat"); break;
    case PercussionKey::HighFloorTom:
      sprintf(buffer, "High Floor Tom"); break;
    case PercussionKey::PedalHiHat:
      sprintf(buffer, "Pedal Hi-Hat"); break;
    case PercussionKey::LowTom:
      sprintf(buffer, "Low Tom"); break;
    case PercussionKey::OpenHiHat:
      sprintf(buffer, "Open Hi-Hat"); break;
    case PercussionKey::LowMidTom:
      sprintf(buffer, "Low Mid Tom"); break;
    case PercussionKey::HiMidTom:
      sprintf(buffer, "High Mid Tom"); break;
    case PercussionKey::CrashCymbal1:
      sprintf(buffer, "Crash Cymbal 1"); break;
    case PercussionKey::HighTom:
      sprintf(buffer, "High Tom"); break;
    case PercussionKey::RideCymbal1:
      sprintf(buffer, "Ride Cymbal 1"); break;
    case PercussionKey::ChineseCymbal:
      sprintf(buffer, "Chinese Cymbal"); break;
    case PercussionKey::RideBell:
      sprintf(buffer, "Ride Bell"); break;
    case PercussionKey::Tambourine:
      sprintf(buffer, "Tambourine"); break;
    case PercussionKey::SplashCymbal:
      sprintf(buffer, "Splash Cymbal"); break;
    case PercussionKey::Cowbell:
      sprintf(buffer, "Cowbell"); break;
    case PercussionKey::CrashCymbal2:
      sprintf(buffer, "Crash Cymbal 2"); break;
    case PercussionKey::Vibraslap:
      sprintf(buffer, "Vibraslap"); break;
    case PercussionKey::RideCymbal2:
      sprintf(buffer, "Ride Cymbal 2"); break;
    case PercussionKey::HiBongo:
      sprintf(buffer, "High Bongo"); break;
    case PercussionKey::LowBongo:
      sprintf(buffer, "Low Bongo"); break;
    case PercussionKey::MuteHiConga:
      sprintf(buffer, "Mute High Conga"); break;
    case PercussionKey::OpenHiConga:
      sprintf(buffer, "Open High Conga"); break;
    case PercussionKey::LowConga:
      sprintf(buffer, "Low Conga"); break;
    case PercussionKey::HighTimbale:
      sprintf(buffer, "High Timbale"); break;
    case PercussionKey::LowTimbale:
      sprintf(buffer, "Low Timbale"); break;
    case PercussionKey::HighAgogo:
      sprintf(buffer, "High Agogo"); break;
    case PercussionKey::LowAgogo:
      sprintf(buffer, "Low Agogo"); break;
    case PercussionKey::Cabasa:
      sprintf(buffer, "Cabasa"); break;
    case PercussionKey::Maracas:
      sprintf(buffer, "Maracas"); break;
    case PercussionKey::ShortWhistle:
      sprintf(buffer, "Short Whistle"); break;
    case PercussionKey::LongWhistle:
      sprintf(buffer, "Long Whistle"); break;
    case PercussionKey::ShortGuiro:
      sprintf(buffer, "Short Guiro"); break;
    case PercussionKey::LongGuiro:
      sprintf(buffer, "Long Guiro"); break;
    case PercussionKey::Claves:
      sprintf(buffer, "Claves"); break;
    case PercussionKey::HiWoodBlock:
      sprintf(buffer, "High Wood Block"); break;
    case PercussionKey::LowWoodBlock:
      sprintf(buffer, "Low Wood Block"); break;
    case PercussionKey::MuteCuica:
      sprintf(buffer, "Mute Cuica"); break;
    case PercussionKey::OpenCuica:
      sprintf(buffer, "Open Cuica"); break;
    case PercussionKey::MuteTriangle:
      sprintf(buffer, "Mute Triangle"); break;
    case PercussionKey::OpenTriangle:
      sprintf(buffer, "Open Triangle"); break;
    default:
      sprintf(buffer, "--UNKNOWN PERCUSSION--"); break;
  }
  size_t len = Min(strlen(buffer), (bufferSize -1));
  memcpy((void*)bufferOut, (void*)buffer, len);
  bufferOut[len] = '\0';
}
