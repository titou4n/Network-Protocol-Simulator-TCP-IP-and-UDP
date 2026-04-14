#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include <vector>
#include "Event.hpp"

class Simulator {
private:
    int current_time;
    std::vector<Event> events;

public:
    Simulator();

    void addEvent(const Event& event);
    void run();

    int getCurrentTime() const;
};

#endif