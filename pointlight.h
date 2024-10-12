#pragma once

#include "PPC.h"
#include "V3.h"

class PointLight {
public:
	V3 lp; // location of point light
	float ka, kd, res; // ambient and diffuse constants
	float* zbs[6]; // z buffers per camera
	PPC cubeMap[6]; // a PPC for each axis, 90 deg FOV

	PointLight() {};
	PointLight(V3 lp, float ka, float kd, float res);

};