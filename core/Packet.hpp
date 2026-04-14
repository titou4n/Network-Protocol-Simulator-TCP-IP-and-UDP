#ifndef PACKET_HPP
#define PACKET_HPP

#include <string>

class Packet {
public:
    int id;
    int source;
    int destination;
    std::string data;

    bool corrupted;

    Packet(int id, int src, int dest, const std::string& data);
};

#endif