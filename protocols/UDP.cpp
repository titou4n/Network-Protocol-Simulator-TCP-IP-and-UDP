#include "UDP.hpp"

UDP::UDP() {}

void UDP::send(UDPPacket& packet, Channel& channel)
{
    //packet.protocol = "UDP";
    channel.transmit(packet);
}

void UDP::receive(UDPPacket& packet)
{
    std::cout << "[NODE"<<packet.destination<<"] Received [UDP] packet from "<< packet.source << std::endl;
    
    // =========================
    // DATA RECEIVED
    // =========================

    if (packet.corrupted)
    {
        std::cout << "[UDP] Corrupted packet ignored";
        return;
    }
}
