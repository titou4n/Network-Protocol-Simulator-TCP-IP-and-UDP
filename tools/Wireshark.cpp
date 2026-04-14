#include "Wireshark.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

static long getTimeMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}

void Wireshark::logPacket(const Packet& p, const std::string& status) {

    std::cout << "----------------------------------------\n";
    std::cout << "[t=" << getTimeMs() << " ms]\n";
    std::cout << "Packet ID: " << p.id << "\n";
    std::cout << "From: " << p.source << " → To: " << p.destination << "\n";
    std::cout << "Data: " << p.data << "\n";
    std::cout << "Status: " << status << "\n";
    std::cout << "Corrupted: " << (p.corrupted ? "YES" : "NO") << "\n";
    std::cout << "----------------------------------------\n\n";
}