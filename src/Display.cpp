#include "Display.h"
#include <GL/GLUT.h>

Display::Display(float scale, int width, int  height) {
    this->scale = scale;
    this->width = width;
    this->height = height;
}

void Display::plotPixel(int x, int y, float r, float g, float b) {

    float scaledWidth = float(this->scale * this->width);
    float scaledHeight = float(this->scale * this->height);
    float normX = ((float(x * this->scale) / scaledWidth) * 2.0) - 1.0;
    float normY = 1.0 - ((float(y * this->scale) / scaledHeight) * 2.0);
    float xDistance = 2.0 / float(this->width);
    float yDistance = 2.0 / float(this->height);

    glBegin(GL_POLYGON);
    glColor3f(r, g, b);
    glVertex3f(normX, normY, 0.0);
    glVertex3f(normX, normY - yDistance, 0.0);
    glVertex3f(normX + xDistance, normY - yDistance, 0.0);
    glVertex3f(normX + xDistance, normY, 0.0);

    glEnd();
    
}

void Display::initialize(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(this->width * this->scale, this->height * this->scale);
    glutCreateWindow("Nintendoish");
}

Display::~Display() {
}
