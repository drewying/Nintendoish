#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm> 

#include "glfw3.h"
#include "portaudio.h"
#include "Display.h"
#include "../Core/NES.h"
#include "../Core/PPU.h"


using namespace std::chrono;
using namespace std;

static Display *display;
static NES::Console *nes;

bool pause = false;
bool showLog = false;
bool timeSync = false;
int debugStartLineNumber = 2000;
int lineNumber = 0x0;

const double audioSamplesPerSecond = 44100;
const double targetFPS = 60.0988; //NTSC Vertical Scan Rate

PaStream *audioStream;
PaError audioError;

void updateAudio() {
    Pa_WriteStream(audioStream, nes->audioBuffer, nes->audioBufferLength);
    nes->audioBufferLength = 0;
}

void updateDisplay() {
    display->drawBuffer(nes->displayBuffer);
}

void updateNES() {
    //Update one frame
    int currentFrame = nes->ppu->totalFrames;
    while (nes->ppu->totalFrames == currentFrame) {
        nes->emulateCycle();
    }
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

void logLoop() {
    while (!glfwWindowShouldClose(display->window)) {
        if (!pause) {
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
                
                string emuLine = ss.str();
                if (lineNumber >= debugStartLineNumber) {
                    cout << lineNumber << " " << emuLine << endl;
                }
                
            }
            
            int currentFrame = nes->ppu->totalFrames;
            nes->emulateCycle();
            if (nes->ppu->totalFrames != currentFrame) updateDisplay();
        }
        glfwPollEvents();
    }
    glfwTerminate();
}

void gameLoop() {
    Pa_StartStream(audioStream);
    double targetFrameLength = 1000.0/targetFPS;
    double startTime = glfwGetTime() * 1000;
    
    int frameCount = 0;
    int droppedFrames = 0;
    int sleptFrames = 0;
    
    while (!glfwWindowShouldClose(display->window)) {
        bool dropFrame = false;
        bool sleepFrame = false;
        
        long realTime = (glfwGetTime() * 1000) - startTime; //How much time we've taken in the real world
        long gameTime = (frameCount * targetFrameLength);   //How much time in the game has passed
        
        if (gameTime - realTime > targetFrameLength) sleepFrame = true; //We are behind. Sleep a frame.
        if (realTime - gameTime > targetFrameLength) dropFrame = true; //We are ahead. Skip rendeirng.
        
        if (!pause) {
            if (sleepFrame == true && timeSync == true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(realTime - gameTime));
                sleptFrames++;
            }
            updateNES();
            if (dropFrame == true && timeSync == true){
                printf("Skipping frame\n");
                droppedFrames++;
            } else {
                updateDisplay();
                updateAudio();
            }
            frameCount++;
        } else {
            startTime = glfwGetTime() * 1000;
            frameCount = 0;
        }
        glfwPollEvents();
    }
    Pa_StopStream(audioStream);
    glfwTerminate();
}

int main(int argc, char** argv) {
    audioError = Pa_Initialize();
    if (audioError != paNoError) {
        printf("Error initalizing Audio\n");
        exit(0);
    }

    PaStreamParameters output;
    output.device = Pa_GetDefaultOutputDevice();
    output.channelCount = 1;
    output.sampleFormat = paFloat32;
    output.suggestedLatency = Pa_GetDeviceInfo(output.device)->defaultHighOutputLatency;
    output.hostApiSpecificStreamInfo = NULL;

    Pa_OpenStream(&audioStream,
        NULL,
        &output,
        44100,
        735,
        paClipOff,
        NULL,
        NULL);
    display = new Display(4, 256, 240);
    display->initialize(argc, argv);
    glfwSetKeyCallback(display->window, keyCallback);
    nes = new NES::Console();
    
    //Test Roms
    
    //CPU
    //nes->loadProgram("roms/Test/cpu/branch_timing_tests/1.Branch_Basics.nes");
    //nes->loadProgram("roms/Test/cpu/branch_timing_tests/2.Backward_Branch.nes");
    //nes->loadProgram("roms/Test/cpu/branch_timing_tests/3.Forward_Branch.nes");
    //nes->loadProgram("roms/Test/cpu/cpu_dummy_reads/cpu_dummy_reads.nes");
    //nes->loadProgram("roms/Test/cpu/cpu_exec_space/test_cpu_exec_space_apu.nes");
    //nes->loadProgram("roms/Test/cpu/cpu_exec_space/test_cpu_exec_space_ppuio.nes");
    //nes->loadProgram("roms/Test/cpu/cpu_interrupts_v2/cpu_interrupts.nes");
    //nes->loadProgram("roms/Test/cpu/cpu_reset/ram_after_reset.nes");
    //nes->loadProgram("roms/Test/cpu/cpu_reset/registers.nes");
    //nes->loadProgram("roms/Test/cpu/instr_misc/instr_misc.nes");
    //nes->loadProgram("roms/Test/cpu/instr_test-v5/all_instrs.nes");
    //nes->loadProgram("roms/Test/cpu/instr_test-v5/official_only.nes");
    //nes->loadProgram("roms/Test/cpu/instr_timing/instr_timing.nes");
    //nes->loadProgram("roms/Test/cpu/nestest/nestest.nes");
    //nes->loadProgram("roms/Test/cpu/ram_retain/ram_retain.nes");
    
    //PPU
    //nes->loadProgram("roms/Test/ppu/blargg_ppu_tests_2005.09.15b/palette_ram.nes");
    //nes->loadProgram("roms/Test/ppu/blargg_ppu_tests_2005.09.15b/power_up_palette.nes");
    //nes->loadProgram("roms/Test/ppu/blargg_ppu_tests_2005.09.15b/sprite_ram.nes");
    //nes->loadProgram("roms/Test/ppu/blargg_ppu_tests_2005.09.15b/vbl_clear_time.nes");
    //nes->loadProgram("roms/Test/ppu/blargg_ppu_tests_2005.09.15b/vram_access.nes");
    //nes->loadProgram("roms/Test/ppu/color_test/color_test.nes");
    //nes->loadProgram("roms/Test/ppu/full_palette/full_palette.nes");
    //nes->loadProgram("roms/Test/ppu/full_palette/full_palette_alt.nes");
    //nes->loadProgram("roms/Test/ppu/nmi_sync/demo_ntsc.nes");
    //nes->loadProgram("roms/Test/ppu/ntsc_torture/ntsc_torture.nes");
    //nes->loadProgram("roms/Test/ppu/oam_read/oam_read.nes");
    //nes->loadProgram("roms/Test/ppu/oam_stress/oam_stress.nes");
    //nes->loadProgram("roms/Test/ppu/palette/palette.nes");
    //nes->loadProgram("roms/Test/ppu/ppu_open_bus/ppu_open_bus.nes");
    //nes->loadProgram("roms/Test/ppu/ppu_sprite_hit/ppu_sprite_hit.nes");
    //nes->loadProgram("roms/Test/ppu/ppu_sprite_overflow/ppu_sprite_overflow.nes");
    //nes->loadProgram("roms/Test/ppu/ppu_vbl_nmi/ppu_vbl_nmi.nes");
    //nes->loadProgram("roms/Test/ppu/scanline/scanline.nes");
    //nes->loadProgram("roms/Test/ppu/sprdma_and_dmc_dma/sprdma_and_dmc_dma.nes");
    //nes->loadProgram("roms/Test/ppu/sprdma_and_dmc_dma/sprdma_and_dmc_dma_512.nes");
    //nes->loadProgram("roms/Test/ppu/tvpassfail/tv.nes");
    
    //Games
    //nes->loadProgram("../roms/PunchOut.nes");
    //nes->loadProgram("roms/Battletoads.nes");
    //nes->loadProgram("../roms/Gradius.nes");
    //nes->loadProgram("../roms/Contra.nes");
    //nes->loadProgram("../roms/Metroid.nes");
    //nes->loadProgram("../roms/IceClimber.nes");
    //nes->loadProgram("../roms/Megaman3.nes");
    //nes->loadProgram("../roms/Castlevania.nes");
    //nes->loadProgram("../roms/Zelda.nes");
    //nes->loadProgram("../roms/SuperMario3.nes");
    //nes->loadProgram("../roms/Mario.nes");
    //nes->loadProgram("../roms/Excitebike.nes");
    //nes->loadProgram("../roms/DonkeyKong.nes");

    if (showLog) {
        logLoop();
    } else {
        gameLoop();
    }
    return 0;
}
