enum Status {
    NOT_PRESENT = 0b00,
    PRESENT = 0b01,
    ADDED = 0b11,
    REMOVED = 0b10
};

class Figure {
public:
    Figure();
    unsigned char GetByte(char block, char offset);
    unsigned short GetShort(char block, char offset);
    Status status;
    unsigned char data[1024];
    
};