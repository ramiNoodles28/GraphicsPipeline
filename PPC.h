#pragma once

#include <ostream>
#include <istream>
//#include "framebuffer.h"
using namespace std;
#include "V3.h"

class PPC {
public:
	V3 a, b, c, C; // a and b describe image plane, C is point where eye is, c vector from C to top corner of img plane
	int w, h; // width and height (in pixels) of image frame
	float* zb; // z buffer
	PPC() {}
	PPC(float hfov, int w, int h); // Constructor from field of view, and w/h of image frame

	int project(V3 P, V3& Q); // project point onto image plane
	void translate(V3 tv); // translate camera
	void setPose(V3 newC, V3 newVD, V3 newUpG); // set camera to new pose
	void pan(float theta); // pan camera (rotate about y axis of eye)
	void tilt(float theta); // tilt camera (rotate about x axis of eye)
	void roll(float theta); // roll camera (rotate about z axis of eye)
	void zoom(float scale); // camera zoom in/out (change focal length)
	void interpCam(PPC start, PPC end, float interval, float (*interpFunc)(float)); // interpolate camera between start and end camera given easing function
	void interpCam(PPC start, PPC end, float t); // interpolate camera between start and end camera linearly


	V3 getViewDirection(); // get view direction vector
	float getFocalLength(); // get focal length
	V3 getRay(int u, int v); // get ray coming from pixel

	void clearZB(); // clear the z buffer
	int isFarther(int u, int v, float z); // check if current pixel z is farther than z buffer
	int isCloser(int u, int v, float z); // check and set if current pixel z is closer than z buffer
	void setZB(int u, int v, float z); // set z buffer at pixel coordinate
	float getZB(int u, int v); // get z buffer from pixel coordinate

	void saveToTxt(char* fname); // save camera parameters to txt file
	void loadFromTxt(char* fname); // load in camera parameters from txt file
	friend istream& operator>>(istream& istr, PPC& p); // Stream input
};
