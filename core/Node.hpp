#ifndef NODE_HPP
#define NODE_HPP

#include "Packet.hpp"

class Channel;

class Node {
private:
    int id;

public:
    Node(int id);

    int getId() const;

    void send(Packet packet, Channel& channel);
    void receive(Packet packet);
};

#endif