#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "PPC.h"
#include "TM.h"

class Scene {
public:

	GUI *gui;
	FrameBuffer *fb;
	PPC *ppc;
	TM *tms;
	int tmsN;
	Scene();
	void DBG();
	void NewButton();
	void Render();

	void RotatingPoints(); // Generate points rotated about an axis and save to file
	void NameCycle(); // rasterize my name and have it translate across frame
};

extern Scene *scene;