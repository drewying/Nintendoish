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
string compareFile = "cli_latency.log";
int debugStartLineNumber = 64000;
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
        case GLFW_KEY_R:
            if (isKeyDown) nes->reset();
            break;
        default:
            break;
    }
}

string getLogLine() {
    std::stringstream ss;

    uint8_t loadedInstruction = nes->cpu->loadedInstruction;

    ss << uppercase << setfill('0') << setw(4) << hex << nes->cpu->reg.PC << "  " << uppercase << setfill('0') << setw(2) << hex << (uint16_t)loadedInstruction;
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
    ss << "  " << nes->cpu->debugTable[loadedInstruction];
    ss << "                             A:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.A;
    ss << " X:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.X;
    ss << " Y:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.Y;
    ss << " P:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.P.byte;
    ss << " SP:" << uppercase << setfill('0') << setw(2) << hex << (uint16_t)nes->cpu->reg.S;
    ss << " CYC:" << setfill(' ') << dec << setw(3) << nes->ppu->currentCycle << " SL:" << nes->ppu->currentScanline;
    return ss.str();
}

void compareLogsLoop() {
    string logEmuLine;
    ifstream logFile(compareFile);
    while (!glfwWindowShouldClose(display->window)) {
        if (!pause) {
            if (nes->cpu->stallCycles <= 0x2) {
                lineNumber++;
                getline(logFile, logEmuLine);
                string myEmuLine = getLogLine();
                string myInstruction = myEmuLine.substr(16, 3);
                string logInstruction = logEmuLine.substr(16, 3);
                if (myInstruction != logInstruction || lineNumber > debugStartLineNumber) {
                    cout << myEmuLine << endl;
                    cout << logEmuLine << endl << endl;
                    if (myInstruction != logInstruction) {
                        cout << "Instructions Differ" << endl;
                    }
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

void logLoop() {
    while (!glfwWindowShouldClose(display->window)) {
        if (!pause) {
            if (nes->cpu->stallCycles == 0x2) {
                lineNumber++;
                if (lineNumber > debugStartLineNumber) {
                    cout << lineNumber << " " << getLogLine() << endl;
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
    //* nes->loadProgram("roms/test/cpu/branch_timing_tests/1.Branch_Basics.nes");
    //* nes->loadProgram("roms/test/cpu/branch_timing_tests/2.Backward_Branch.nes");
    //* nes->loadProgram("roms/test/cpu/branch_timing_tests/3.Forward_Branch.nes");
    //nes->loadProgram("roms/test/cpu/cpu_interrupts_v2/cpu_interrupts.nes");
    //nes->loadProgram("roms/test/cpu/cpu_reset/ram_after_reset.nes");
    //nes->loadProgram("roms/test/cpu/cpu_reset/registers.nes");
    //* nes->loadProgram("roms/test/cpu/cpu_timing_test6/cpu_timing_test.nes");
    //nes->loadProgram("roms/test/cpu/instr_misc/instr_misc.nes");
    //nes->loadProgram("roms/test/cpu/instr_test-v5/all_instrs.nes");
    //* nes->loadProgram("roms/test/cpu/instr_test-v5/official_only.nes");
    //* nes->loadProgram("roms/test/cpu/instr_timing/instr_timing.nes");
    
    //PPU
    //* nes->loadProgram("roms/test/ppu/blargg_ppu_tests_2005.09.15b/palette_ram.nes");
    //N nes->loadProgram("roms/test/ppu/blargg_ppu_tests_2005.09.15b/power_up_palette.nes");
    //nes->loadProgram("roms/test/ppu/blargg_ppu_tests_2005.09.15b/sprite_ram.nes");
    //* nes->loadProgram("roms/test/ppu/blargg_ppu_tests_2005.09.15b/vbl_clear_time.nes");
    //* nes->loadProgram("roms/test/ppu/blargg_ppu_tests_2005.09.15b/vram_access.nes");
    //nes->loadProgram("roms/test/ppu/full_palette/full_palette.nes");
    //nes->loadProgram("roms/test/ppu/full_palette/full_palette_alt.nes");
    //nes->loadProgram("roms/test/ppu/nmi_sync/demo_ntsc.nes");
    //* nes->loadProgram("roms/test/ppu/oam_read/oam_read.nes");
    //nes->loadProgram("roms/test/ppu/oam_stress/oam_stress.nes");
    //nes->loadProgram("roms/test/ppu/ppu_open_bus/ppu_open_bus.nes");
    //* nes->loadProgram("roms/test/ppu/ppu_sprite_hit/ppu_sprite_hit.nes");
    //nes->loadProgram("roms/test/ppu/ppu_sprite_overflow/ppu_sprite_overflow.nes");
    //* nes->loadProgram("roms/test/ppu/ppu_vbl_nmi/ppu_vbl_nmi.nes");
    //nes->loadProgram("roms/test/ppu/sprdma_and_dmc_dma/sprdma_and_dmc_dma.nes");
    //nes->loadProgram("roms/test/ppu/sprdma_and_dmc_dma/sprdma_and_dmc_dma_512.nes");

    //APU
    //nes->loadProgram("roms/test/apu/apu_mixer/dmc.nes");
    //nes->loadProgram("roms/test/apu/apu_mixer/noise.nes");
    //nes->loadProgram("roms/test/apu/apu_mixer/square.nes");
    //nes->loadProgram("roms/test/apu/apu_mixer/triangle.nes");
    //nes->loadProgram("roms/test/apu/apu_reset/4015_cleared.nes");
    //nes->loadProgram("roms/test/apu/apu_reset/4017_timing.nes");
    //nes->loadProgram("roms/test/apu/apu_reset/4017_written.nes");
    //* nes->loadProgram("roms/test/apu/apu_reset/irq_flag_cleared.nes");
    //* nes->loadProgram("roms/test/apu/apu_reset/len_ctrs_enabled.nes");
    //nes->loadProgram("roms/test/apu/apu_reset/works_immediately.nes");
    //nes->loadProgram("roms/test/apu/apu_test/apu_test.nes");
    //* nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/01.len_ctr.nes");
    //* nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/02.len_table.nes");
    //* nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/03.irq_flag.nes");
    //* nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/04.clock_jitter.nes");
    //nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/05.len_timing_mode0.nes");
    //nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/06.len_timing_mode1.nes");
    //nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/07.irq_flag_timing.nes");
    //nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/08.irq_timing.nes");
    //nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/09.reset_timing.nes");
    //* nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/10.len_halt_timing.nes");
    //nes->loadProgram("roms/test/apu/blargg_apu_2005.07.30/11.len_reload_timing.nes");
    //nes->loadProgram("roms/test/apu/dmc_dma_during_read4/dma_2007_read.nes");
    //* nes->loadProgram("roms/test/apu/dmc_dma_during_read4/dma_2007_write.nes");
    //nes->loadProgram("roms/test/apu/dmc_dma_during_read4/dma_4016_read.nes");
    //nes->loadProgram("roms/test/apu/dmc_dma_during_read4/double_2007_read.nes");
    //nes->loadProgram("roms/test/apu/dmc_dma_during_read4/read_write_2007.nes");
    //nes->loadProgram("roms/test/apu/square_timer_div2/square_timer_div2.nes");
    //nes->loadProgram("roms/test/apu/test_apu_env/test_apu_env.nes");
    //nes->loadProgram("roms/test/apu/test_apu_sweep/sweep_cutoff.nes");
    //nes->loadProgram("roms/test/apu/test_apu_sweep/sweep_sub.nes");
    //nes->loadProgram("roms/test/apu/test_apu_timers/dmc_pitch.nes");
    //nes->loadProgram("roms/test/apu/test_apu_timers/noise_pitch.nes");
    //nes->loadProgram("roms/test/apu/test_apu_timers/square_pitch.nes");
    //nes->loadProgram("roms/test/apu/test_apu_timers/triangle_pitch.nes");
    //nes->loadProgram("roms/test/apu/test_tri_lin_ctr/lin_ctr.nes");

    //Games
    //nes->loadProgram("../roms/PunchOut.nes");
    nes->loadProgram("roms/games/Battletoads.nes");
    //nes->loadProgram("../roms/Gradius.nes");
    //nes->loadProgram("../roms/Contra.nes");
    //nes->loadProgram("../roms/Metroid.nes");
    //nes->loadProgram("../roms/IceClimber.nes");
    //nes->loadProgram("../roms/Megaman3.nes");
    //nes->loadProgram("roms/games/Castlevania2.nes");
    //nes->loadProgram("roms/games/Zelda.nes");
    //nes->loadProgram("../roms/SuperMario3.nes");
    //nes->loadProgram("../roms/Mario.nes");
    //nes->loadProgram("../roms/Excitebike.nes");
    //nes->loadProgram("../roms/DonkeyKong.nes");
    
    //nes->loadProgram("roms/test/cpu/cpu_interrupts_v2/rom_singles/1-cli_latency.nes");

    if (compareFile.length() > 0 && showLog) {
        compareLogsLoop();
    } else if (showLog) {
        logLoop();
    } else {
        gameLoop();
    }
    return 0;
}
