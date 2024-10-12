#include "pointlight.h"

PointLight::PointLight(V3 lp, float ka, float kd, float res) {
	this->lp = lp;
	this->ka = ka;
	this->kd = kd;
	this->res = res;
	
	for (int i = 0; i < 6; i++) {
		cubeMap[i] = PPC(90, res, res);
		zbs[i] = new float[(int)(this->res * this->res)];
	}
	cubeMap[0].setPose(lp, V3(1.0f, 0.0f, 0.0f), V3(0.0f, 1.0f, 0.0f));
	cubeMap[1].setPose(lp, V3(0.0f, 1.0f, 0.0f), V3(0.0f, 0.0f, 1.0f));
	cubeMap[2].setPose(lp, V3(0.0f, 0.0f, 1.0f), V3(0.0f, 1.0f, 0.0f));
	cubeMap[3].setPose(lp, V3(-1.0f, 0.0f, 0.0f), V3(0.0f, -1.0f, 0.0f));
	cubeMap[4].setPose(lp, V3(0.0f, -1.0f, 0.0f), V3(0.0f, 0.0f, -1.0f));
	cubeMap[5].setPose(lp, V3(0.0f, 0.0f, -1.0f), V3(0.0f, -1.0f, 0.0f));
}
