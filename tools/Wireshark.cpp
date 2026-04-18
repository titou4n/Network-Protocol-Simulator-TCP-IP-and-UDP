#include "Wireshark.hpp"

static long getTimeMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
    ).count();
}
    /*
    std::cout << "----------------------------------------\n";
    std::cout << "[t=" << getTimeMs() << " ms]\n";
    std::cout << "Packet ID: " << p.id << "\n";
    std::cout << "From: " << p.source << " → To: " << p.destination << "\n";
    std::cout << "Data: " << p.data << "\n";
    std::cout << "Status: " << status << "\n";
    std::cout << "Corrupted: " << (p.corrupted ? "YES" : "NO") << "\n";
    std::cout << "----------------------------------------\n\n";
    */


void Wireshark::logPacket(const Packet& packet, const std::string& status)
{
    // Couleurs ANSI
    const std::string RESET  = "\033[0m";
    const std::string RED    = "\033[31m";
    const std::string GREEN  = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE   = "\033[34m";
    const std::string CYAN   = "\033[36m";

    std::cout << CYAN << "[t=" << getTimeMs() << " ms] " << RESET;

    std::cout << "[";

    // Protocol (en bleu)
    std::cout << BLUE << packet.getProtocol() << RESET;

    // TCP FLAGS
    if (const TCPPacket* tcp = dynamic_cast<const TCPPacket*>(&packet))
    {
        if (tcp->syn)
            std::cout << YELLOW << "|SYN" << RESET;

        if (tcp->ack)
            std::cout << GREEN << "|ACK" << RESET;

        if (tcp->fin)
            std::cout << RED << "|FIN" << RESET;
    }

    std::cout << "] ";

    // Status en couleur
    std::string statusColor = RESET;
    if (status == "DROPPED")
        statusColor = RED;
    else if (status == "DELIVERED")
        statusColor = GREEN;
    else if (status == "TRANSMITTING")
        statusColor = YELLOW;

    // Infos packet
    std::cout << "ID=" << packet.id
              << " FROM=" << packet.source
              << " TO=" << packet.destination
              << " STATUS=" << statusColor << status << RESET;

    if (packet.corrupted)
        std::cout << " " << RED << "[CORRUPTED]" << RESET;

    std::cout << std::endl;
}