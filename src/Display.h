#pragma once
class Display
{
	float scale;
	int width;
	int height;
public:
	Display(float scale, int width, int  height);
	void plotPixel(int x, int y, float r, float g, float b);
	void initialize(int argc, char **argv);
	~Display();
};

