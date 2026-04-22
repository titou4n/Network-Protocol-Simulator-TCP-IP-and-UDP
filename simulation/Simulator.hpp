#pragma once

#include <queue>
#include <vector>
#include <string>
#include <functional>

#include "Event.hpp"
#include "../core/Channel.hpp"
#include "../core/Node.hpp"
#include "../protocols/TCP.hpp"
#include "../protocols/UDP.hpp"
#include "../packets/TCPPacket.hpp"
#include "../packets/UDPPacket.hpp"

class Simulator {

private:

    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> eventQueue;
    double currentTime;
    int timeout;

    void processEvent(const Event& event, Channel& channel,
                        Node& client, Node& server,
                        TCP& tcp_client, TCP& tcp_server,
                        UDP& udp_client, UDP& udp_server);

    std::vector<std::string> splitIntoChunks(const std::string& data, size_t chunkSize);

public:
    Simulator();

    void schedule(EventType type, double timestamp, int srcId, int destId, const std::string& data = "");

    void run();
};
