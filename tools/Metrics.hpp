#ifndef METRICS_HPP
#define METRICS_HPP

class Metrics {
private:
    int packets_sent;
    int packets_received;
    int packets_lost;
    double total_latency;

public:
    Metrics();

    void packetSent();
    void packetReceived(double latency);
    void packetLost();

    double getThroughput() const;
    double getAverageLatency() const;
    double getPacketLossRate() const;
};

#endif