#pragma once

#include "glfw3.h"

class Display {
    float scale;
    int width;
    int height;
public:
    unsigned int mainTexture;
    Display(float scale, int width, int  height);
    void drawBuffer(unsigned int* pixelBuffer);
    void initialize(int argc, char **argv);
    GLFWwindow* window;
    ~Display();
};

