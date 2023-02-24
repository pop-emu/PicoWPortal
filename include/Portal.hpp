#pragma once

#include <queue>
#include "Figure.hpp"

struct Command {
    unsigned char data[32];
};

struct Color {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

class Portal {
public:
    static void Initialize();
    static void HandleCommand(const unsigned char* buffer);
    static void Poll();
    static void Enable();
    static void Disable();
    static void GetStatus(unsigned char* target);
    static bool canSend;
    static bool active;
    static std::queue<Command> outgoing;
    static unsigned char counter;
    static Color colorLeft;
    static Color colorRight;
    static Color colorTrap;
    static Figure figures[16];
};