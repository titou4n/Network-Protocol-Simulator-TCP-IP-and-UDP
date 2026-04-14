#include "Node.hpp"
#include "Channel.hpp"
#include "Packet.hpp"
#include <iostream>

Node::Node(int id) {
    this->id = id;
}

int Node::getId() const{
    return id;
}

void Node::send(Packet packet, Channel& channel) {
    channel.transmit(packet);
}

void Node::receive(Packet packet) {
    if (packet.corrupted)
    {
        std::cout << "Node " << id
                  << " received CORRUPTED packet from node "
                  << packet.source << std::endl;
        return;
    }

    std::cout << "Node " << id
              << " received packet from node "
              << packet.source << std::endl;
}