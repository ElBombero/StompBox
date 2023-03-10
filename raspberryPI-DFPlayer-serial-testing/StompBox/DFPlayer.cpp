#include "DFPlayer.h"

#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
//#include <stdio.h>
//#include <fcntl.h>
//#include <unistd.h>


void DFPlayer::SendCommand(std::vector<uint8_t>& commandData, ECommand command, uint16_t paramWL = 0, uint8_t paramH = 0, bool feedback = false)
{
    uint8_t paramL = 0;
    commandData.clear();

    commandData.push_back(static_cast<uint8_t>(ESpecialTokens::Token_Start));   // $S 
    commandData.push_back(static_cast<uint8_t>(command));                       // command ID
    commandData.push_back(0x06);                                                // instruction length (excluding bytes $S and $O)
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
        paramL = static_cast<uint8_t>(paramWL & 0x00ff);
    }
    commandData.push_back(feedback ? 0x01 : 0x00);
    commandData.push_back(paramH);
    commandData.push_back(paramL);

    CalculateCommandChecksum(commandData);
    commandData.push_back(static_cast<uint8_t>(ESpecialTokens::Token_End));   // $O

    Instance().WriteCommand(commandData);
}

DFPlayer& DFPlayer::Instance()
{
    static std::string s_initialization;
    static DFPlayer s_instance;
    return s_instance;
}

bool DFPlayer::WriteCommand(const std::vector<uint8_t>& commandData)
{
    if (m_file == nullptr)
    {
        return false;
    }
    size_t dataSize = commandData.size();
    if (fwrite(static_cast<const void*>(&commandData.begin()), dataSize, dataSize, m_file) != dataSize)
    {
        return false;
    }
    fflush(m_file);
    if (std::ferror(m_file))
    {
        return false;
    }
    return true;
}

// ToDo: implement (buffer the data!)
bool DFPlayer::ReadResponse()
{
    // TBD
    return false;
}

// ToDo: configure port from config parameter
bool DFPlayer::Initialize(std::string port, std::string config)
{
    if ((!m_port.empty() && (m_port.compare(port) != 0)) ||
        (!m_portConfig.empty() && (m_portConfig.compare(config) != 0)))
    {
        ClosePort();
    }
    if (m_file == nullptr)
    {
        m_port = port;
        m_portConfig = config;
        m_file = fopen(m_port.c_str(), "r+");
        // TBD
        // configure port parameters using m_portConfig
    }
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
    sum *= -1;
    commandData.push_back(static_cast<uint8_t>(sum >> 8));                      // Checksum higher 8 bits
    commandData.push_back(static_cast<uint8_t>(sum & 0x00ff));                  // Checksum lower 8 bits
}


DFPlayer::DFPlayer() :
    m_port(""),
    m_file(nullptr)
{
}

DFPlayer::~DFPlayer()
{
    ClosePort();
}

void DFPlayer::ClosePort()
{
    if (m_file != nullptr)
    {
        fclose(m_file);
    }
    m_file = nullptr;
}
