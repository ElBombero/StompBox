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
    printf(" 0|m\tShow this MENU\n");
    printf("99|x63\tEXIT\n");
    printf("\n");
    printf(" 1|x01\tNext Track\n");
    printf(" 2|x02\tPrevious Track\n");
    printf(" 3|x03\tSet Track [ 0 - 29999 ]\n");
    printf(" 4|x04\tIncrease Volume\n");
    printf(" 5|x05\tDecrease Volume\n");
    printf(" 6|x06\tSet Volume [ 0 - 30 ]\n");
    printf(" 7|x07\tSet Equalizer [ 0: Normal | 1: Pop | 2: Rock | 3: Jazz | 4: Classic | 5: Base ]\n");
    printf(" 8|x08\tSet Playback Mode [ 0: Repeat | 1: RepeatFolder | 2: RepeatSingle | 3: Random ]\n");
    printf(" 9|x09\tSet Playback Source [ 0: U | 1: TF | 2: AUX | 3: SLEEP | 4: FLASH ]\n");
    printf("10|x0a\tStandby Mode\n");
    printf("11|x0b\tNormal Mode\n");
    printf("12|x0c\tReset\n");
    printf("13|x0d\tPlayback\n");
    printf("14|x0e\tPause\n");
    printf("15|x0f\tSet Folder [ 0 - 10 ]\n");
    printf("16|x10\tAdjust Volume [ 1: Open volume adjust] [volume gain (0 - 31) ]\n");
    printf("17|x11\tRepeat Play [ 0: Stop Play | 1: Start Repeat Play ]\n");
    printf("\n");
    printf("63|x3f\tSend Initialization Params [ 0 - 0x0f (each bit represent one device) ]\n");
    printf("64|x40\tError, request retransmittion\n");
    printf("65|x41\tReply\n");
    printf("66|x42\tQuery Current Status\n");
    printf("67|x43\tQuery Current Volume\n");
    printf("68|x44\tQuery Current EQ\n");
    printf("69|x45\tQuery Current Playback Mode\n");
    printf("70|x46\tQuery Current SW Version\n");
    printf("71|x47\tQuery TF card Total Files\n");
    printf("72|x48\tQuery U-disk Total Files\n");
    printf("73|x49\tQuery Flash Total Files\n");
    printf("74|x4a\tKeep on\n");
    printf("75|x4b\tQuery TF card Current Track\n");
    printf("76|x4c\tQuery U-disk Current Track\n");
    printf("77|x4d\tQuery Flash Current Track\n");
}

bool IsStringInList(const std::string& str, const std::vector<std::string>& list)
{
    for (auto listedStr : list)
    {
        if (str == listedStr)
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char** argv)
{
    printf("hello from %s!\n", "StompBox");

    pollfd cinfd[2];
    // Theoretically this should always be 0, but one fileno call isn't going to hurt, and if
    // we try to run somewhere that stdin isn't fd 0 then it will still just work

    //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Init_Params, 0x08, 0, true);
    //usleep(200000);
    //DFPlayer::Instance().ReadResponse();
   
    // cinfd[0] : standard input:
    cinfd[0].fd = fileno(stdin);
    cinfd[0].events = POLLIN;
    // cinfd[1] : serial port:
    cinfd[1].fd = DFPlayer::Instance().InitializeSerialPort(argv[1], argv[2]);
    cinfd[1].events = POLLIN;
    PrintMenu();

    while (true)
    {
        while (!poll(cinfd, 2, 100));
        if (cinfd[1].revents & POLLIN)
        {
            usleep(200000);
            DFPlayer::Instance().ReadResponse();
            continue;
        }
        if (!cinfd[0].revents & POLLIN)
        {
            continue;
        }

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
        int commandInt = atoi(command.c_str());
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

        if (IsStringInList(command, std::vector<std::string>{"99", "x63", "x063", "xff", "0xff", "x", "exit"}))
        {
            break;
        }
        else if (IsStringInList(command, std::vector<std::string>{"0", "m", "menu"}))
        {
            PrintMenu();
            continue;
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"1",*/ "x01", "0x01", "t+", "nxt", "next"}))
        {
            commandInt = 0x01;
            DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Next, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"2",*/ "x02", "0x02", "t-", "prev", "previous"}))
        {
            commandInt = 0x02;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Previous, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"3",*/ "x03", "0x03", "t", "trck", "track"}))
        {
            commandInt = 0x03;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Track_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"4",*/ "x04", "0x04", "v+", "vol+", "volume+"}))
        {
            commandInt = 0x04;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Inc, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"5",*/ "x05", "0x05", "v-", "vol-", "volume-"}))
        {
            commandInt = 0x05;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Dec, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"6",*/ "x06", "0x06", "v", "vol", "volume"}))
        {
            commandInt = 0x06;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"7",*/ "x07", "0x07", "eq", "equalizer"}))
        {
            commandInt = 0x07;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Eq_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"8",*/ "x08", "0x08", "mod", "mode"}))
        {
            commandInt = 0x08;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback_Mode_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"9"*/ "x09", "0x09", "src", "source"}))
        {
            commandInt = 0x09;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback_Source_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"10"*/ "x0a", "0x0a", "sby", "standby"}))
        {
            commandInt = 0x0a;
            //DDFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Standby, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"11"*/ "x0b", "0x0b", "norm", "normal"}))
        {
            commandInt = 0x0b;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Normal, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"12"*/ "x0c", "0x0c", "rst", "reset"}))
        {
            commandInt = 0x0c;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Reset, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"13"*/ "x0d", "0x0d", "p", "play", "playback"}))
        {
            commandInt = 0x0d;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Playback, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"14"*/ "x0e", "0x0e", "pse", "paus", "pause"}))
        {
            commandInt = 0x0e;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Pause, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"15"*/ "x0f", "0x0f", "f", "fldr", "folder"}))
        {
            commandInt = 0x0f;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Folder_Set, static_cast<uint16_t>(numParam1), 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"16"*/ "x10", "0x10", "av", "va", "adjvol", "voladj", "adjustvolume"}))
        {
            commandInt = 0x10;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Volume_Adjust_set, static_cast<uint16_t>(numParam1), static_cast<uint8_t>(numParam2), true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"17"*/ "x11", "0x11", "r", "rpt", "repeat"}))
        {
            commandInt = 0x11;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Command_Repeat, static_cast<uint16_t>(numParam1), 0, true);
        }

        else if (IsStringInList(command, std::vector<std::string>{/*"63"*/ "x3f", "0x3f", "init", "initpars"}))
        {
            commandInt = 0x3f;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"64"*/ "x40", "0x40", "e", "err", "error"}))
        {
            commandInt = 0x40;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"65"*/ "x41", "0x41", "rpl", "rply", "replay"}))
        {
            commandInt = 0x41;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"66"*/ "x42", "0x42", "s", "gs", "sg", "stat", "getstat", "status", "getstatus"}))
        {
            commandInt = 0x42;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"67"*/ "x43", "0x43", "gv", "vg", "getvol", "getvolume"}))
        {
            commandInt = 0x43;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"68"*/ "x44", "0x44", "geq", "geteq"}))
        {
            commandInt = 0x44;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"69"*/ "x45", "0x45", "gm", "getmod", "getmode"}))
        {
            commandInt = 0x45;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"70"*/ "x46", "0x46", "gver", "getversion"}))
        {
            commandInt = 0x46;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"71"*/ "x47", "0x47", "tffiles", "gtffiles", "gettffiles"}))
        {
            commandInt = 0x47;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"72"*/ "x48", "0x48", "ufiles", "gufiles", "getufiles"}))
        {
            commandInt = 0x48;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"73"*/ "x49", "0x49", "flashfiles", "gflashfiles", "getflashfiles"}))
        {
            commandInt = 0x49;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"74"*/ "x4a", "0x4a", "keep", "keepon"}))
        {
            commandInt = 0x4a;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"74"*/ "x4b", "0x4b", "tftrack", "gtftrack", "gettftrack"}))
        {
            commandInt = 0x4b;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"74"*/ "x4c", "0x4c", "utrack", "gutrack", "getutrack"}))
        {
            commandInt = 0x4c;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }
        else if (IsStringInList(command, std::vector<std::string>{/*"74"*/ "x4d", "0x4d", "flastrack", "gflashtrack", "getflashtrack"}))
        {
            commandInt = 0x4d;
            //DFPlayer::Instance().SendCommand(DFPlayer::ECommand::Query_Current_Status, 0, 0, true);
        }

        if (commandInt > 0)
        {
            std::cout << "command: " << command << std::endl;
            DFPlayer::Instance().SendCommand(static_cast<DFPlayer::ECommand>(commandInt),
                static_cast<uint16_t>(numParam1),
                static_cast<uint8_t>(numParam2),
                false);
                //true);
        }
        else
        {
            std::cout << "Invalid command: \'" << command << "\'" << std::endl;
        }

        //usleep(200000);
        //DFPlayer::Instance().ReadResponse();
    }

    usleep(200000);
    DFPlayer::Instance().ReadResponse();

    return 0;
}