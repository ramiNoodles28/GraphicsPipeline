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
	FrameBuffer *fb, *hwfb;
	PPC *ppc;
	TM *tms;
	int tmsN;
	int renderMode;

	float fps;
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

	EnvMap* env;

	Scene();
	void Render();
	void RenderHW();
	void renderText(float x, float y, const char* text);
	void DBG();
	void FreeCam();
	void LightControl();

	void TileMinus();
	void TilePlus();
	void TileMirror();

	// old stuff


};

extern Scene *scene;