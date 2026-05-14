#pragma once
#include <string>
#include "../packets/TCPPacket.hpp"
#include "../packets/UDPPacket.hpp"

enum class EventType {
    // UDP
    UDP_SEND,
    UDP_RECEIVE,

    // TCP Handshake
    TCP_CONNECT,        // Triggers the SYN
    TCP_LISTEN,         // Puts the server in listening mode

    // TCP Data
    TCP_SEND,           // Sends a data chunk
    TCP_RECEIVE,        // Receives a TCP packet
    TCP_ACK,            // ACK received
    TCP_TIMEOUT,        // Timeout → retransmission

    // TCP Disconnect
    TCP_DISCONNECT,

    // Simulation
    SIM_END             // Stops the simulation
};

struct Event {
    double      timestamp;      // Simulated time (ms)
    EventType   type;           // Event type
    int         sourceId;       // Sender node
    int         destId;         // Destination node
    std::string data;           // Payload (for TCP_SEND / UDP_SEND)

    // Operator for min-heap (smaller timestamp = higher priority)
    bool operator>(const Event& other) const {
        return timestamp > other.timestamp;
    }
};