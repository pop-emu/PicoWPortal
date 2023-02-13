#pragma once
#include <queue>

struct Command {
    unsigned char data[32];
};

class Portal {
public:
    static void Initialize();
    static void HandleCommand(const unsigned char* buffer);
    static void Poll();
    static void Enable();
    static void Disable();
    static bool canSend;
private:
    static bool active;
    static std::queue<Command> outgoing;
};