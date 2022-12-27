#include "Portal.hpp"
#include "tusb.h"

Portal::Portal()
{
    for(char i = 0; i < 16; i++)
    {
        figures[i] = Figure();
    }
}

Portal* Portal::GetPortal()
{
    static Portal* portal = nullptr;

    if(portal == nullptr)
    {
        portal = new Portal();
    }

    return portal;
}

void Portal::HandleCommand(const unsigned char* buf, unsigned short size)
{
    switch(buf[0])
    {
        case 'R':
        {
            commands.push({.command = 'R'});

            break;
        }
        case 'A':
        {
            Command command = {
                .command = 'A',
                .activate = buf[1]};
            commands.push(command);

            if(buf[1] == 0x01)
                this->Activate();
            else
                this->Deactivate();

            break;
        }
        case 'S':
        {
            commands.push({.command = 'S'});

            break;
        }
        case 'Q':
        {

            Command command = {
                .command = 'Q',
                .query = {
                    .index = (char)((buf[1] % 0x10) + 0x10),
                    .block = buf[2]
                }
                };

            memcpy(&command.query.data, figures[command.query.index].Query(command.query.block), 16);
            commands.push(command);
        }
        case 'J':
        {
            commands.push({.command = 'J'});
            break;
        }
    }
}

void Portal::Activate()
{
    active = true;
}

void Portal::Deactivate()
{
    active = false;
}

void Portal::SendCommand()
{
    if(!commands.empty())
    {
        Command command = commands.front();
        commands.pop();

        unsigned char out[32] = "";

        out[0] = command.command;

        switch(command.command)
        {
            case 'R':
            {
                out[1] = 0x02;
                out[2] = 0x18;

                break;
            }
            case 'A':
            {
                out[1] = command.activate;
                out[2] = 0xFF;
                out[3] = 0x77;

                break;
            }
            case 'S':
            {
                for(unsigned char i = 1; i <= 4; i++ )
                {
                    for(unsigned char j = 0; j < 4; j++)
                    {
                        unsigned char index = ((i - 1) * 4) + j;

                        out[i] = figures[index].GetStatus() << 2 * j;

                        figures[index].UpdateStatus();
                    }
                }
                out[5] = counter++;
                out[6] = active ? 0x01 : 0x00;

                break;
            }
            case 'Q':
            {
                
                out[1] = command.query.index;
                out[2] = command.query.block;
                memcpy(out + 3, command.query.data, 16);

                break;
            }
            case 'J':
            {
                out[0] = 'J';
                break;
            }
        }

        tud_hid_report(0, out, 32);
        readyToSend = false;
    }
    else
    {
        if(active)
        {
            unsigned char out[32] = {'S'};
            for(char i = 1; i <= 4; i++ )
            {
                for(char j = 0; j < 4; j++)
                {
                    char index = ((i - 1) * 4) + j;

                    out[i] = figures[index].GetStatus() << 2 * j;
                    figures[index].UpdateStatus();
                }
            }
            out[5] = counter++;
            out[6] = active ? 0x01 : 0x00;
            tud_hid_report(0, out, 32);
            readyToSend = false;
        }
    }
}

void Portal::PortalTask()
{
    if(readyToSend)
    {
        SendCommand();
    }
}