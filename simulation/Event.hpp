#pragma once
#include <string>
#include "../packets/TCPPacket.hpp"
#include "../packets/UDPPacket.hpp"

enum class EventType {
    // UDP
    UDP_SEND,
    UDP_RECEIVE,

    // TCP Handshake
    TCP_CONNECT,        // Déclenche le SYN
    TCP_LISTEN,         // Met le serveur en écoute

    // TCP Data
    TCP_SEND,           // Envoi d'un chunk de données
    TCP_RECEIVE,        // Réception d'un paquet TCP
    TCP_ACK,            // ACK reçu
    TCP_TIMEOUT,        // Timeout → retransmission

    // TCP Disconnect
    TCP_DISCONNECT,

    // Simulation
    SIM_END             // Arrêt de la simulation
};

struct Event {
    double      timestamp;      // Temps simulé (ms)
    EventType   type;           // Nature de l'événement
    int         sourceId;       // Nœud émetteur
    int         destId;         // Nœud destinataire
    std::string data;           // Payload (pour TCP_SEND / UDP_SEND)

    // Opérateur pour min-heap (plus petit timestamp = priorité haute)
    bool operator>(const Event& other) const {
        return timestamp > other.timestamp;
    }
};
