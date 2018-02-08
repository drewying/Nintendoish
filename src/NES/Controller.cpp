#include "Controller.h"


unsigned char NES::Controller::pollController()
{

	
	if (strobeLatch == false) return 0x40 & a;
	
	switch (currentPoll++) {
	case 0:
		return 0x40 | a;
		break;
	case 1:
		return 0x40 | b;
		break;
	case 2:
		return 0x40 | select;
		break;
	case 3:
		return 0x40 | start;
		break;
	case 4:
		return 0x40 | up;
		break;
	case 5:
		return 0x40 | down;
		break;
	case 6:
		return 0x40 | left;
		break;
	case 7:
		return 0x40 | right;
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
