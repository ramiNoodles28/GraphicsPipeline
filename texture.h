﻿#pragma once
#include "V3.h"

class Texture {
public:
	unsigned int* pix; // pixel array
	int w, h; // width and height of frame

	Texture(int w, int h);
	unsigned int get(int u, int v);
	void set(int u, int v, V3 color);
	void loadTiff(char* fname);
	void setChecker(int cSize, V3 rgb0, V3 rgb1);
	void setXTex(V3 rgb0, V3 rgb1);
	
};