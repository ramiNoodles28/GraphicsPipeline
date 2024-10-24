#pragma once

#include <vector>

#include "gui.h"
#include "framebuffer.h"
#include "PPC.h"
#include "TM.h"
#include "pointlight.h"

class Scene {
public:

	GUI *gui;
	FrameBuffer *fb;
	PPC *ppc;
	TM *tms;
	int tmsN;

	int lightingMode;
	int lightType;
	float ka;
	float kd;
	V3 lv, lp, lp1;
	int lNum;
	PointLight* pLight; // point light objects

	int texType;
	int mirrorTiles;
	int tileNum;
	Texture* tex;

	Scene();
	void Render();
	void DBG();
	void FreeCam();
	void LightControl();

	void TileMinus();
	void TilePlus();
	void TileMirror();

	// old stuff


};

extern Scene *scene;