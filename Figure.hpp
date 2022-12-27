#pragma once

enum FigureStatus {
    NOT_PRESENT = 0b00,
    PRESENT = 0b01,
    REMOVED = 0b10,
    ADDED = 0b11,
};

class Figure {

public:
    Figure();
    char* Query(char block);
    FigureStatus GetStatus();
    void UpdateStatus()
    {
        switch(status)
        {
            case ADDED: status = PRESENT; break;
            case REMOVED: status = NOT_PRESENT; break;
            default: break;
        }
    }

private:
    char data[1024];
    FigureStatus status;
};