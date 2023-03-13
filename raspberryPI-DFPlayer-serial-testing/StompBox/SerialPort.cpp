#include "SerialPort.h"
#include "Logger.h"
//#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <sstream>


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
bool SerialPort::Initialize(const std::string& port, const speed_t speed)
{
    std::string logSource = "SerialPort::Initialize";
    if ((!m_port.empty() && (m_port.compare(port) != 0)) ||
        (speed != m_speed))
    {
        Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Closing port");
        Close();
    }

    if (m_fd < 0)
    {
        Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Opening port: " + port);
        m_port = port;
        m_speed = speed;
        m_fd = open(m_port.c_str(), c_defaultOpenFlags);
        //m_fd = open("/dev/ttyAMA0", O_RDWR);
        if (m_fd < 0)
        {
            Logger::Log(Logger::ELogLevel::Log_Error, logSource, "Port could not be opened");
            printf("Error %i from open: %s\n", errno, strerror(errno));
            return false;
        }
        memset(&m_tty, 0, sizeof m_tty);
        Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Port opened OK");
        if (tcgetattr(m_fd, &m_tty) != 0)
        {
            printf("Error %i from open: %s\n", errno, strerror(errno));
            Logger::Log(Logger::ELogLevel::Log_Error, logSource, "Port configuration could not be read");
            return false;
        }
        cfsetospeed(&m_tty, speed);
        cfsetispeed(&m_tty, speed);

        m_tty.c_cflag &= ~PARENB;            // Make 8n1
        m_tty.c_cflag &= ~CSTOPB;
        m_tty.c_cflag &= ~CSIZE;
        m_tty.c_cflag |= CS8;

        m_tty.c_cflag &= ~CRTSCTS;           // no flow control
        m_tty.c_cc[VMIN] = 1;                // read doesn't block
        m_tty.c_cc[VTIME] = 5;               // 0.5 seconds read timeout
        m_tty.c_cflag |= CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

        cfmakeraw(&m_tty);                   // Make raw

        tcflush(m_fd, TCIFLUSH);             // Flush Port, then apply the attributes
        if (tcsetattr(m_fd, TCSANOW, &m_tty) != 0)
        {
            Logger::Log(Logger::ELogLevel::Log_Error, logSource, "Port could not be configured");
            return false;
        }
        else
        {
            Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Port configured OK");
        }
    }
    Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Port initialized OK");
    return true;
}

bool SerialPort::Initialize(const std::string& port, const std::string& config)
{
    std::string logSource = "SerialPort::Initialize(2)";
    std::vector<std::string> strings;
    std::istringstream f(config);
    std::string s;
    while (getline(f, s, c_configStringSeparator))
    {
        //cout << s << endl;
        strings.push_back(s);
    }
    speed_t speed = c_defaultSpeed;
    if (strings.size() >= 1)
    {
        std::string speedStr = strings[0];
        if (speedStr == "9600") speed = B9600;              // The most common setting first
        else if (speedStr == "50") speed = B50;
        else if (speedStr == "75") speed = B75;
        else if (speedStr == "110") speed = B110;
        else if (speedStr == "134") speed = B134;
        else if (speedStr == "150") speed = B150;
        else if (speedStr == "200") speed = B200;
        else if (speedStr == "300") speed = B300;
        else if (speedStr == "600") speed = B600;
        else if (speedStr == "1800") speed = B1800;
        else if (speedStr == "2400") speed = B2400;
        else if (speedStr == "4800") speed = B4800;
        else if (speedStr == "19200") speed = B19200;
        else if (speedStr == "38400") speed = B38400;
        else if (speedStr == "57600") speed = B57600;
        else if (speedStr == "115200") speed = B115200;
        else if (speedStr == "230400") speed = B230400;
        else if (speedStr == "460800") speed = B460800;
        else if (speedStr == "500000") speed = B500000;
        else if (speedStr == "576000") speed = B576000;
        else if (speedStr == "921600") speed = B921600;
        else if (speedStr == "1000000") speed = B1000000;
        else if (speedStr == "1152000") speed = B1152000;
        else if (speedStr == "1500000") speed = B1500000;
        else if (speedStr == "2000000") speed = B2000000;
        else if (speedStr == "2500000") speed = B2500000;
        else if (speedStr == "3000000") speed = B3000000;
        else if (speedStr == "3500000") speed = B3500000;
        else if (speedStr == "4000000") speed = B4000000;
        Logger::Log(Logger::ELogLevel::Log_Debug, logSource, "Speed: " + speedStr);
    }

    Initialize(port, speed);
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
