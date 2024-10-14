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

	Scene();
	void DBG();
	void FreeCam();

	void LightControl();
	void SM1();
	void SM2();
	void SM3();
	void SM23();
	void LightType();

	void Render();
	void loadCamsFromTxt(char* fname, vector<PPC>& ppcs, int camNum);

	void PathCam();
	void RotatingPoints(); // Generate points rotated about an axis and save to file
};

extern Scene *scene;