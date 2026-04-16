#ifndef TCP_HPP
#define TCP_HPP

#include <map>
#include "../packets/TCPPacket.hpp"
#include "../core/Channel.hpp"
#include "../core/Node.hpp"

class TCP
{
private:
    int next_seq_number;
    int expected_seq_number;
    int last_ack_received;

    std::map<int, TCPPacket> sent_packets;

    enum TCPState
    {
        CLOSED,
        SYN_SENT,
        SYN_RECEIVED,
        ESTABLISHED
    };

    TCPState state;

public:
    TCP();

    void connect(Node& node1, Node& node2, Channel& channel);
    void disconnect();

    void send(TCPPacket& packet, Channel& channel);
    void receive(TCPPacket& packet, Channel& channel);

    //void sendAck(int seq, Channel& channel);
    void handleAck(int ack_number);

    void retransmit(Channel& channel);

};

#endif