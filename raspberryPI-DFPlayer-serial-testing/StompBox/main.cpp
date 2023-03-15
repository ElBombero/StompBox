#ifndef __cplusplus
#define __cplusplus
#endif // defined __cplusplus
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <sstream>
//#include <ncurses.h>
#include "DFPlayer.h"
#include "Logger.h"
#include <poll.h>

void PrintMenu()
{
    printf("MENU:\n");
    printf("0|m\tShow this MENU\n");
    printf("1|s\tQuery Current Status\n");
    printf("2\tSet Playback Source [ 0: U | 1: TF | 2: AUX | 3: SLEEP | 4: FLASH ]\n");
    printf("3\tSet Equalizer [ 0: Normal | 1: Pop | 2: Rock | 3: Jazz | 4: Classic | 5: Base ]\n");
    printf("4\tSet Folder [ 0 - 10 ]\n");
    printf("5\tSet Track [ 0 - 29999 ]\n");
    printf("6\tSet Playback Mode [ 0: Repeat | 1: RepeatFolder | 2: RepeatSingle | 3: Random ]\n");
    printf("7\tSet Volume [ 0 - 30 ]\n");
    printf("8\tDecrease Volume\n");
    printf("9\tIncrease Volume\n");
    printf("10\tPrevious Track\n");
    printf("11\tNext Track\n");
    printf("12\tStandby Mode\n");
    printf("13\tNormal Mode\n");
    printf("14\tReset\n");
    printf("15\tAdjust Volume [1: Open volume adjust] [volume gain (0 - 31)\n");
    printf("16\tPlay\n");
    printf("17\tPause\n");
    printf("18\tRepeat\n");

}

int main(int argc, char** argv)
{

    pollfd cinfd[1];
    // Theoretically this should always be 0, but one fileno call isn't going to hurt, and if
    // we try to run somewhere that stdin isn't fd 0 then it will still just work

    printf("hello from %s!\n", "StompBox");
    DFPlayer::Instance().InitializeSerialPort(argv[1], argv[2]);
    PrintMenu();

    DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Init_Params, 0x08, 0, true);
    usleep(200000);
    DFPlayer::Instance().ReadResponse();
   
    cinfd[0].fd = fileno(stdin);
    cinfd[0].events = POLLIN;
    while (true)
    {
        while (!poll(cinfd, 1, 200));

        std::string input;
        std::cin >> input;
        std::vector<std::string> parts;
        std::istringstream f(input);
        std::string s;
        while (getline(f, s, ':'))
        {
            parts.push_back(s);
        }

        std::string command = parts[0];
        int numParam1 = 0;
        int numParam2 = 0;
        if (parts.size() > 1)
        {
            numParam1 = atoi(parts[1].c_str());
        }
        if (parts.size() > 2)
        {
            numParam2 = atoi(parts[2].c_str());
        }

        if ((command == "99") || (command == "x") || (command == "exit"))
        {
            break;
        }
        else if ((command == "0") || (command == "m") || (command == "menu"))
        {
            PrintMenu();
        }
        else if ((command == "1") || (command == "s") || (command == "status"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if ((command == "2") || (command == "src") || (command == "source"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback_Source_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if ((command == "3") || (command == "eq") || (command == "equalizer"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Eq_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if ((command == "4") || (command == "fldr") || (command == "folder"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Folder_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if ((command == "5") || (command == "trck") || (command == "track"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Track_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if ((command == "6") || (command == "mod") || (command == "mode"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback_Mode_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if ((command == "7") || (command == "v") || (command == "vol") || (command == "volume"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if ((command == "8") || (command == "v-") || (command == "vol-") || (command == "volume-"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Dec, 0, 0, true);
        }
        else if ((command == "9") || (command == "v+") || (command == "vol+") || (command == "volume+"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Inc, 0, 0, true);
        }
        else if ((command == "10") || (command == "t-") || (command == "prev") || (command == "previous"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Previous, 0, 0, true);
        }
        else if ((command == "11") || (command == "t+") || (command == "nxt") || (command == "next"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Next, 0, 0, true);
        }
        else if ((command == "12") || (command == "sby") || (command == "standby"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Standby, 0, 0, true);
        }
        else if ((command == "13") || (command == "norm") || (command == "normal"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Normal, 0, 0, true);
        }
        else if ((command == "14") || (command == "rst") || (command == "reset"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Reset, 0, 0, true);
        }
        else if ((command == "15") || (command == "adjvol") || (command == "adjustvolume"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Adjust_set, static_cast<uint16_t>(numParam1), static_cast<uint8_t>(numParam2), true);
        }
        else if ((command == "16") || (command == "p") || (command == "play") || (command == "playback"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback, 0, 0, true);
        }
        else if ((command == "17") || (command == "paus") || (command == "pause"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Pause, 0, 0, true);
        }
        else if ((command == "18") || (command == "rpt") || (command == "repeat"))
        {
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Repeat, 0, 0, true);
        }

        usleep(200000);
        DFPlayer::Instance().ReadResponse();
    }

    usleep(200000);
    DFPlayer::Instance().ReadResponse();

    return 0;
}