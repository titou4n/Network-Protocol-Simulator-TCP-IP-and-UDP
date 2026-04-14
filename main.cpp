#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

// Core
#include "core/Node.hpp"
#include "core/Packet.hpp"
#include "core/Channel.hpp"

// Protocols
#include "protocols/TCP.hpp"
#include "protocols/UDP.hpp"

// Simulation
#include "simulation/Simulator.hpp"
#include "simulation/Event.hpp"

// Tools
#include "tools/Logger.hpp"
#include "tools/Metrics.hpp"

int main() {

    // Config Chanel - Network
    double loss_rate = 0.1;     // ex: 0.1 = 10%
    double delay = 100;         // en ms
    double corruption_rate = 0.05;
    bool activate_wireshark = true;
    Channel channel(loss_rate, delay, corruption_rate, activate_wireshark);


    int id_source = 1;
    int id_destination = 2;

    Node ComputerA(id_source);
    Node ComputerB(id_destination);
    channel.add_node(ComputerA);
    channel.add_node(ComputerB);

    int id_packet = 1;
    std::string data = "Hello";

    Packet p(id_packet, id_source, id_destination, data);

    ComputerA.send(p, channel);
    
    return 0;
}