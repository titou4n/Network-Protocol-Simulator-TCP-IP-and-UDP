#ifndef TCP_HPP
#define TCP_HPP

#include "../core/Packet.hpp"
#include "../core/Channel.hpp"
#include "../core/Node.hpp"

class Node;

class TCP {
private:
    int sequence_number;

public:
    TCP();

    //void connect(Node& sender, Node& receiver, Channel& channel);

    //void send(Packet packet, Channel& channel, Node& destination);

    //void receive(Packet packet);

    //void sendAck(int packet_id, Channel& channel, Node& destination);
};

#endif