#pragma once
//#define _POSIX_SOURCE

#include <string>
#include <vector>
#include <queue>
#include <mutex>

#include <termios.h>
#include <fcntl.h>

class SerialPort
{
public:
    bool Initialize(const std::string& port, const speed_t speed);
    bool Initialize(const std::string& port, const std::string& config);
    void Close();
    bool WriteBytes(const std::vector<uint8_t>& message);
    bool ReadBytes(std::vector<uint8_t>& response);

    SerialPort();
    ~SerialPort();

protected:
    int m_fd;
    std::string m_port;
    speed_t m_speed;
    struct termios m_tty;
    std::queue<uint8_t> m_readQueue;
    std::queue<uint8_t> m_writeQueue;

    const char c_configStringSeparator = ';';
    const int c_defaultOpenFlags = O_RDWR | O_NONBLOCK;
    const speed_t c_defaultSpeed = B9600;
    mutable std::mutex m_readPortAccess;
    mutable std::mutex m_writePortAccess;
    mutable std::mutex m_readQueueAccess;
    mutable std::mutex m_writeQueueAccess;
};

