#include "glfw3.h"
#include "Display.h"
#include "CHIP8/Chip8.h"
#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "NES/NES.h"
#include "NES/PPU.h"

using namespace std::chrono;
using namespace std;

static Display *display;
static NES::Console *nes;

ifstream logfile;
bool pause = false;
bool passedTest = false;
bool runTests = false;
bool fullLog = false;
int debugStartLineNumber = 0000;
int lineNumber = 0x0;

const double cyclesPerSecond = 1789773;
const double targetFps = 60.0988;

void updateDisplay(void) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    for (int x = 0; x < 256; x++) {
        for (int y = 0; y < 240; y++) {
            unsigned int color = nes->graphics[x + (y * 256)];
            if (color) {
                float blue = (float)(color & 0x000000FF);
                float green = (float)((color & 0x0000FF00) >> 8);
                float red = (float)((color & 0x00FF0000) >> 16);
                display->plotPixel(x, y, red / 255.0, green / 255.0, blue / 255.0);
            }
        }
    }

    glEnd();
    glfwSwapBuffers(display->window);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    bool isKeyDown = action == GLFW_PRESS;
    bool isKeyUp = action == GLFW_RELEASE;
    if (!isKeyUp && !isKeyDown) return;
    switch (key) {
        case GLFW_KEY_A:
            nes->controllerOne->b = isKeyDown;
            break;
        case GLFW_KEY_S:
            nes->controllerOne->a = isKeyDown;
            break;
        case GLFW_KEY_UP:
            nes->controllerOne->up = isKeyDown;
            break;
        case GLFW_KEY_DOWN:
            nes->controllerOne->down = isKeyDown;
            break;
        case GLFW_KEY_LEFT:
            nes->controllerOne->left = isKeyDown;
            break;
        case GLFW_KEY_RIGHT:
            nes->controllerOne->right = isKeyDown;
            break;
        case GLFW_KEY_RIGHT_SHIFT:
            nes->controllerOne->select = isKeyDown;
            break;
        case GLFW_KEY_ENTER:
            nes->controllerOne->start = isKeyDown;
            break;
        case GLFW_KEY_P:
            if (isKeyDown) pause = !pause;
            break;
        default:
            break;
    }
}


void updateNES() {
    //Update one frame
    int currentFrame = nes->ppu->totalFrames;
    while (nes->ppu->totalFrames == currentFrame) nes->emulateCycle();
}

void testNES(void) {
    if (nes->cpu->stallCycles == 0x0) {

        std::stringstream ss;

        uint8_t nextInstruction = nes->memory->get(nes->cpu->reg.PC);

        ss << uppercase << setfill('0') << setw(4) << hex << nes->cpu->reg.PC << "  " << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nextInstruction;
        /*if (reg.PC - startingPC > 1) {
        debugBuffer << " " << uppercase << setfill('0') << setw(2) << hex << (uint16_t)lo;
        }
        else {
        debugBuffer << "   ";
        }
        if (reg.PC - startingPC > 2) {
        debugBuffer << " " << uppercase << setfill('0') << setw(2) << hex << (uint16_t)hi;
        }
        else {
        debugBuffer << "   ";
        }*/
        ss << "      ";
        ss << "  " << nes->cpu->debugTable[nextInstruction];
        ss << "                             A:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.A;
        ss << " X:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.X;
        ss << " Y:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.Y;
        ss << " P:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.P.byte;
        ss << " SP:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.S;
        ss << " CYC:" << setfill(' ') << dec << setw(3) << nes->ppu->currentCycle << " SL:" << nes->ppu->currentScanline;

        lineNumber++;
        string logLine;
        if (!getline(logfile, logLine)){
            if (passedTest){
                cout << "Tests Passed" << endl;
            } else {
                cout << "Tests Failed" << endl;
            }
            exit(0);
        }

        string logPc = logLine.substr(0, 4);
        string logOp = logLine.substr(6, 2);
        string logLo = logLine.substr(9, 2);
        string logHi = logLine.substr(12, 2);
        string logDesc = logLine.substr(16, 3);
        string logA = logLine.substr(50, 2);
        string logX = logLine.substr(55, 2);
        string logY = logLine.substr(60, 2);
        string logP = logLine.substr(65, 2);
        string logSP = logLine.substr(71, 2);
        string logCYC = logLine.substr(78, 3);
        string logSL = logLine.substr(85, 3);
    
        string emuLine = ss.str();
        string emuPc = emuLine.substr(0, 4);
        string emuOp = emuLine.substr(6, 2);
        string emuLo = emuLine.substr(9, 2);
        string emuHi = emuLine.substr(12, 2);
        string emuDesc = emuLine.substr(16, 3);
        string emuA = emuLine.substr(50, 2);
        string emuX = emuLine.substr(55, 2);
        string emuY = emuLine.substr(60, 2);
        string emuP = emuLine.substr(65, 2);
        string emuSP = emuLine.substr(71, 2);
        string emuCYC = emuLine.substr(78, 3);
        string emuSL = emuLine.substr(85, 3);


        if (logPc == emuPc && logOp == emuOp && /*logLo == emuLo && logHi == emuHi &&*/ logDesc == emuDesc && logA == emuA && logX == emuX && logY == emuY && logP == emuP && logSP == emuSP && logCYC == emuCYC && logSL == emuSL) {
            if (lineNumber > debugStartLineNumber || fullLog == true) {
                //cout << lineNumber << " " << logLine << endl;
                cout << lineNumber << " " << emuLine << endl;
                //cout << "CORRECT. FRAME:" << nes->ppu->frameCount << endl << endl;
            }
        }
        else if (lineNumber > debugStartLineNumber || fullLog == true) {
            //cout << lineNumber << " " << logLine << endl;
            cout << lineNumber << " "   << emuLine << endl;
            //cout << "NOT CORRECT. FRAME:" << nes->ppu->frameCount << endl << endl;
            passedTest = false;
        }
    }

    nes->emulateCycle();;
}

void gameLoop() {
    
    double targetFPS = 60.0988; //NTSC Vertical Scan Rate
    double targetFrameLength = 1000.0/targetFPS;
    double startTime = glfwGetTime() * 1000;
    
    int frameCount = 0;
    int droppedFrames = 0;
    int sleptFrames = 0;
    
    while (!glfwWindowShouldClose(display->window)) {
        bool skipFrame = false;
        bool sleepFrame = false;
        
        long realTime = (glfwGetTime() * 1000) - startTime; //How much time we've taken
        long gameTime = (frameCount * targetFrameLength);   //How much time in the game
        
        if (gameTime - realTime > targetFrameLength) sleepFrame = true; //We are behind. Sleep a frame.
        if (realTime - gameTime > targetFrameLength) skipFrame = true; //We are ahead. Skip rendeirng.
        
        if (!pause) {
            if (sleepFrame) {
                printf("Sleeping\n");
                std::this_thread::sleep_for(std::chrono::milliseconds(realTime - gameTime));
                sleptFrames++;
            }
            updateNES();
            
            if (!skipFrame){
                updateDisplay();
            } else {
                printf("Skipping frame\n");
                droppedFrames++;
            }
            frameCount++;
        } else {
            startTime = glfwGetTime() * 1000;
            frameCount = 0;
        }
        glfwPollEvents();
        
    }
    glfwTerminate();
}

int main(int argc, char** argv) {
    display = new Display(4, 256, 240);
    display->initialize(argc, argv);
    //glfwSetInputMode(display->window, GLFW_STICKY_KEYS, 1);
    glfwSetKeyCallback(display->window, keyCallback);
    nes = new NES::Console();
    //nes->loadProgram("../roms/PunchOut.nes");
    //nes->loadProgram("../roms/test/scanline.nes");
    //nes->loadProgram("../roms/Battletoads.nes");
    //nes->loadProgram("../roms/Gradius.nes");
    //nes->loadProgram("../roms/Contra.nes");
    //nes->loadProgram("../roms/Metroid.nes");
    //nes->loadProgram("../roms/IceClimber.nes");
    nes->loadProgram("../roms/Megaman3.nes");
    //nes->loadProgram("../roms/Castlevania.nes");
    //nes->loadProgram("../roms/Zelda.nes");
    //nes->loadProgram("../roms/Mario.nes");
    //nes->loadProgram("../roms/Excitebike.nes");
    //nes->loadProgram("../roms/DonkeyKong.nes");
    //nes->loadProgram("../roms/sprite_ram.nes");

    
    gameLoop();
    return 0;
}
