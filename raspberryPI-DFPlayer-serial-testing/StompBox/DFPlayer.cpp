#include "DFPlayer.h"
#include "SerialPort.h"
#include "Logger.h"

#include <vector>
#include <string>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>


DFPlayer::DFPlayer() :
    m_serialPort(new SerialPort())
{
    std::string logSource = "DFPlayer::DFPlayer";
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "created");
}

DFPlayer::~DFPlayer()
{
    std::string logSource = "DFPlayer::~DFPlayer";
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Destructing DFPlayer");
    m_serialPort->Close();
    m_serialPort = nullptr;
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "DFPlayer destructed");
}

DFPlayer& DFPlayer::Instance()
{
    static std::string s_initialization;
    static DFPlayer s_instance;
    return s_instance;
}

int DFPlayer::InitializeSerialPort(const std::string& port, const std::string& config)
{
    return m_serialPort->Initialize(port, config);
}

void DFPlayer::SendCommand(ECommand command, uint16_t paramWL, uint8_t paramH, bool feedback)
{
    std::string logSource = "DFPlayer::SendCommand";
    //Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Building command message...");

    uint8_t paramL = 0;
    std::vector<uint8_t> commandData;

    commandData.push_back(static_cast<uint8_t>(ESpecialTokens::Token_Start));   // $S 
    commandData.push_back(static_cast<uint8_t>(ESpecialTokens::Token_Version)); // DF Player version
    commandData.push_back(0x06);                                                // instruction length (excluding bytes $S and $O)
    commandData.push_back(static_cast<uint8_t>(command));                       // command ID
    switch (command)
    {
    case ECommand::Command_Track_Set:
        paramH = static_cast<uint8_t>(paramWL >> 8);
        paramL = static_cast<uint8_t>(paramWL & 0x00ff);
        break;

    case ECommand::Command_Volume_Set:
    case ECommand::Command_Eq_Set:
    case ECommand::Command_Playback_Mode_Set:
    case ECommand::Command_Playback_Source_Set:
    case ECommand::Command_Folder_Set:
    case ECommand::Command_Volume_Adjust_set:
    case ECommand::Command_Repeat:
    case ECommand::Query_Error_Retransmit:
    case ECommand::Query_Init_Params:
        paramL = static_cast<uint8_t>(paramWL & 0x00ff);
        break;
    }
    commandData.push_back(feedback ? 0x01 : 0x00);
    commandData.push_back(paramH);
    commandData.push_back(paramL);

    CalculateCommandChecksum(commandData);
    commandData.push_back(static_cast<uint8_t>(ESpecialTokens::Token_End));   // $O

    std::string message = "";
    HexDump(commandData, message);
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "...sending command: " + message);

    Instance().m_serialPort->WriteBytes(commandData);
}

void DFPlayer::CalculateCommandChecksum(std::vector<uint8_t>& commandData)
{
    uint16_t sum = 0;
    for (std::vector<uint8_t>::iterator it = commandData.begin(); it != commandData.end(); ++it)
    {
        if (*it != static_cast<uint8_t>(ESpecialTokens::Token_Start))
        {
            sum += *it;
        }
    }
    //sum *= 0xffff;
    sum *= -1;
    commandData.push_back(static_cast<uint8_t>(sum >> 8));                      // Checksum higher 8 bits
    commandData.push_back(static_cast<uint8_t>(sum & 0x00ff));                  // Checksum lower 8 bits
}

// ToDo: split messages by special tokens and data lengths
void DFPlayer::ReadResponse()
{
    std::string logSource = "DFPlayer::ReadResponse";
    //Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Reading message...");
    std::vector<uint8_t> response;
    m_serialPort->ReadBytes(response);

    std::string outputHex = "";
    HexDump(response, outputHex);
    if (!outputHex.empty())
    {
        Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "...received response: " + outputHex);
    }
}

void DFPlayer::HexDump(const char* buffer, size_t bufferSize, std::string& output)
{
    output = "";
    static char buff[16];
    for (size_t i = 0; i < bufferSize; ++i)
    {
        //sprintf(buff, "\\x%02x", buffer[i]);
        sprintf(buff, "%02x ", buffer[i]);
        output += buff;
    }
}

void DFPlayer::HexDump(const std::vector<uint8_t>&buffer, std::string& output)
{
    output = "";
    static char buff[16];
    for (auto byte : buffer)
    {
        //sprintf(buffer, "\\x%02x", byte);
        sprintf(buff, "%02x ", byte);
        output += buff;
    }
}
