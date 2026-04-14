#ifndef WIRESHARK_HPP
#define WIRESHARK_HPP

#include <string>
#include "../core/Packet.hpp"

class Wireshark {
public:
    static void logPacket(const Packet& p, const std::string& status);
};

#endif