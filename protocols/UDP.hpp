#ifndef UDP_HPP
#define UDP_HPP

#include "../packets/Packet.hpp"
#include "../core/Channel.hpp"

class UDP {
private:
    Channel channel;

public:
    UDP(Channel& channel);

    void send(UDPPacket& packet);
    void receive(UDPPacket& packet);
};

#endif