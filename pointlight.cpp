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
	cubeMap[0].setPose(lp, V3(1.0f, 0.0f, 0.0f), V3(0.0f, 1.0f, 0.0f)); // +x
	cubeMap[1].setPose(lp, V3(0.0f, 1.0f, 0.0f), V3(0.0f, 0.0f, 1.0f)); // +y
	cubeMap[2].setPose(lp, V3(0.0f, 0.0f, 1.0f), V3(0.0f, 1.0f, 0.0f)); // +z
	cubeMap[3].setPose(lp, V3(-1.0f, 0.0f, 0.0f), V3(0.0f, -1.0f, 0.0f)); // -x
	cubeMap[4].setPose(lp, V3(0.0f, -1.0f, 0.0f), V3(0.0f, 0.0f, -1.0f)); // -y
	cubeMap[5].setPose(lp, V3(0.0f, 0.0f, -1.0f), V3(0.0f, -1.0f, 0.0f)); // -z
}

int PointLight::selectCam(V3 p) {
	V3 dir = p - lp;
	dir = dir.normalize();
	float x = fabs(dir[0]);
	float y = fabs(dir[1]);
	float z = fabs(dir[2]);
	if (x >= y && x >= z) return (dir[0] > 0) ? 0 : 3;
	if (y >= x && y >= z) return (dir[1] > 0) ? 1 : 4;
	return (dir[2] > 0) ? 2 : 5;
}// figures out which camera a given point should be projected to 

void PointLight::setShadowMaps(TM* tms) {


	return;
} // set up z buffers for cameras to determine what pixels are in shadow
