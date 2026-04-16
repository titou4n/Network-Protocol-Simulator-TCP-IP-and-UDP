#ifndef TCPPACKET_HPP
#define TCPPACKET_HPP

#include "Packet.hpp"
#include <string>

class TCPPacket : public Packet {
public:
    int seq = 0;
    bool syn = true;
    bool ack = false;

    //TCPPacket();

    TCPPacket(Packet& packet);

    TCPPacket(int src, int dest, const std::string& data);

    std::string getProtocol() const override {
        return "TCP";
    }
};

#endif