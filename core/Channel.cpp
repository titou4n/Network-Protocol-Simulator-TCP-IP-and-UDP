#include "Channel.hpp"

Channel::Channel(double loss,
                 double delay,
                 double corruption,
                 bool activate_wireshark)
{
    this->loss_rate = loss;
    this->delay = delay;
    this->corruption_rate = corruption;
    this->activate_wireshark = activate_wireshark;
}

void Channel::add_node(Node& node)
{
    nodes[node.getId()] = &node;
}

void Channel::transmit(const Packet& packet)
{
    if (activate_wireshark)
        Wireshark::logPacket(packet, "TRANSMITTING");

    // LOSS
    if ((rand() % 100) < loss_rate * 100)
    {
        if (activate_wireshark)
            Wireshark::logPacket(packet, "DROPPED");
        return;
    }

    bool shouldCorrupt = false;

    // CORRUPTION
    if ((rand() % 100) < corruption_rate * 100)
    {
        shouldCorrupt = true;

        if (activate_wireshark)
            Wireshark::logPacket(packet, "CORRUPTED");
    }

    // DELAY
    std::this_thread::sleep_for(
        std::chrono::milliseconds((int)delay)
    );

    // ROUTING
    auto it = nodes.find(packet.destination);

    if (it == nodes.end())
    {
        if (activate_wireshark)
            Wireshark::logPacket(packet, "UNKNOWN DESTINATION");
        return;
    }
    Node* destination = it->second;

    if (activate_wireshark)
        Wireshark::logPacket(packet, "DELIVERED");

    // DISPATCH
    if (const TCPPacket* tcp = dynamic_cast<const TCPPacket*>(&packet))
    {
        TCPPacket copy = *tcp;
        if (shouldCorrupt) copy.corrupted = true;

        destination->receiveTCP(copy);
    }
    else if (const UDPPacket* udp = dynamic_cast<const UDPPacket*>(&packet))
    {
        UDPPacket copy = *udp;
        if (shouldCorrupt) copy.corrupted = true;

        destination->receiveUDP(copy);
    }
    else
    {
        Packet copy = packet;
        if (shouldCorrupt) copy.corrupted = true;

        destination->receive(copy);
    }
}