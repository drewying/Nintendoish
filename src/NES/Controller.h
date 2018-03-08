#pragma once

#include "NES.h"

#include <stdint.h>

namespace NES {
    class Controller {
    public:
        Controller();
        ~Controller();

        uint8_t pollController();
        void startPoll();

        bool a      = false;
        bool b      = false;
        bool select = false;
        bool start  = false;
        bool up     = false;
        bool down   = false;
        bool left   = false;
        bool right  = false;

        bool strobeLatch = false;
        int currentPoll = 0;
    };
};


