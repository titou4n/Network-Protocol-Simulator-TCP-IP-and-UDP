#include "TCPPacket.hpp"

TCPPacket::TCPPacket()
    : TCPPacket(0, 0, "") {}

TCPPacket::TCPPacket(int src, int dest)
    : TCPPacket(src, dest, "") {}

TCPPacket::TCPPacket(int src, int dest, const std::string& data)
    : Packet(src, dest, data)
{
    seq = 0;
    syn = false;
    ack = false;
    fin = false;
}