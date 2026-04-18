#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>

#include "../packets/Packet.hpp"
#include "../packets/UDPPacket.hpp"
#include "../packets/TCPPacket.hpp"
#include "Channel.hpp"

class Channel;
class TCP;

class Node {
private:
    int id;

public:
    Channel* channel;
    TCP* tcp;
    
    Node(int id, Channel& channel);

    int getId() const;
    void setTCP(TCP* t); // link tcp_server to Node server

    void send(const Packet& packet);
    void receive(const Packet& packet);

    void sendUDP(const UDPPacket& packet);
    void receiveUDP(const UDPPacket& packet);

    void sendTCP(const TCPPacket& packet);
    void receiveTCP(const TCPPacket& packet);
};

#endif