#ifndef EVENT_HPP
#define EVENT_HPP

#include "../packets/Packet.hpp"
#include "../core/Channel.hpp"


class Event {
public:
    int time;
    Packet packet;

    Event(int t, Packet p);
};

#endif