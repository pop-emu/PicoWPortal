#include "Portal.hpp"
#include <stdio.h>
#include "tusb.h"

bool Portal::canSend;
bool Portal::active;
std::queue<Command> Portal::outgoing;
unsigned char Portal::counter;
Color Portal::colorLeft;
Color Portal::colorRight;
Color Portal::colorTrap;
Figure Portal::figures[16];

void Portal::Initialize()
{
    outgoing = std::queue<Command>();
    active = false;
    canSend = true;
    counter = 0;

    colorLeft = {
        .red = 0x00,
        .green = 0x00,
        .blue = 0x00,
    };

    colorRight = {
        .red = 0x00,
        .green = 0x00,
        .blue = 0x00,
    };

    colorTrap = {
        .red = 0x00,
        .green = 0x00,
        .blue = 0x00,
    };
}

void Portal::HandleCommand(const unsigned char *buf)
{
    unsigned char buffer[32];

    memcpy(buffer, buf, 32);

    switch (buffer[0])
    {
    case 'R':
    {
        Command command = {
            .data = {'R', 0x02, 0x18}
        };

        Portal::Disable();

        outgoing.push(command);
        break;
    }
    case 'A':
    {
        (buffer[1] == 0x01) ? Portal::Enable() : Portal::Disable();
        Command command = {
            .data = {'A', buffer[1]}};
        outgoing.push(command);
        break;
    }
    case 'S':
    {
        Command command = {};

        GetStatus(command.data);

        outgoing.push(command);
        break;
    }
    case 'C':
    {
        Command command = {};

        Color color = {
            .red = buffer[1],
            .green = buffer[2],
            .blue = buffer[3],
        };

        colorLeft = color;
        colorRight = color;

        memcpy(command.data, buffer, 4);

        outgoing.push(command);
        break;
    }
    case 'J':
    {

        Color color = {
            .red = buffer[2],
            .green = buffer[3],
            .blue = buffer[4],
        };

        switch (buffer[1])
        {
        case 0x00:
        {
            colorRight = color;
            break;
        }
        case 0x01:
        {
            colorLeft = color;
            colorRight = color;
            break;
        }
        case 0x02:
        {
            colorLeft = color;
            break;
        }
        }

        break;
    }
    case 'L':
    {

        Color color = {
            .red = buffer[2],
            .green = buffer[3],
            .blue = buffer[4],
        };

        switch (buffer[1])
        {
        case 0x00:
        {
            colorRight = color;
            break;
        }
        case 0x01:
        {
            colorTrap = color;
            break;
        }
        case 0x02:
        {
            colorLeft = color;
            break;
        }
        }

        break;
    }
    case 'Q':
    {
        unsigned char index = (buffer[1] % 0x10);

        unsigned char block = buffer[2];

        Command command = {
            .data = {'Q', (char)(index + 16), buffer[2]}
        };

        Status status = figures[index].status;

        if(status == PRESENT || status == ADDED)
        {
            memcpy(&command.data[3], &figures[index].data[block * 16], 16);
        }
        else
        {
            command.data[1] = 0x01;
        }

        outgoing.push(command);

        break;
    }
    case 'W':
    {
        unsigned char index = (buffer[1] % 0x10);

        unsigned char block = buffer[2];

        Command command = {
            .data = {}
        };

        memcpy(&figures[index].data[block * 16], &buffer[3], 16);

        memcpy(command.data, buffer, 32);

        outgoing.push(command);

        break;
    }
	case 'M':
	{
		Command command = {
            .data = {'M', 0x00}
        };

        outgoing.push(command);
	}
    }
}

void Portal::Poll()
{
    if (canSend)
    {
        if (!outgoing.empty())
        {
            unsigned char command[32] = {};

            memcpy(command, outgoing.front().data, 32);

            outgoing.pop();

            tud_hid_report(0, command, 32);
        }
        // TODO: find a way to make this not break everything
        // else if (active)
        // {
        //     unsigned char command[32] = {};

        //     GetStatus(command);

        //     tud_hid_report(0, command, 32);
        // }
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

void Portal::GetStatus(unsigned char *target)
{
    unsigned char status[32] = {'S'};

    for (unsigned char i = 0; i < 4; i++)
    {
        unsigned char figureStatus = 0x00;

        for (unsigned char j = 0; j < 4; j++)
        {
            unsigned char index = (4 * i) + j;

            figureStatus |= (figures[index].status << (2 * j));

            if(figures[index].status == REMOVED)
            {
                figures[index].status = NOT_PRESENT;
            }
            else if(figures[index].status == ADDED)
            {
                figures[index].status = PRESENT;
            }
        }

        status[1 + i] = figureStatus;
    }

    status[5] = counter++;

    status[6] = (active) ? 0x01 : 0x00;

    memcpy(target, status, 32);
}

void Portal::RemoveFigure(char index)
{
    figures[index].status = REMOVED;

    memset(figures[index].data, 0, 1024);

    unsigned char command[32] = {};

    GetStatus(command);

    tud_hid_report(0, command, 32);
}

void Portal::AddFigure(char index, const char* data)
{
    figures[index].status = ADDED;

    memcpy(figures[index].data, data, 1024);

    unsigned char command[32] = {};

    GetStatus(command);

    tud_hid_report(0, command, 32);
}