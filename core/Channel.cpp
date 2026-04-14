#include "Channel.hpp"
#include "Node.hpp"
#include "Packet.hpp"

#include <cstdlib>
#include <chrono>
#include <thread>
#include <iostream>

#include "../tools/Wireshark.hpp"

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

void Channel::transmit(Packet packet)
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

    // CORRUPTION
    if ((rand() % 100) < corruption_rate * 100)
    {
        packet.corrupted = true;

        if (activate_wireshark)
            Wireshark::logPacket(packet, "CORRUPTED");
    }

    // DELAY (simulation simple)
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

    destination->receive(packet);

    if (activate_wireshark)
        Wireshark::logPacket(packet, "DELIVERED");
}