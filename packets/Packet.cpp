#include <iostream>
#include <string>
using namespace std;

#include "Packet.hpp"

int Packet::nextId = 0;

Packet::Packet(int src, int dest, const std::string& data)
{
    this->id = generatePacketId();
    this->source = src;
    this->destination = dest;
    this->data = data;
    this->corrupted=false;
}

int Packet::generatePacketId() {
    return ++nextId;
}