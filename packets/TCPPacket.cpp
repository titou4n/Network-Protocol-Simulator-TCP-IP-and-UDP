#include "TCPPacket.hpp"

TCPPacket::TCPPacket(int src, int dest, const std::string& data)
    : Packet(src, dest, data)
{
    seq = 0;
    syn = false;
    ack = false;
}