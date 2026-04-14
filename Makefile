# ====== CONFIG ======
CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17

TARGET = main

# ====== SOURCES ======
SRC = \
    main.cpp \
    core/Node.cpp \
    core/Packet.cpp \
    core/Channel.cpp \
    protocols/TCP.cpp \
    protocols/UDP.cpp \
    simulation/Simulator.cpp \
    simulation/Event.cpp \
    tools/Logger.cpp \
	tools/Wireshark.cpp \
    tools/Metrics.cpp

# ====== OBJECTS ======
OBJ = $(SRC:.cpp=.o)

# ====== RULES ======

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

# Compilation générique
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ====== CLEAN ======

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(TARGET)

re: fclean all

# ====== PHONY ======
.PHONY: all clean fclean re