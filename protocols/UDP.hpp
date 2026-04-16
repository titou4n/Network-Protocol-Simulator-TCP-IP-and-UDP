#ifndef UDP_HPP
#define UDP_HPP

#include "../packets/Packet.hpp"
#include "../core/Channel.hpp"

class UDP {
public:
    UDP();

    void send(UDPPacket& packet, Channel& channel);
    void receive(UDPPacket& packet);
};

#endif