enum Status {
    NOT_PRESENT = 0x00,
    PRESENT = 0x01,
    ADDED = 0x11,
    REMOVED = 0x10
};

class Figure {
public:
    Figure();
    unsigned char data[1024];
    Status status;
};