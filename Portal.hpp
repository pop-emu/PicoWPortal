#pragma once
#include <queue>
#include "Figure.hpp"

typedef struct Command 
{
    char command;
    union{
        bool activate;
        struct {
            unsigned char index;
            unsigned char block;
            unsigned char data[16];
        } query;
    };
} Command;

class Portal
{
    public:
    static Portal* GetPortal();
    void HandleCommand(const unsigned char* buf, unsigned short size);
    void SendCommand();
    void PortalTask();
    void Activate();
    void Deactivate();
    bool readyToSend = true;

    private:
    Portal();
    Figure figures[16];
    std::queue<Command> commands = std::queue<Command>();
    bool active = false;
    unsigned char counter = 0;
};