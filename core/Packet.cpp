#include <iostream>
#include <string>
using namespace std;

#include "Packet.hpp"

Packet::Packet(int id, int src, int dest, const std::string& data)
{
    this->id=id;
    this->source=src;
    this->destination=dest;
    this->data=data;
    this->corrupted=false;
}