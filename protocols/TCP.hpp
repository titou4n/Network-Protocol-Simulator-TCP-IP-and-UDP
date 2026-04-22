#ifndef TCP_HPP
#define TCP_HPP

#include <map>
#include <iostream>
#include <algorithm>
#include <string>

#include "../packets/TCPPacket.hpp"
#include "../core/Channel.hpp"
#include "../core/Node.hpp"

enum TCPState {
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RECEIVED,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,
    LAST_ACK,
    TIME_WAIT
};

class TCP
{
private:
    Channel channel;

    int next_seq_number;
    int expected_seq_number;
    int last_ack_received;
    bool waiting_for_ack;

    TCPState state;
    std::map<int, TCPPacket> sent_packets;

    // timeout/retransmission
    TCPPacket last_control_packet;
    std::chrono::time_point<std::chrono::steady_clock> last_send_time;
    int timeout_ms;

    bool print_log;

public:

    TCP(Channel& channel);
    TCP(Channel& channel, int timeout_ms);
    TCP(Channel& channel, bool print_log);

    void listen();

    void connect(int id_node1, int id_node2);
    void connect(Node& node1, Node& node2);
    void disconnect(int source, int destination);
    void disconnect(int source, int destination, TCPState add_state);

    void send(TCPPacket& packet);
    void sendSpecialPacket(TCPPacket& packet);
    void receive(TCPPacket& packet);

    void sendAck(TCPPacket& packet);
    void handleAck(int ack_number);

    void retransmit(Channel& channel);

    std::string getTCPState();
    std::string getTCPState(int state);

    bool isWaitingForAck();
    void checkTimeout(Channel& channel);

    bool isConnecting();
    bool isConnected();
    bool isDisconnecting();
    bool isDisconnected();

    void printLog(std::string message);
    void printLogReceivePacket(Packet& packet, std::string message);
};

#endif