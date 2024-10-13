#pragma once

#include "PPC.h"
#include "V3.h"
#include "TM.h"

class PointLight {
public:
	V3 lp; // location of point light
	float ka, kd, res; // ambient and diffuse constants
	float* zbs[6]; // z buffers per camera
	PPC cubeMap[6]; // a PPC for each axis, 90 deg FOV

	PointLight() {};
	PointLight(V3 lp, float ka, float kd, float res);

	int selectCam(V3 p); // figures out which camera a given point should be projected to 
	void setShadowMaps(TM *tms); // set up z buffers for cameras to determine what pixels are in shadow
};