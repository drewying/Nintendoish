#include <GL/GLUT.h>
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
static Chip8 *chip8;
static NES::Console *nes;

ifstream logfile;
bool pause = false;
bool passedTest = false;
bool runTests = false;
bool fullLog = false;
int debugStartLineNumber = 0000;
int lineNumber = 0x0;

//const int frequency = 540; //540 hertz
const int frequency = 10;

void updateDisplay(void)
{
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
    glutSwapBuffers();
    nes->updateGraphics = false;
}

void specialKeyDown(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        nes->controllerOne->up = true;
        break;
    case GLUT_KEY_DOWN:
        nes->controllerOne->down = true;
        break;
    case GLUT_KEY_LEFT:
        nes->controllerOne->left = true;
        break;
    case GLUT_KEY_RIGHT:
        nes->controllerOne->right = true;
        break;
    default:
        break;
    }
}

void specialKeyUp(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_UP:
        nes->controllerOne->up = false;
        break;
    case GLUT_KEY_DOWN:
        nes->controllerOne->down = false;
        break;
    case GLUT_KEY_LEFT:
        nes->controllerOne->left = false;
        break;
    case GLUT_KEY_RIGHT:
        nes->controllerOne->right = false;
        break;
    default:
        break;
    }
}


void keyDown(uint8_t key, int x, int y) {
    switch (key) {
    case 'a':
        nes->controllerOne->b = true;
        break;
    case 's':
        nes->controllerOne->a = true;
        break;
    case '\\':
        nes->controllerOne->select = true;
        break;
    case '\r':
        nes->controllerOne->start = true;
        break;
    case 'p':
        pause = !pause;
        break;
    default:
        break;
    }
}


void keyUp(uint8_t key, int x, int y) {
    switch (key) {
    case 'a':
        nes->controllerOne->b = false;
        break;
    case 's':
        nes->controllerOne->a = false;
        break;
    case '\\':
        nes->controllerOne->select = false;
        break;
    case '\r':
        nes->controllerOne->start = false;
        break;
    default:
        break;
    }
}



void updateNES(void) {
    //high_resolution_clock::time_point t1 = high_resolution_clock::now();
    if (pause) return;
    nes->emulateCycle();
    if (nes->updateGraphics) glutPostRedisplay();
    //high_resolution_clock::time_point t2 = high_resolution_clock::now();
    //duration<double, std::micro> time_span = t2 - t1;
    //int microsToSleep = int(1000000 / frequency) - time_span.count();
    //std::this_thread::sleep_for(std::chrono::microseconds(microsToSleep));
}

void testNES(void)
{
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    
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

    nes->emulateCycle();

    //if (cpu->drawFlag) glutPostRedisplay();
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double, std::micro> time_span = t2 - t1;
    //int microsToSleep = int(1000000 / frequency) - time_span.count();
    //std::this_thread::sleep_for(std::chrono::microseconds(microsToSleep));
}

int main(int argc, char** argv)
{
    display = new Display(4, 256, 240);
    display->initialize(argc, argv);
    if (runTests) {
        nes = new NES::Console();
        nes->loadProgram("../roms/Zelda.nes");
        logfile = ifstream("../roms/DonkeyKong.log");
        glutIdleFunc(testNES);
    } else {
        nes = new NES::Console();
        //nes->loadProgram("../roms/IceClimber.nes");
        //nes->loadProgram("../roms/Megaman.nes");
        //nes->loadProgram("../roms/Castlevania.nes");
        nes->loadProgram("../roms/Zelda.nes");
        //nes->loadProgram("../roms/Mario.nes");
        //nes->loadProgram("../roms/Excitebike.nes");
        //nes->loadProgram("../roms/DonkeyKong.nes");
        //nes->loadProgram("../roms/palette.nes");
        glutIdleFunc(updateNES);
    }

    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialKeyDown);
    glutSpecialUpFunc(specialKeyUp);
    glutDisplayFunc(updateDisplay);
    glutMainLoop();

    return 0;
}

/*
void updateCHIP8Display(void)
{
glClear(GL_COLOR_BUFFER_BIT);
for (int x = 0; x < 64; x++) {
for (int y = 0; y < 32; y++) {
if (chip8->graphics[x + (y * 64)]) {
display->plotPixel(x, y, 1.0, 1.0, 1.0);
}
}
}

glEnd();
glutSwapBuffers();
}

void CHIP8KeyDown(uint8_t key, int x, int y) {
chip8->handleKeyPress(key, true);
}

void CHIP8KeyUp(uint8_t key, int x, int y) {
chip8->handleKeyPress(key, false);
}

void updateCHIP8(void)
{
high_resolution_clock::time_point t1 = high_resolution_clock::now();
chip8->emulateCycle();
if (chip8->drawFlag) glutPostRedisplay();
high_resolution_clock::time_point t2 = high_resolution_clock::now();
duration<double, std::micro> time_span = t2 - t1;
int microsToSleep = int(1000000 / frequency) - time_span.count();
std::this_thread::sleep_for(std::chrono::microseconds(microsToSleep));
}*/