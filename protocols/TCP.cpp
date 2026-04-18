#include "TCP.hpp"

TCP::TCP(Channel& channel)
{
    this->channel = channel;
    this->next_seq_number = 1;
    this->expected_seq_number = 1;
    this->last_ack_received = 0;
    this->waiting_for_ack = false;

    this->state = CLOSED;

    this->last_control_packet = TCPPacket();
    this->last_send_time = std::chrono::steady_clock::now();
    this->timeout_ms = 500;
}

TCP::TCP(Channel& channel, int timeout) : TCP(channel)
{
    this->timeout_ms = timeout;
}


void TCP::listen()
{
    if (state != CLOSED)
    {
        std::cout << "[TCP] Cannot listen: state=" << getTCPState() << "\n";
        return;
    }
    state = LISTEN;
    std::cout << "[TCP] SERVER: Listening...\n";
}

void TCP::connect(int id_node1, int id_node2)
{
    if (state != CLOSED)
    {
        std::cout << "[TCP] Cannot connect: state=" << getTCPState() << std::endl;
        return;
    }
    
    state = SYN_SENT;
    std::cout << "[TCP] CLIENT: Sending SYN\n";

    TCPPacket syn(id_node1, id_node2);
    syn.syn = true;
    syn.ack = false;

    last_control_packet = syn;
    last_send_time      = std::chrono::steady_clock::now();
    channel.transmit(syn);
}

void TCP::connect(Node& node1, Node& node2)
{
    connect(node1.getId(),node2.getId());
    return;
}

void TCP::send(TCPPacket& packet)
{
    std::cout << "[TCP] [SEND] [NODE "<< packet.destination<<"] [STATE=" << getTCPState() << "]" << std::endl;
    std::cout << std::endl;
    if (state != ESTABLISHED)
    {
        if (state == CLOSED)
        {
            std::cout << "[TCP] Not connected → starting handshake\n";
            connect(packet.source, packet.destination);
        }
        else
        {
            std::cout << "[TCP] Handshake in progress → cannot send data\n";
            return;
        }
    }
    if (waiting_for_ack) return;

    packet.seq = next_seq_number;
    packet.syn = false;
    packet.ack = false;

    std::cout << "[TCP] [NODE " << packet.source << "] SEND seq=" << packet.seq << "\n";

    waiting_for_ack = true;
    last_send_time = std::chrono::steady_clock::now();

    sent_packets.emplace(packet.seq, packet);
    channel.transmit(packet);
    next_seq_number++;
}

void TCP::sendSpecialPacket(TCPPacket& packet)
{
    std::cout << "[TCP] [NODE "<< packet.source <<"] [SEND] Special Packet [STATE=" << getTCPState() << "]" << std::endl;

    last_control_packet = packet;
    last_send_time = std::chrono::steady_clock::now();
    channel.transmit(packet);
}
void TCP::receive(TCPPacket& packet)
{
    std::cout << "[TCP] [NODE "<< packet.destination<<"] [RECEIVE] [STATE=" << getTCPState() << "]" << std::endl;

    // Paquet ignoré si la connexion est fermée
    // Packet ignored if connection is closed
    if (state == CLOSED)
    {
        // Pour le dernier packet
        // For the last packet
        if (packet.fin)
        {
            std::cout << "[TCP] [NODE " << packet.destination << "] FIN received in CLOSED → sending ACK (server still waiting)\n";
            TCPPacket ack(packet.destination, packet.source);
            ack.fin = false;
            ack.ack = true;
            channel.transmit(ack);
        }
        else
        {
            std::cout << "[TCP] [NODE "<< packet.destination<<"] Packet ignored [STATE=CLOSED]\n";
        }
        return;
    }

    // =========== CONNEXION / CONNECTION =========== //

    // 1. SYN reçu → répondre avec SYN-ACK (serveur)
    //    SYN received → reply with SYN-ACK (server)
    if (packet.syn && !packet.ack && (state == LISTEN || state == SYN_RECEIVED))
    {
        state = SYN_RECEIVED;
        std::cout << "[TCP] [NODE " << packet.destination << "] SYN received → sending SYN-ACK" << std::endl;

        TCPPacket syn_ack(packet.destination, packet.source);
        syn_ack.syn = true;
        syn_ack.ack = true;

        sendSpecialPacket(syn_ack);
        return;
    }

    // 2. SYN-ACK reçu → répondre avec ACK (client)
    //    SYN-ACK received → reply with ACK (client)
    if (packet.syn && packet.ack && (state == SYN_SENT || state == ESTABLISHED))
    {
        std::cout << "[TCP] [NODE " << packet.destination << "] SYN-ACK received → sending ACK" << std::endl;

        TCPPacket ack(packet.destination, packet.source);
        ack.syn = false;
        ack.ack = true;

        sendSpecialPacket(ack);
        state = ESTABLISHED;
        return;
    }

    // 3. ACK final reçu → connexion établie (serveur en SYN_RECEIVED)
    //    Final ACK received → connection established (server side)
    if (packet.ack && !packet.syn && state == SYN_RECEIVED)
    {
        state = ESTABLISHED;
        std::cout << "[TCP] [NODE " << packet.destination << "] ACK received → [CONNECTION ESTABLISHED]\n" << std::endl;
        return;
    }

    // =========== DÉCONNEXION / DISCONNECTION =========== //

    // 4. FIN reçu en ESTABLISHED → envoyer ACK puis FIN (serveur)
    //    FIN received in ESTABLISHED → send ACK then FIN (server)
    if (packet.fin && !packet.ack && state == ESTABLISHED)
    {
        //state = CLOSE_WAIT;
        std::cout << "[TCP] SERVER: FIN received → sending ACK + FIN\n";

        TCPPacket ack(packet.destination, packet.source);
        ack.fin = false;
        ack.ack = true;
        sendSpecialPacket(ack);

        state = LAST_ACK;
        disconnect(packet.destination, packet.source, LAST_ACK);
        return;
    }

    // 5. FIN reçu en FIN_WAIT_1 → l'ACK serveur a été droppé, fermeture rapide
    //    FIN received in FIN_WAIT_1 → server ACK was dropped, fast-path close
    if (packet.fin && !packet.ack && state == FIN_WAIT_1)
    {
        std::cout << "[TCP] CLIENT: FIN received in FIN_WAIT_1 → sending ACK → CLOSING\n";

        TCPPacket ack(packet.destination, packet.source);
        ack.fin = false;
        ack.ack = true;

        state = CLOSED;
        sendSpecialPacket(ack);
        return;
    }

    // 6. ACK du FIN reçu → passer en FIN_WAIT_2 (client)
    //    ACK of FIN received → move to FIN_WAIT_2 (client)
    if (packet.ack && !packet.fin && state == FIN_WAIT_1)
    {
        state = FIN_WAIT_2;
        std::cout << "[TCP] CLIENT: ACK of FIN received\n";
        return;
    }

    // 7. FIN du serveur reçu en FIN_WAIT_2 → envoyer ACK final (client)
    //    Server FIN received in FIN_WAIT_2 → send final ACK (client)
    if (packet.fin && state == FIN_WAIT_2)
    {
        std::cout << "[TCP] CLIENT: FIN received → sending ACK → TIME_WAIT\n";

        TCPPacket ack(packet.destination, packet.source);
        ack.fin = false;
        ack.ack = true;

        state = CLOSED;
        sendSpecialPacket(ack);
        return;
    }

    // 8. ACK final reçu → fermeture complète (serveur en LAST_ACK)
    //    Final ACK received → connection fully closed (server in LAST_ACK)
    if (packet.ack && state == LAST_ACK)
    {
        state = CLOSED;
        std::cout << "[TCP] SERVER: CONNECTION CLOSED\n";
        return;
    }

    // =========== DONNÉES / DATA =========== //

    // 9. ACK de données reçu → libérer le paquet du buffer (client)
    //    Data ACK received → release packet from send buffer (client)
    if (packet.ack && !packet.syn && state == ESTABLISHED)
    {
        std::cout << "[TCP] [NODE " << packet.destination
                  << "] ACK received for seq=" << packet.seq - 1
                  << " (next expected=" << packet.seq << ")" << std::endl;
        handleAck(packet.seq);
        std::cout << std::endl;
        return;
    }

    // Paquet de contrôle inattendu dans cet état → ignoré
    // Unexpected control packet in this state → ignored
    if (state != ESTABLISHED)
    {
        std::cout << "[TCP] [NODE " << packet.destination << "] Packet ignored [STATE=" << getTCPState() << "]\n";
        return;
    }

    // Paquet corrompu → ignoré (le timeout déclenchera la retransmission)
    // Corrupted packet → ignored (timeout will trigger retransmission)
    if (packet.corrupted)
    {
        std::cout << "[TCP] [NODE " << packet.destination << "] [CORRUPTED PACKET] packet ignored" << std::endl;
        return;
    }

    // Paquet déjà reçu (doublon) → renvoyer un ACK quand même
    // Duplicate packet → re-send ACK anyway
    if (packet.seq < expected_seq_number)
    {
        std::cout << "[TCP] [NODE " << packet.destination << "] DATA ALREADY received seq=" << packet.seq << " | SEND ACK" << std::endl;
        sendAck(packet);
        return;
    }

    // Paquet dans l'ordre → ACK et livraison
    // In-order packet → ACK and deliver
    if (packet.seq == expected_seq_number)
    {
        std::cout << "[TCP] [NODE " << packet.destination << "] DATA received seq=" << packet.seq << " | SEND ACK" << std::endl;
        std::cout << "[TCP] [DATA SEQ=" << packet.seq << "] '" << packet.data << "'" << std::endl;

        expected_seq_number++;
        sendAck(packet);
        return;
    }

    // Paquet hors ordre → demander une retransmission
    // Out-of-order packet → request retransmission
    std::cout << "[TCP] [NODE " << packet.destination << "] Out of order (expected "
              << expected_seq_number << " got " << packet.seq << ")" << std::endl;
    retransmit(channel);
}

// Send ACK
void TCP::sendAck(TCPPacket& packet)
{
    TCPPacket ack = TCPPacket(packet.destination, packet.source);
    ack.syn = false;
    ack.ack = true;
    ack.fin = false;
    ack.seq = expected_seq_number;
    channel.transmit(ack);
}


// ACK HANDLER
void TCP::handleAck(int ack_seq)
{
    // Erase packet inside buffer
    sent_packets.erase(ack_seq - 1);
    last_ack_received = ack_seq;
    waiting_for_ack = false;
}

// RETRANSMISSION
void TCP::retransmit(Channel& channel)
{
    std::cout << "[TCP] [RETRANSMITTING] Retransmitting packets...\n";

    for (auto& [seq, packet] : sent_packets)
    {
        std::cout << "[TCP] [RETRANSMITTING] Resend seq=" << seq << "\n";
        channel.transmit(packet);
    }
}

// DISCONNECT

void TCP::disconnect(int source, int destination)
{
    disconnect(source, destination, FIN_WAIT_1);
    return;
}

void TCP::disconnect(int source, int destination, TCPState add_state)
{
    if (state != ESTABLISHED)
    {
        // seul disconnect() interne peut passer ici via l'overload avec TCPState
        if (state != LAST_ACK) {
            std::cout << "[TCP] Cannot disconnect: state=" << getTCPState() << "\n";
            return;
        }
    }

    std::cout << "[TCP] [DISCONNECTING....] [CURRENT STATE : "<< getTCPState() <<"] ";
    state = add_state;
    std::cout <<  "[NEW STATE : "<<getTCPState()<<"]" << std::endl;

    TCPPacket fin = TCPPacket(source, destination);
    fin.syn = false;
    fin.ack = false;
    fin.fin = true;

    sendSpecialPacket(fin);
}

std::string TCP::getTCPState()
{
    return getTCPState(state);
}

std::string TCP::getTCPState(int tcp_state)
{
    switch (tcp_state) {
        case CLOSED:       return "CLOSED";
        case LISTEN:       return "LISTEN";
        case SYN_SENT:     return "SYN_SENT";
        case SYN_RECEIVED: return "SYN_RECEIVED";
        case ESTABLISHED:  return "ESTABLISHED";
        case FIN_WAIT_1:   return "FIN_WAIT_1";
        case FIN_WAIT_2:   return "FIN_WAIT_2";
        case CLOSE_WAIT:   return "CLOSE_WAIT";
        case CLOSING:      return "CLOSING";
        case LAST_ACK:     return "LAST_ACK";
        case TIME_WAIT:    return "TIME_WAIT";
        default:           return "UNKNOWN";
    }
}

bool TCP::isWaitingForAck() {
    return waiting_for_ack;
}

void TCP::checkTimeout(Channel& channel)
{
    auto now     = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>
                   (now - last_send_time).count();

    if (elapsed <= timeout_ms) return;

    // Timeout pendant l'envoi de données
    if (waiting_for_ack)
    {
        int seq = next_seq_number - 1;
        if (sent_packets.count(seq))
        {
            std::cout << "[TCP] TIMEOUT seq=" << seq << " → retransmitting data\n";
            channel.transmit(sent_packets.at(seq));
            last_send_time = std::chrono::steady_clock::now();
        }
        return;
    }

    // Timeout pendant la connexion (SYN perdu)
    if (state == SYN_SENT)
    {
        std::cout << "[TCP] TIMEOUT SYN → retransmitting SYN\n";
        channel.transmit(last_control_packet);
        last_send_time = std::chrono::steady_clock::now();
        return;
    }

    // Timeout pendant la connexion (SYN-ACK perdu)
    if (state == SYN_RECEIVED)
    {
        std::cout << "[TCP] TIMEOUT SYN-ACK → retransmitting SYN-ACK\n";
        channel.transmit(last_control_packet);
        last_send_time = std::chrono::steady_clock::now();
        return;
    }

    // Timeout pendant la déconnexion (FIN perdu)
    if (state == FIN_WAIT_1 || state == LAST_ACK)
    {
        std::cout << "[TCP] TIMEOUT FIN → retransmitting FIN\n";
        channel.transmit(last_control_packet);
        last_send_time = std::chrono::steady_clock::now();
        return;
    }
}

bool TCP::isConnecting() {
    return state == SYN_SENT || state == SYN_RECEIVED;
}

bool TCP::isConnected() {
    return state == ESTABLISHED;
}

bool TCP::isDisconnecting() {
    return state == FIN_WAIT_1 || 
           state == FIN_WAIT_2 || 
           state == CLOSE_WAIT || 
           state == CLOSING    || 
           state == LAST_ACK   || 
           state == TIME_WAIT;
}

bool TCP::isDisconnected() {
    return state == CLOSED;
}