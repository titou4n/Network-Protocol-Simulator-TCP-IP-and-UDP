#ifndef WIRESHARK_HPP
#define WIRESHARK_HPP

#include <iostream>
#include <chrono>
#include <iomanip>
#include <string>

#include "../packets/Packet.hpp"
#include "../packets/TCPPacket.hpp"
#include "../packets/UDPPacket.hpp"

class Wireshark {
public:
    static void logPacket(const Packet& packet, const std::string& status);
};

#endif