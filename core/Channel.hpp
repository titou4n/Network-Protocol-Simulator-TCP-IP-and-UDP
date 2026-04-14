#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>

#include "Packet.hpp"

class Node;

class Channel {
private:
    std::map<int, Node*> nodes;

    double loss_rate;
    double delay;
    double corruption_rate;

    bool activate_wireshark;

public:
    Channel(double loss,
            double delay,
            double corruption,
            bool activate_wireshark);

    void transmit(Packet packet);

    void add_node(Node& node);
};

#endif