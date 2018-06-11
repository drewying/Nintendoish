#include "Controller.h"


uint8_t NES::Controller::pollController()
{
    if (strobeLatch == false) return 0x40 | (uint8_t)a;
    
    switch (currentPoll++) {
    case 0:
        return 0x40 | (uint8_t)a;
        break;
    case 1:
        return 0x40 | (uint8_t)b;
        break;
    case 2:
        return 0x40 | (uint8_t)select;
        break;
    case 3:
        return 0x40 | (uint8_t)start;
        break;
    case 4:
        return 0x40 | (uint8_t)up;
        break;
    case 5:
        return 0x40 | (uint8_t)down;
        break;
    case 6:
        return 0x40 | (uint8_t)left;
        break;
    case 7:
        return 0x40 | (uint8_t)right;
        break;
    default:
        return 0x41;
    }
}

void NES::Controller::startPoll()
{
    strobeLatch = true;
    currentPoll = 0;
}

NES::Controller::Controller()
{
}


NES::Controller::~Controller()
{
}
