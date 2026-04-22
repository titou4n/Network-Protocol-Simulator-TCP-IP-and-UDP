#include "Node.hpp"
#include "../protocols/UDP.hpp"
#include "../protocols/TCP.hpp"

Node::Node(int id, Channel& channel) {
    this->id = id;
    this->channel = &channel;
    this->udp = new UDP(channel);
    this->tcp = new TCP(channel);
}

int Node::getId() const{
    return id;
}

void Node::setUDP(UDP* u) {
    udp = u;
}

void Node::setTCP(TCP* t) {
    tcp = t;
}

void Node::send(const Packet& packet) {

    std::cout << "[NODE " << id << "] Sending packet "
              << "to " << packet.destination << std::endl;

    if (!channel)
    {
        std::cerr << "[NODE " << id << "] ERROR: No channel connected" << std::endl;
        return;
    }

    channel->transmit(packet);
}

void Node::receive(const Packet& packet) {

    if (packet.corrupted)
    {
       std::cout << "[CORRUPTED] Node " << id
                  << " received CORRUPTED packet from node "
                  << packet.source << std::endl;
    }
    return;
}


void Node::sendUDP(const UDPPacket& packet) {
    //channel->transmit(packet);
    send(packet);
}

void Node::receiveUDP(const UDPPacket& packet) {

    if (packet.corrupted)
    {
       std::cout << "[CORRUPTED] Node " << id
                  << " received CORRUPTED packet from node "
                  << packet.source << std::endl;
    }
    return;
}


void Node::sendTCP(const TCPPacket& packet)
{
    //channel->transmit(packet);
    send(packet);
}

void Node::receiveTCP(const TCPPacket& packet)
{
    // We delegate all the logic to TCP::receive
    //tcp->receive(const_cast<TCPPacket&>(packet), *channel);
    TCPPacket copy = packet;
    tcp->receive(copy);
}