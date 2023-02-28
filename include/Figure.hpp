enum Status {
    NOT_PRESENT = 0x00,
    PRESENT = 0x01,
    ADDED = 0x11,
    REMOVED = 0x10
};

class Figure {
public:
    Figure();
    unsigned char GetByte(char block, char offset);
    unsigned short GetShort(char block, char offset);
    Status status;
    unsigned char data[1024];
    
};