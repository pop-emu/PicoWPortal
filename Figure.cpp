#include "Figure.hpp"
#include <string.h>

Figure::Figure()
{
    status = NOT_PRESENT;
    memset(data, 0, 1024);
}

char* Figure::Query(char block)
{
    unsigned short offset = block * 16;
    return &data[offset];
}

FigureStatus Figure::GetStatus()
{
    return status;
}