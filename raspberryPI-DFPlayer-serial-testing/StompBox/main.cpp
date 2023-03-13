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
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Set, 25); // , 0, true);
    usleep(200000);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback_Source_Set, static_cast<uint16_t>(DFPlayer::PlaybackSource::Source_FLASH)); // , 0, true);
    usleep(200000);
//    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Folder_Set, 1); // , 0, true);
//    usleep(200000);
//    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Track_Set, 1); // , 0, true);
//    usleep(200000);
//    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback_Mode_Set, static_cast<uint16_t>(DFPlayer::PlaybackMode::Mode_Repeat)); // , 0, true);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback_Mode_Set, static_cast<uint16_t>(DFPlayer::PlaybackMode::Mode_Random)); // , 0, true);
    usleep(200000);
//    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback); // , 0, 0, true);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Repeat, 1); // , 0, 0, true);

//    usleep(200000);
//    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status); // , 0, 0, true);
    
    sleep(15);

    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Repeat, 0); // , 0, 0, true);
    usleep(200000);
    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Pause); // , 0, 0, true);
    usleep(200000);

    DFPlayer::Instance().ReadResponse();
    return 0;
}