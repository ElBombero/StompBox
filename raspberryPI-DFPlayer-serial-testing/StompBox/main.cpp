#ifndef __cplusplus
#define __cplusplus
#endif // defined __cplusplus
#include <cstdio>
#include <unistd.h>
#include "DFPlayer.h"

int main(int argc, char** argv)
{
    printf("hello from %s!\n", "StompBox");
    DFPlayer::Instance().InitializeSerialPort(argv[1], argv[2]);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Set, 25);
    usleep(200000);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback_Source_Set, static_cast<uint16_t>(DFPlayer::PlaybackSource::Source_FLASH));
    usleep(200000);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Folder_Set, 1);
    usleep(200000);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Track_Set, 1);
    usleep(200000);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback_Mode_Set, static_cast<uint16_t>(DFPlayer::PlaybackMode::Mode_Repeat));
    usleep(200000);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback);

    
    
    sleep(10);
    
    DFPlayer::Instance().ReadResponse();
    return 0;
}