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
public:
    Simulator();

    // Planifie un événement dans la file
    void schedule(EventType type, double timestamp,
                  int srcId, int destId,
                  const std::string& data = "");

    // Lance la boucle de simulation
    void run();

private:
    // File d'événements triée par timestamp (min-heap)
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> eventQueue;

    double currentTime = 0.0;

    // Traitement d'un événement
    void processEvent(const Event& event,
                      Channel& channel,
                      Node& client, Node& server,
                      TCP& tcp_client, TCP& tcp_server,
                      UDP& udp);

    // Utilitaire : découpe data en chunks
    std::vector<std::string> splitIntoChunks(const std::string& data, size_t chunkSize);
};
