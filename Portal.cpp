#include "Portal.hpp"
#include <stdio.h>
#include "tusb.h"

bool Portal::canSend;
bool Portal::active;
std::queue<Command> Portal::outgoing;
unsigned char Portal::counter;

void Portal::Initialize()
{
    outgoing = std::queue<Command>();
    active = false;
    canSend = true;
    counter = 0;
}

void Portal::HandleCommand(const unsigned char* buffer)
{
    switch(buffer[0])
    {
        case 'R': {
            Command command = {
                .data = {'R', 0x02, 0x18}
            };
            outgoing.push(command);
            break;
        }
        case 'A': {
            (buffer[1] == 0x01)? Portal::Enable() : Portal::Disable();
            Command command = {
                .data = {'A', buffer[1]}
            };
            outgoing.push(command);
            break;
        }
        case 'S': {
            Command command = {};

            GetStatus(command.data);

            outgoing.push(command);
            break;
        }
    }
}

void Portal::Poll()
{
    if(canSend)
    {
        if(!outgoing.empty())
        {
            unsigned char command[32] = {};
            memcpy(command, outgoing.front().data, 32);

            outgoing.pop();

            tud_hid_report(0, command, 32);
        }
        else
        {
            unsigned char command[32] = {};
            
            GetStatus(command);

            tud_hid_report(0, command, 32);
        }
    }

}

void Portal::Enable()
{
    active = true;
}

void Portal::Disable()
{
    active = false;
}

void Portal::GetStatus(unsigned char* target)
{
    unsigned char status[32] = {'S'};

    // TODO: figures

    status[5] = counter++;

    status[6] = (active)? 0x01 : 0x00;

    memcpy(target, status, 32);
}