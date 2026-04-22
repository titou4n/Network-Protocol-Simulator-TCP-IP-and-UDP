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

    this->print_log = true;
}

TCP::TCP(Channel& channel, int timeout) : TCP(channel)
{
    this->timeout_ms = timeout;
}

TCP::TCP(Channel& channel, bool print_log) : TCP(channel)
{
    this->print_log = print_log;
}


void TCP::listen()
{
    if (state != CLOSED)
    {
        printLog("Cannot listen [STATE is not CLOSED]");
        return;
    }
    state = LISTEN;
    printLog("SERVER: Listening...");
}

void TCP::connect(int id_node1, int id_node2)
{
    if (state != CLOSED)
    {
        printLog("Cannot connect [STATE is not CLOSED]");
        return;
    }
    
    state = SYN_SENT;
    printLog("CLIENT: Sending SYN");

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
    // for print log :
    std::ostringstream oss; 

    if (state != ESTABLISHED)
    {
        if (state == CLOSED)
        {
            printLog("Not connected → starting handshake");
            connect(packet.source, packet.destination);
        }
        else
        {
            printLog("Handshake in progress → cannot send data");
            return;
        }
    }

    if (waiting_for_ack)
        return;

    packet.seq = next_seq_number;
    packet.syn = false;
    packet.ack = false;

    oss << "[NODE " << packet.source << "] [SEND] seq=" << packet.seq;
    printLog(oss.str());

    waiting_for_ack = true;
    last_send_time = std::chrono::steady_clock::now();

    sent_packets.emplace(packet.seq, packet);
    channel.transmit(packet);
    next_seq_number++;
}

void TCP::sendSpecialPacket(TCPPacket& packet)
{
    printLog("[SEND] Special Packet");

    last_control_packet = packet;
    last_send_time = std::chrono::steady_clock::now();
    channel.transmit(packet);
}

void TCP::receive(TCPPacket& packet)
{
    // for print log :
    std::ostringstream oss; 

    // Paquet ignoré si la connexion est fermée
    // Packet ignored if connection is closed
    if (state == CLOSED)
    {
        // Pour le dernier packet
        // For the last packet
        if (packet.fin)
        {
            printLogReceivePacket(packet, "FIN received in CLOSED → sending ACK (server still waiting)");
            TCPPacket ack(packet.destination, packet.source);
            ack.fin = false;
            ack.ack = true;
            channel.transmit(ack);
        }
        else
        {
            printLogReceivePacket(packet, "Packet ignored [STATE=CLOSED]");
        }
        return;
    }

    // =========== CONNECTION =========== //

    // 1. SYN received → reply with SYN-ACK (server)
    if (packet.syn && !packet.ack && (state == LISTEN || state == SYN_RECEIVED))
    {
        state = SYN_RECEIVED;
        printLogReceivePacket(packet, "SYN received → sending SYN-ACK");

        TCPPacket syn_ack(packet.destination, packet.source);
        syn_ack.syn = true;
        syn_ack.ack = true;

        sendSpecialPacket(syn_ack);
        return;
    }

    // 2. SYN-ACK received → reply with ACK (client)
    if (packet.syn && packet.ack && (state == SYN_SENT || state == ESTABLISHED))
    {
        printLogReceivePacket(packet, "SYN-ACK received → sending ACK");

        TCPPacket ack(packet.destination, packet.source);
        ack.syn = false;
        ack.ack = true;

        sendSpecialPacket(ack);
        state = ESTABLISHED;
        return;
    }

    // 3. Final ACK received → connection established (server side)
    if (packet.ack && !packet.syn && state == SYN_RECEIVED)
    {
        state = ESTABLISHED;
        printLogReceivePacket(packet, "ACK received → [CONNECTION ESTABLISHED]\n");
        return;
    }

    // =========== DISCONNECTION =========== //

    // 4. FIN received in ESTABLISHED → send ACK then FIN (server)
    if (packet.fin && !packet.ack && state == ESTABLISHED)
    {
        //state = CLOSE_WAIT;
        printLogReceivePacket(packet, "FIN received → sending ACK + FIN");

        TCPPacket ack(packet.destination, packet.source);
        ack.fin = false;
        ack.ack = true;
        sendSpecialPacket(ack);

        state = LAST_ACK;
        disconnect(packet.destination, packet.source, LAST_ACK);
        return;
    }

    // 5. FIN received in FIN_WAIT_1 → server ACK was dropped, fast-path close
    if (packet.fin && !packet.ack && state == FIN_WAIT_1)
    {
        printLogReceivePacket(packet, "FIN received in FIN_WAIT_1 → sending ACK → CLOSING");

        TCPPacket ack(packet.destination, packet.source);
        ack.fin = false;
        ack.ack = true;

        state = CLOSED;
        sendSpecialPacket(ack);
        return;
    }

    // 6. ACK of FIN received → move to FIN_WAIT_2 (client)
    if (packet.ack && !packet.fin && state == FIN_WAIT_1)
    {
        state = FIN_WAIT_2;
        printLogReceivePacket(packet, "ACK of FIN received");
        return;
    }

    // 7. Server FIN received in FIN_WAIT_2 → send final ACK (client)
    if (packet.fin && state == FIN_WAIT_2)
    {
        printLogReceivePacket(packet, "FIN received → sending ACK → TIME_WAIT");

        TCPPacket ack(packet.destination, packet.source);
        ack.fin = false;
        ack.ack = true;

        state = CLOSED;
        sendSpecialPacket(ack);
        return;
    }

    // 8. Final ACK received → connection fully closed (server in LAST_ACK)
    if (packet.ack && state == LAST_ACK)
    {
        state = CLOSED;
        printLogReceivePacket(packet, "[CONNECTION CLOSED]\n");
        return;
    }

    // =========== DATA =========== //

    // 9. Data ACK received → release packet from send buffer (client)
    if (packet.ack && !packet.syn && state == ESTABLISHED)
    {
        oss << "ACK received for seq=" << (packet.seq - 1) << " (next expected=" << packet.seq << ")";
        printLogReceivePacket(packet, oss.str());

        handleAck(packet.seq);
        std::cout << std::endl;
        return;
    }

    // Unexpected control packet in this state → ignored
    if (state != ESTABLISHED)
    {
        printLogReceivePacket(packet, "Packet ignored [CONNECTION NOT ESTABLISHED]");
        return;
    }

    // Corrupted packet → ignored (timeout will trigger retransmission)
    if (packet.corrupted)
    {
        printLogReceivePacket(packet, "Packet ignored [CORRUPTED PACKET]");
        return;
    }

    // Duplicate packet → re-send ACK anyway
    if (packet.seq < expected_seq_number)
    {
        oss << "DATA ALREADY received seq=" << packet.seq << " | SEND ACK";
        printLogReceivePacket(packet, oss.str());

        sendAck(packet);
        return;
    }

    // In-order packet → ACK and deliver
    if (packet.seq == expected_seq_number)
    {
        oss << "DATA received seq=" << packet.seq << " | SEND ACK" << " | DATA [" << packet.data << "]";
        printLogReceivePacket(packet, oss.str());

        expected_seq_number++;
        sendAck(packet);
        return;
    }

    // Out-of-order packet → request retransmission
    oss << "Out of order (expected "<< expected_seq_number << " got " << packet.seq << ")";
    printLogReceivePacket(packet, oss.str());

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
    // for printLog:
    std::ostringstream oss; 

    printLog("[RETRANSMITTING] Retransmitting packets...");

    for (auto& [seq, packet] : sent_packets)
    {
        oss << "[RETRANSMITTING] Resend seq=" << seq;
        printLog(oss.str());

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
        if (state != LAST_ACK) {
            printLog("Cannot disconnect");
            return;
        }
    }

    printLog("[DISCONNECTING....]");
    state = add_state;
    printLog("[DISCONNECTING....]");

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
    // for printLog:
    std::ostringstream oss; 

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
            oss << "TIMEOUT seq=" << seq << " → retransmitting data";
            printLog(oss.str());

            channel.transmit(sent_packets.at(seq));
            last_send_time = std::chrono::steady_clock::now();
        }
        return;
    }

    // Timeout pendant la connexion (SYN perdu)
    if (state == SYN_SENT)
    {
        printLog("TIMEOUT SYN → retransmitting SYN");
        channel.transmit(last_control_packet);
        last_send_time = std::chrono::steady_clock::now();
        return;
    }

    // Timeout pendant la connexion (SYN-ACK perdu)
    if (state == SYN_RECEIVED)
    {
        printLog("TIMEOUT SYN-ACK → retransmitting SYN-ACK");
        channel.transmit(last_control_packet);
        last_send_time = std::chrono::steady_clock::now();
        return;
    }

    // Timeout pendant la déconnexion (FIN perdu)
    if (state == FIN_WAIT_1 || state == LAST_ACK)
    {
        printLog("TIMEOUT FIN → retransmitting FIN");
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

void TCP::printLog(std::string message)
{
    if (print_log)
    {
        std::cout << "[TCP] [STATE=" << getTCPState() << "] " << message << std::endl;
    }
}

void TCP::printLogReceivePacket(Packet& packet, std::string message)
{
    if (print_log)
    {
        std::cout << "[TCP] [STATE=" << getTCPState() << "] [NODE "<< packet.destination <<"] [RECEIVE] " << message << std::endl;
    }
}