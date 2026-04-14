# Network Protocol Simulator (TCP/IP & UDP)

## Overview

This project is a **network protocol simulator written in C++** designed to emulate communication between network nodes using simplified TCP and UDP protocols.

It focuses on reproducing realistic network conditions such as packet loss, delay, and corruption while providing tools to analyze protocol behavior, performance, and reliability.

The simulator is fully modular and built for educational purposes (1–2 students), with a clean separation between core networking components, protocol logic, simulation engine, and tooling.

---

## Features

* Simulation of multiple network nodes communicating over a shared channel
* Simplified **TCP-like reliable communication**
* **UDP datagram-based communication**
* Network impairments:

  * Packet loss
  * Transmission delay
  * Packet corruption
* Event-driven simulation engine
* Logging and packet inspection tools (Wireshark-like)
* Performance metrics:

  * Throughput
  * Latency
  * Packet loss rate

---

## Project Architecture

The project is organized into four main layers:

### 1. Core (`core/`)

Responsible for the fundamental networking elements.

* `Node`: Represents a network endpoint (sender/receiver)
* `Channel`: Simulates the communication medium
* `Packet`: Represents data exchanged between nodes

Core responsibilities:

* Packet transmission
* Node communication
* Channel behavior (loss, delay, corruption)

---

### 2. Protocols (`protocols/`)

Implements transport-layer behavior.

* `TCP`:

  * Connection establishment (handshake simulation)
  * Reliable delivery
  * Acknowledgements (ACK)
  * Retransmissions

* `UDP`:

  * Connectionless communication
  * Best-effort delivery
  * No guarantees on order or delivery

---

### 3. Simulation Engine (`simulation/`)

Handles event-driven execution of the network.

* `Simulator`:

  * Runs the simulation loop
  * Processes scheduled events
  * Coordinates nodes and channels

* `Event`:

  * Represents discrete simulation actions (send, receive, drop, etc.)

This layer enables deterministic and repeatable simulations.

---

### 4. Tools (`tools/`)

Provides debugging, monitoring, and analysis utilities.

* `Logger`:

  * Logs network events (transmissions, drops, ACKs)

* `Metrics`:

  * Computes throughput, latency, packet loss

* `Wireshark`:

  * Packet tracing and inspection tool
  * Helps visualize simulated traffic

---

## Network Model

The simulator introduces controlled network imperfections:

* **Packet Loss**: probabilistic dropping of packets in `Channel`
* **Delay**: simulated transmission latency
* **Corruption**: random modification of packet data

These parameters can be tuned to simulate different network conditions.

---

## Packet Structure

Each `Packet` typically contains:

* Source ID
* Destination ID
* Payload
* Sequence number (TCP)
* Acknowledgement flag (TCP)
* Type (DATA / ACK / CONTROL)

---

## Simulation Flow

1. Nodes generate events (send packet)
2. Events are pushed to the `Simulator`
3. `Channel` processes transmission with impairments
4. Receiver node processes packet
5. TCP/UDP protocol layer handles delivery logic
6. Tools log and analyze the result

---

## Build System

### Requirements

* C++17 or later
* `make`
* `g++` or `clang++`

### Compilation

```bash
make
```

### Run

```bash
./network_simulator
```

---

## Project Structure

```
.
├── LICENSE
├── Makefile
├── README.md
├── core
│   ├── Channel.cpp
│   ├── Channel.hpp
│   ├── Node.cpp
│   ├── Node.hpp
│   ├── Packet.cpp
│   └── Packet.hpp
├── main.cpp
├── protocols
│   ├── TCP.cpp
│   ├── TCP.hpp
│   ├── UDP.cpp
│   └── UDP.hpp
├── simulation
│   ├── Event.cpp
│   ├── Event.hpp
│   ├── Simulator.cpp
│   └── Simulator.hpp
└── tools
    ├── Logger.cpp
    ├── Logger.hpp
    ├── Metrics.cpp
    ├── Metrics.hpp
    ├── Wireshark.cpp
    └── Wireshark.hpp
```

---

## Design Assumptions

* No real network sockets are used (fully simulated environment)
* TCP is simplified (no congestion control, no sliding window)
* Event-driven simulation instead of real-time OS scheduling
* Fixed or simplified packet sizing
* Direct node-to-node communication (no routing layer)

---

## Limitations

* No IP layer or routing protocols
* No real parallelism (single-threaded simulation)
* Simplified TCP compared to real-world implementations

---

## Possible Improvements

* Add congestion control (TCP Reno / Tahoe behavior)
* Introduce routing (Dijkstra, distance vector)
* Add GUI visualization of network events
* Multi-threaded simulation engine
* More accurate timing model (discrete event priority queue optimization)

---

## Authors

Project of Titouan SIMON for learning network protocols and systems programming in C++.