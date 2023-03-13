#pragma once
#define _POSIX_SOURCE

#include <string>
#include <vector>
#include <queue>

//#include <cstdint>
//#include <cstdlib>
//#include <fstream>
#include <termios.h>
#include <fcntl.h>

class SerialPort
{
public:
    // ToDo: configure port from config parameter
    bool Initialize(const std::string& port, const speed_t speed);
    bool Initialize(const std::string& port, const std::string& config);
    void Close();
    bool WriteMessage(const std::vector<uint8_t>& message) const;
    // ToDo: implement (buffer the data!)
    bool ReadResponse(std::vector<uint8_t>& response);

    SerialPort();
    ~SerialPort();

protected:
    //FILE* m_file;
    int m_fd;
    std::string m_port;
    speed_t m_speed;
    struct termios m_tty;
    std::queue<uint8_t> m_readBytes;

    const char c_configStringSeparator = ';';
    const int c_defaultOpenFlags = O_RDWR | O_NONBLOCK;
    const speed_t c_defaultSpeed = B9600;
};

