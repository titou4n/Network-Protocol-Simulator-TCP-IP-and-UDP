#ifndef UDPPACKET_HPP
#define UDPPACKET_HPP

#include "Packet.hpp"
#include <string>

class UDPPacket : public Packet {
public:
    UDPPacket(int src, int dest, const std::string& data);

    std::string getProtocol() const override {
        return "UDP";
    }
};

#endif