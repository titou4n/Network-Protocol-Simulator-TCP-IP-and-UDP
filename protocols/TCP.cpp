#include "TCP.hpp"
#include <iostream>
#include <algorithm>

/*
=========================================================
        TCP SIMULÉ - VERSION PÉDAGOGIQUE PROPRE
=========================================================

États :
- CLOSED
- SYN_SENT
- SYN_RECEIVED
- ESTABLISHED

Handshake :
1. SYN
2. SYN-ACK
3. ACK

Ensuite :
- envoi DATA fiable (SEQ + ACK)
=========================================================
*/

TCP::TCP()
{
    next_seq_number = 1;
    expected_seq_number = 1;
    last_ack_received = 0;

    state = CLOSED;
}


void TCP::connect(Node& node1, Node& node2, Channel& channel)
{
    if (state != CLOSED)
        return;

    std::cout << "\n[TCP] CLIENT: Sending SYN\n";

    TCPPacket packet_syn(node1.getId(), node2.getId(), "SYN");
    //packet_syn.protocol = "TCP";
    packet_syn.syn = true;
    packet_syn.ack = false;
    packet_syn.seq = 0;

    state = SYN_SENT;

    channel.transmit(packet_syn);
}



void TCP::send(TCPPacket& packet, Channel& channel)
{
    if (state != ESTABLISHED)
    {
        std::cout << "[TCP] Not connected → starting handshake\n";
        Node node1 = Node(packet.source, channel);
        Node node2 = Node(packet.destination, channel);
        connect(node1, node2, channel);
        return;
    }

    packet.seq = next_seq_number;
    packet.syn = false;
    packet.ack = false;
    //packet.protocol = "TCP";

    sent_packets.emplace(packet.seq, packet);

    std::cout << "[TCP] SEND seq=" << packet.seq << "\n";

    channel.transmit(packet);

    next_seq_number++;
}

void TCP::receive(TCPPacket& packet, Channel& channel)
{

    std::cout << "Received ...";
    //
    // =========================
    // 1. SYN reçu (serveur)
    // =========================
    //
    if (packet.syn && !packet.ack && state == CLOSED)
    {
        std::cout << "[TCP] SERVER: SYN received\n";

        state = SYN_RECEIVED;

        TCPPacket syn_ack = TCPPacket(packet);
        //syn_ack.protocol = "TCP";
        syn_ack.syn = true;
        syn_ack.ack = true;
        syn_ack.seq = 0;

        std::cout << "[TCP] SERVER: Sending SYN-ACK\n";

        channel.transmit(syn_ack);
        return;
    }

    //
    // =========================
    // 2. SYN-ACK reçu (client)
    // =========================
    //
    if (packet.syn && packet.ack && state == SYN_SENT)
    {
        std::cout << "[TCP] CLIENT: SYN-ACK received\n";

        TCPPacket ack = TCPPacket(packet);
        //ack.protocol = "TCP";
        ack.syn = false;
        ack.ack = true;
        ack.seq = 0;

        std::cout << "[TCP] CLIENT: Sending ACK → ESTABLISHED\n";

        channel.transmit(ack);

        state = ESTABLISHED;
        return;
    }

    //
    // =========================
    // 3. ACK final reçu (serveur)
    // =========================
    //
    if (packet.ack && !packet.syn && state == SYN_RECEIVED)
    {
        std::cout << "[TCP] SERVER: Final ACK received → ESTABLISHED\n";

        state = ESTABLISHED;
        return;
    }

    //
    // =========================
    // 4. ACK de DATA
    // =========================
    //
    if (packet.ack && !packet.syn)
    {
        handleAck(packet.seq);
        return;
    }

    //
    // =========================
    // 5. DATA RECEIVED
    // =========================
    //
    if (state != ESTABLISHED)
    {
        std::cout << "[TCP] Packet ignored (not established)\n";
        return;
    }

    if (packet.corrupted)
    {
        std::cout << "[TCP] Corrupted packet ignored\n";
        return;
    }

    if (packet.seq == expected_seq_number)
    {
        std::cout << "[TCP] DATA received seq=" << packet.seq << "\n";

        //sendAck(packet.seq, channel);

        expected_seq_number++;
    }
    else
    {
        std::cout << "[TCP] Out of order packet (expected "
                  << expected_seq_number
                  << " got " << packet.seq << ")\n";
    }
}


/*
//
// 📤 ENVOI ACK
//
void TCP::sendAck(int seq, Channel& channel)
{
    Packet packet = Packet()
    TCPPacket ack = TCPPacket(packet);

    //ack.protocol = "TCP";
    ack.syn = false;
    ack.ack = true;
    ack.seq = seq;

    std::cout << "[TCP] Sending ACK seq=" << seq << "\n";

    channel.transmit(ack);
}
*/

// ACK HANDLER
void TCP::handleAck(int ack_number)
{
    std::cout << "[TCP] ACK RECEIVED seq=" << ack_number << "\n";

    sent_packets.erase(ack_number);
    last_ack_received = ack_number;
}

// RETRANSMISSION
void TCP::retransmit(Channel& channel)
{
    std::cout << "[TCP] Retransmitting packets...\n";

    for (auto& [seq, packet] : sent_packets)
    {
        std::cout << "[TCP] Resend seq=" << seq << "\n";
        channel.transmit(packet);
    }
}

// DISCONNECT
void TCP::disconnect()
{
    state = CLOSED;
    std::cout << "[TCP] Disconnected\n";
}