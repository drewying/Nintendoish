#include "Display.h"
#include "glfw3.h"

Display::Display(float scale, int width, int  height) {
    this->scale = scale;
    this->width = width;
    this->height = height;
}

void Display::drawBuffer(unsigned int* pixelBuffer) {
    glBindTexture(GL_TEXTURE_2D, mainTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);

    glBindTexture(GL_TEXTURE_2D, mainTexture);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glTexCoord2i(0, 0); glVertex2f(1, 1);
    glTexCoord2i(0, 1); glVertex2f(1, -1);
    glTexCoord2i(1, 1); glVertex2f(-1, -1);
    glTexCoord2i(1, 0); glVertex2f(-1, 1);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glfwSwapBuffers(window);
}


void Display::initialize(int argc, char **argv) {
    if (!glfwInit()) return;
    window = glfwCreateWindow(width * scale, height * scale, "Nintendoish", NULL, NULL);
    if (!window) glfwTerminate();
    glfwMakeContextCurrent(window);
   
    glGenTextures(1, &mainTexture);
    glBindTexture(GL_TEXTURE_2D, mainTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA , width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glMatrixMode(GL_PROJECTION);
    glOrtho(-1, 1, 1, -1, -1, 1);
}

Display::~Display() {
}
