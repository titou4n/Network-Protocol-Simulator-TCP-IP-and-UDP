#ifndef PACKET_HPP
#define PACKET_HPP

#include <string>

class Packet {

private:
    static int nextId;

public:
    int id;
    int source;
    int destination;
    std::string data;

    bool corrupted=false;

    Packet(int src, int dest, const std::string& data);

    virtual std::string getProtocol() const {
        return "NONE";
    }

    virtual ~Packet() = default;

    static int generatePacketId();
};

#endif