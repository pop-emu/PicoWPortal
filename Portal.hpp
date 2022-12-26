#include <queue>

typedef struct Command 
{
    char command;
    union{
        bool activate;
        struct {
            char index;
            char block;
            char data[16];
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
    std::queue<Command> commands = std::queue<Command>();
    bool active = false;
    unsigned char counter = 0;
};