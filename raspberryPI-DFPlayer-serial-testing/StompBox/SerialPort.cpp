#include "SerialPort.h"
#include "Logger.h"
//#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>


// ToDo: implement (buffer the data!)
bool SerialPort::ReadResponse(std::vector<uint8_t>& response)
{
    std::string logSource = "SerialPort::ReadResponse";
    const size_t bufferSize = 32;
    static uint8_t readBuff[bufferSize];
    if (m_fd < 0)
    {
        Logger::Log(Logger::ELogLevel::Log_Error, logSource, "Port not opened");
        return false;
    }
    while(true)
    {
        size_t readBytes = read(m_fd, readBuff, bufferSize);
        for (size_t i = 0; i < readBytes; ++i)
        {
            m_readBytes.push(readBuff[i]);
            response.push_back(readBuff[i]);
        }
        if (readBytes < bufferSize)
        {
            break;
        }
    }

    return true;
}

// ToDo: configure port from config parameter
bool SerialPort::Initialize(const std::string& port, const std::string& config)
{
    std::string logSource = "SerialPort::Initialize";
    if ((!m_port.empty() && (m_port.compare(port) != 0)) ||
        (!m_portConfig.empty() && (m_portConfig.compare(config) != 0)))
    {
        Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Closing port");
        Close();
    }

    if (m_fd < 0)
    {
        Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Opening port: " + port);
        m_port = port;
        m_portConfig = config;
        m_fd = open(m_port.c_str(), O_RDWR | O_NONBLOCK);
        //m_fd = open("/dev/ttyAMA0", O_RDWR);
        if (m_fd < 0)
        {
            printf("Error %i from open: %s\n", errno, strerror(errno));
            return false;
        }
        if (tcgetattr(m_fd, &m_tty) != 0)
        {
            Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Port could not be opened (1)");
            printf("Error %i from open: %s\n", errno, strerror(errno));
        }
        else
        {
            int a = EACCES;
        }

        // TBD
        // configure port parameters using m_portConfig
    }
    if (m_fd < 0)
    {
        Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Port could not be opened (2)");
        return false;
    }
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Port initialized OK");
    return true;
}

bool SerialPort::WriteMessage(const std::vector<uint8_t>& messageData) const
{
    std::string logSource = "SerialPort::WriteMessage";
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Writing message...");

    if (m_fd < 0)
    {
        Logger::Log(Logger::ELogLevel::Log_Error, logSource, "Port not opened");
        return false;
    }
    if (messageData.size() == 0)
    {
        Logger::Log(Logger::ELogLevel::Log_Error, logSource, "Message is empty");
        return false;
    }
    size_t dataSize = messageData.size();
    std::vector<uint8_t>::const_iterator it = messageData.begin();
    const uint8_t* buffer = static_cast<const uint8_t*>(&(*it));
    if (write(m_fd, static_cast<const void*>(buffer), dataSize) != dataSize)
    {
        Logger::Log(Logger::ELogLevel::Log_Error, logSource, "Writing to port failed");
        return false;
    }

    /*if (flush(m_fd) != 0)
    {
        Logger::Log(Logger::ELogLevel::Log_Error, logSource, "Flush failed");
        return false;
    }*/
    /*if (std::ferror(m_file))
    {
        Logger::Log(Logger::ELogLevel::Log_Error, logSource, "Port error");
        return false;
    }*/
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Message written OK");
    return true;
}

SerialPort::SerialPort() :
    m_fd(-1),
    m_port("")
{
    std::string logSource = "SerialPort::SerialPort";
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "created");
}

SerialPort::~SerialPort()
{
    std::string logSource = "SerialPort::~SerialPort";
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Destructing SerialPort");
    Close();
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "SerialPort destructed");
}

void SerialPort::Close()
{
    std::string logSource = "SerialPort::ClosePort";
    if (m_fd >= 0)
    {
        Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Closing port");
        close(m_fd);
    }
    m_fd = -1;
}
