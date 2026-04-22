#include "UDP.hpp"

UDP::UDP(Channel& channel) {
    this->channel = channel;
}

void UDP::send(UDPPacket& packet)
{
    channel.transmit(packet);
}

void UDP::receive(UDPPacket& packet)
{
    std::cout << "[NODE"<<packet.destination<<"] Received [UDP] packet from "<< packet.source << std::endl;
    
    if (packet.corrupted)
    {
        std::cout << "[UDP] Corrupted packet ignored";
        return;
    }
}
