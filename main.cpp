#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>

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
    Simulator sim = Simulator();
    sim.run();
    return 0;
}