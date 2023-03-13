#ifndef __cplusplus
#define __cplusplus
#endif // defined __cplusplus

#pragma once

#include <vector>
#include <string>
#include <memory>
#include "SerialPort.h"
//#include <stdio.h>
//#include <fcntl.h>
//#include <unistd.h>



class DFPlayer
{
public:
    enum class EEq
    {
        Eq_Normal = 0x00,
        Eq_Pop = 0x01,
        Eq_Rock = 0x02,
        Eq_Jazz = 0x03,
        Eq_Classic = 0x04,
        Eq_Base = 0x05
    };

    enum class PlaybackMode
    {
        Mode_Repeat         = 0x00,
        Mode_RepeatFolder   = 0x01,
        Mode_RepeatSingle   = 0x02,
        Mode_Random         = 0x03
    };

    enum class PlaybackSource
    {
        Source_U = 0x00,
        Source_TF = 0x01,
        Source_AUX = 0x02,
        Source_SLEEP = 0x03,
        Source_FLASH = 0x04
    };

    enum class ECommand
    {
        Command_Next = 0x01,
        Command_Previous = 0x02,
        Command_Track_Set = 0x03,     // Param [Word]: track number (0 - 29999)
        Command_Volume_Inc = 0x04,
        Command_Volume_Dec = 0x05,
        Command_Volume_Set = 0x06,     // Param DL [Byte]: volume (0 - 30)
        Command_Eq_Set = 0x07,     // Param DL [EEq]: equalizer type
        Command_Playback_Mode_Set = 0x08,     // Param DL [PlaybackMode]: playback mode
        Command_Playback_Source_Set = 0x09,     // Param DL [PlaybackSource]: playback source
        Command_Standby = 0x0a,
        Command_Normal = 0x0b,
        Command_Reset = 0x0c,
        Command_Playback = 0x0d,
        Command_Pause = 0x0e,
        Command_Folder_Set = 0x0f,     // Param DL [Byte]: folder to playback set (1 - 10)
        Command_Volume_Adjust_set = 0x10,     // Param DH [Byte]: Open volume adjust (=1), Param DL [Byte]: volume gain (0 - 31)
        Command_Repeat = 0x11,     // Param DL [Byte]: 1 - start repeat play / 0 - stop play

        Query_STAY_I = 0x3c,     // ???
        Query_STAY_II = 0x3d,     // ???
        Query_STAY_III = 0x3e,     // ???
        Query_Init_Params = 0x3f,     // Param DL [Byte]: (0 - 0x0f) - each bit represent one device of the low four bits
        Query_Error_Retransmit = 0x40,
        Query_Reply = 0x41,
        Query_Current_Status = 0x42,
        Query_Current_Volume = 0x43,
        Query_Current_Eq = 0x44,
        Query_Current_PlaybackMode = 0x45,
        Query_Current_SW_Version = 0x46,
        Query_Total_TF_Card_Files_Count = 0x47,
        Query_Total_U_Disk_Files_Count = 0x48,
        Query_Total_Flash_Files_Count = 0x49,
        Query_Total_Keep_On = 0x4a,
        Query_Current_Track_on_TF_Card = 0x4b,
        Query_Current_Track_on_U_Disk = 0x4c,
        Query_Current_Track_on_Flash = 0x4d
    };

    enum class ESpecialTokens
    {
        Token_Start = 0x7e,
        Token_End = 0xef,
        Token_Version = 0xff
    };


    static DFPlayer& Instance();
    ~DFPlayer();
    void InitializeSerialPort(const std::string& port, const std::string& config = "9600;");

    static void SendCommand(ECommand command, uint16_t paramWL = 0, uint8_t paramH = 0, bool feedback = false);
    void ReadResponse();
    static void HexDump(const char* buffer, size_t bufferSize, std::string& output);
    static void HexDump(const std::vector<uint8_t>& buffer, std::string& output);

private:
    DFPlayer();

    static void CalculateCommandChecksum(std::vector<uint8_t>& commandData);
    std::unique_ptr<SerialPort> m_serialPort;
    //static const uint8_t c_versionDF = 0xff;
};