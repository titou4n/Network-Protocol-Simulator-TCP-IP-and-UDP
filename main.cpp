#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// Core
#include "core/Node.hpp"
#include "core/Channel.hpp"

// Packets
#include "packets/Packet.hpp"
#include "packets/TCPPacket.hpp"
#include "packets/UDPPacket.hpp"

// Protocols
#include "protocols/TCP.hpp"
#include "protocols/UDP.hpp"

// Simulation
#include "simulation/Simulator.hpp"
#include "simulation/Event.hpp"

// Tools
#include "tools/Logger.hpp"
#include "tools/Metrics.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

int main()
{
    std::srand(std::time(nullptr));

    // =====================================================
    // 🌐 CONFIGURATION DU RÉSEAU
    // =====================================================
    double loss_rate = 0.1;        // 10% packet loss
    double delay = 100;            // latency simulée (ms)
    double corruption = 0.05;      // 5% corruption
    bool wireshark = true;

    Channel channel = Channel(loss_rate, delay, corruption, wireshark);

    // =====================================================
    // 🖥️ CRÉATION DES NŒUDS
    // =====================================================
    Node client(1, channel);
    Node server(2, channel);

    channel.add_node(client);
    channel.add_node(server);

    std::cout << "\n========== UDP TEST ==========\n";
    //UDPPacket udp_packet(1, 1, 2, "Hello UDP");
    UDPPacket udp_packet = UDPPacket(1, 2, "Hello UDP");

    UDP udp;
    udp.send(udp_packet, channel);


    std::cout << "\n========== TCP TEST ==========\n";

    TCP tcp_client;

    // 1. Connexion TCP (SYN → SYN-ACK → ACK)
    tcp_client.connect(client, server, channel);

    // =====================================================
    // 📦 ENVOI DE DONNÉES TCP
    // =====================================================

    int source1 = 1;
    int destination1 = 2;
    std::string data1 = "Hello TCP 1";
    TCPPacket tcp_packet1(source1, destination1, data1);
    
    int source2 = 1;
    int destination2 = 2;
    std::string data2 = "Hello TCP 2";
    TCPPacket tcp_packet2(source2, destination2, data2);

    /*
    tcp_client.send(tcp_packet1, channel);
    tcp_client.send(tcp_packet2, channel);

    // =====================================================
    // 🔁 SIMULATION RETRANSMISSION
    // =====================================================
    tcp_client.retransmit(channel);

    // =====================================================
    // 📴 DISCONNECT
    // =====================================================
    tcp_client.disconnect();
    */

    return 0;
}