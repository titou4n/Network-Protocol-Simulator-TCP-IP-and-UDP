#include "Node.hpp"


Node::Node(int id, Channel& channel) {
    this->id = id;
    this->channel = &channel;
}

int Node::getId() const{
    return id;
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
    std::cout << "[NODE " << id << "] Received TCP packet "
              << "from " << packet.source << std::endl;

    // Vérification corruption
    if (packet.corrupted)
    {
        std::cout << "[NODE " << id << "] Packet corrupted → ignored" << std::endl;
        return;
    }

    // --- Gestion des flags TCP ---

    // Received SYN -> answer SYN-ACK
    if (packet.syn && !packet.ack)
    {
        std::cout << "[NODE " << id << "] SYN received → sending SYN-ACK" << std::endl;

        TCPPacket tcp_packet_response(id,packet.source,"SYN-ACK");

        tcp_packet_response.syn = true;
        tcp_packet_response.ack = true;

        sendTCP(tcp_packet_response);
        return;
    }

    // Received SYN-ACK -> answer ACK
    if (packet.syn && packet.ack)
    {
        std::cout << "[NODE " << id << "] SYN-ACK received → sending ACK" << std::endl;

        TCPPacket tcp_packet_response(id, packet.source, "ACK");

        tcp_packet_response.syn = false;
        tcp_packet_response.ack = true;

        sendTCP(tcp_packet_response);
        return;
    }

    // Received ACK -> [CONNEXION ETABLISHED]
    if (packet.ack)
    {
        std::cout << "[NODE " << id << "] ACK received → connection established" << std::endl;
        return;
    }

    // Données normales
    std::cout << "[NODE " << id << "] Data received: "
              << packet.data << std::endl;
}