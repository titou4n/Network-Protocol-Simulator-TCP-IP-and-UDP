#include "../core/Packet.hpp"
#include "../core/Channel.hpp"
#include "../core/Node.hpp"

/*
Rôle :

Protocole fiable (le plus complexe)

Tu implémentes :

1. Connexion
SYN
SYN-ACK
ACK

2. Envoi fiable
numéros de séquence
ACK
retransmission

*/

class TCP {
public:
    int sequence_number = 0;

    /*
    
    void send(Packet packet, Channel& channel, Node& dest) {
        packet.id = sequence_number++;
        channel.transmit(packet, dest);
    }

    void receive(Packet packet) {
        if (!packet.corrupted) {
            sendAck(packet.id, ...);
    }
            */
};