#include "Simulator.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

Simulator::Simulator() {
    this->currentTime = 0.0;
    this->timeout = 10; // in seconds
}

void Simulator::schedule(EventType type, double timestamp, int srcId, int destId, const std::string& data)
{
    Event event;
    event.type      = type;
    event.timestamp = timestamp;
    event.sourceId  = srcId;
    event.destId    = destId;
    event.data      = data;
    eventQueue.push(event);
}

std::vector<std::string> Simulator::splitIntoChunks(const std::string& data, size_t chunkSize)
{
    std::vector<std::string> chunks;
    for (size_t i = 0; i < data.size(); i += chunkSize)
        chunks.push_back(data.substr(i, chunkSize));
    return chunks;
}

void Simulator::run()
{
    std::srand(std::time(nullptr));

    // ── Network configuration ─────────────────
    double loss_rate  = 0.1;
    double delay      = 100.0;
    double corruption = 0.0;
    bool   wireshark  = true;

    Channel channel(loss_rate, delay, corruption, wireshark);

    // ── Node setup ────────────────────────────
    Node client(1, channel);
    Node server(2, channel);

    channel.add_node(client);
    channel.add_node(server);

    // ── Protocol instances ────────────────────
    UDP udp_client = UDP(channel);
    UDP udp_server = UDP(channel);

    server.setUDP(&udp_server);
    client.setUDP(&udp_client);

    bool print_log_TCP = false;
    TCP tcp_client = TCP(channel, print_log_TCP);
    TCP tcp_server = TCP(channel, print_log_TCP);
    
    server.setTCP(&tcp_server);
    client.setTCP(&tcp_client);

    // ── Event scheduling ──────────────────────
    double t = 0.0;

    // 1. Simple UDP test
    schedule(EventType::UDP_SEND, t, 1, 2, "Hello UDP");
    t += delay;

    // 2. TCP handshake: server listens, then client connects
    schedule(EventType::TCP_LISTEN,  t,       2, 1);
    schedule(EventType::TCP_CONNECT, t + 1.0, 1, 2);
    t += delay * 2;

    // 3. Send payload as TCP chunks
    std::string payload   = "Network Protocol Simulator with TCP/IP and UDP (1-2 students)";
    size_t      chunkSize = 10;
    auto chunks = splitIntoChunks(payload, chunkSize);

    for (size_t i = 0; i < chunks.size(); i++)
        schedule(EventType::TCP_SEND, t + i * delay, 1, 2, chunks[i]);

    t += chunks.size() * delay;

    // 4. TCP teardown
    schedule(EventType::TCP_DISCONNECT, t, 1, 2);

    // 5. End of simulation
    schedule(EventType::SIM_END, t + delay, 0, 0);

    // ── Event processing loop ─────────────────
    std::cout << "========== SIMULATION START ==========\n\n";

    while (!eventQueue.empty())
    {
        Event event = eventQueue.top();
        eventQueue.pop();

        currentTime = event.timestamp;
        std::cout << "[t=" << currentTime << "ms] ";

        processEvent(event, channel, client, server, tcp_client, tcp_server, udp_client, udp_server);
    }

    std::cout << "========== SIMULATION END ==========\n";
}

// ──────────────────────────────────────────────
// Process a single event
// ──────────────────────────────────────────────
void Simulator::processEvent(const Event& event,
                              Channel& channel,
                              Node& client, Node& server,
                              TCP& tcp_client, TCP& tcp_server,
                              UDP& udp_client, UDP& udp_server)
{
    switch (event.type)
    {
        // ── UDP ──────────────────────────────
        case EventType::UDP_SEND:
        {
            // I will not use it but if server need to send
            (void)udp_server;

            std::cout << "========== UDP TEST ==========\n";
            UDPPacket packet(event.sourceId, event.destId, event.data);
            udp_client.send(packet);
            break;
        }

        // ── TCP handshake ─────────────────────
        case EventType::TCP_LISTEN:
        {
            std::cout << "========== TCP LISTEN ==========\n";
            tcp_server.listen();
            break;
        }

        case EventType::TCP_CONNECT:
        {
            std::cout << "========== TCP CONNECT ==========\n";
            tcp_client.connect(client, server);

            auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(timeout);
            while (!tcp_client.isConnected() || !tcp_server.isConnected())
            {
                if (std::chrono::steady_clock::now() > deadline)
                {
                    std::cout << "[SIM] Connection timeout after "<< timeout<<" seconds" << std::endl;
                    return;
                }
                tcp_client.checkTimeout(channel);
                tcp_server.checkTimeout(channel);
            }
            break;
        }

        // ── TCP data transfer ─────────────────
        case EventType::TCP_SEND:
        {
            std::cout << "TCP_SEND [" << event.data << "]\n";
            TCPPacket packet(event.sourceId, event.destId, event.data);
            tcp_client.send(packet);

            auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(timeout);
            while (tcp_client.isWaitingForAck())
            {
                if (std::chrono::steady_clock::now() > deadline)
                {
                    std::cout << "\n[SIM] Send timeout after "<< timeout<<" seconds" << std::endl;
                    return;
                }
                tcp_client.checkTimeout(channel);
                tcp_server.checkTimeout(channel);
            }
            break;
        }

        // ── TCP teardown ──────────────────────
        case EventType::TCP_DISCONNECT:
        {
            std::cout << "\n========== TCP DISCONNECT ==========\n";
            tcp_client.disconnect(event.sourceId, event.destId);

            auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(timeout);
            while (!tcp_client.isDisconnected() || !tcp_server.isDisconnected())
            {
                if (std::chrono::steady_clock::now() > deadline)
                {
                    std::cout << "[SIM] Disconnect timeout after "<< timeout<<" seconds" << std::endl;
                    return;
                }
                tcp_client.checkTimeout(channel);
                tcp_server.checkTimeout(channel);
            }
            break;
        }

        // ── End of simulation ─────────────────
        case EventType::SIM_END:
            std::cout << "\nSIM_END" << std::endl;
            break;

        default:
            std::cout << "\n[Simulator] Unknown event type" << std::endl;
            break;
    }
}