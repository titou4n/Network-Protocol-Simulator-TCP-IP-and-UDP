#include "UDPPacket.hpp"

UDPPacket::UDPPacket(int src, int dest, const std::string& data)
    : Packet(src, dest, data) {}