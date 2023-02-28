#include "Figure.hpp"

Figure::Figure()
{
    status = NOT_PRESENT;
}

unsigned char Figure::GetByte(char block, char offset)
{
    unsigned char pos = (block * 16) + offset;
    return data[pos];
}

unsigned short Figure::GetShort(char block, char offset)
{
    unsigned short result = (GetByte(block, offset + 1) << 8) + GetByte(block, offset);

    return result;
}