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

using namespace std::chrono;
using namespace std;

static Display *display;
static Chip8 *chip8;
static NES::Console *nes;

ifstream logfile;
bool passedTest = true;
bool runTests = false;
bool fullLog = false;
int debugStartLineNumber = 9000;
int lineNumber = 0x0;

//const int frequency = 540; //540 hertz
const int frequency = 10;

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

void keyDown(unsigned char key, int x, int y) {
	chip8->handleKeyPress(key, true);
}

void keyUp(unsigned char key, int x, int y) {
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
}

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
}


void updateNES(void) {
	//high_resolution_clock::time_point t1 = high_resolution_clock::now();
    nes->emulateCycle();
    //string emuLine = nes->cpu->lastInstruction;//cout << emuLine << endl;
    //if (cpu->drawFlag) glutPostRedisplay();
    //high_resolution_clock::time_point t2 = high_resolution_clock::now();
    //duration<double, std::micro> time_span = t2 - t1;
    //int microsToSleep = int(1000000 / frequency) - time_span.count();
    //std::this_thread::sleep_for(std::chrono::microseconds(microsToSleep));
}

void testNES(void)
{
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	
	string SL = std::to_string(nes->ppu->currentScanline);
	string CYC = std::to_string(nes->ppu->currentCycle);

	std::stringstream ss;
	ss << " CYC:" << setfill(' ') << setw(3) << nes->ppu->currentCycle << " SL:" << nes->ppu->currentScanline;

    nes->emulateCycle();
	if (nes->stallCycles == 0x0) {
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
    
		string emuLine = nes->cpu->lastInstruction + ss.str();
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


		if (logPc == emuPc && logOp == emuOp && logLo == emuLo && logHi == emuHi && logDesc == emuDesc && logA == emuA && logX == emuX && logY == emuY && logP == emuP && logSP == emuSP && logCYC == emuCYC && logSL == emuSL) {
			if (lineNumber > debugStartLineNumber || fullLog == true) {
				cout << lineNumber << " " << logLine << endl;
				cout << lineNumber << " " << emuLine << endl;
				cout << "CORRECT. FRAME:" << nes->ppu->frameCount << endl << endl;
			}
		}
		else if (lineNumber > debugStartLineNumber || fullLog == true) {
			cout << lineNumber << " " << logLine << endl;
		    cout << lineNumber << " " << emuLine << endl;
			cout << "NOT CORRECT. FRAME:" << nes->ppu->frameCount << endl << endl;
 			passedTest = false;
		}
	}
	else {
		string emuLine = nes->cpu->lastInstruction + ss.str();
		cout << "STALL CYCLE " << emuLine << endl << endl;
	}

 
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
	//chip8 = new Chip8();
	//chip8->loadProgram("../roms/BRIX");
    if (runTests) {
        nes = new NES::Console();
        nes->loadProgram("../roms/NESTEST.nes");
        logfile = ifstream("../roms/nestest.log");
        glutIdleFunc(testNES);
    } else {
        nes = new NES::Console();
        nes->loadProgram("../roms/DonkeyKong.nes");
		logfile = ifstream("../roms/DonkeyKong.log");
		glutIdleFunc(testNES);
        //glutIdleFunc(updateNES);
    }
    
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);
	glutDisplayFunc(updateDisplay);
	glutMainLoop();

	return 0;
}
