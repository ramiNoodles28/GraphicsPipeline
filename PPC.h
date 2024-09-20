#pragma once


#pragma once

#include <ostream>
#include <istream>

using namespace std;

#include "V3.h"
#include "framebuffer.h"

class PPC {
public:
	V3 a, b, c, C; // a and b describe image plane, C is point where eye is, c vector from C to top corner of img plane
	int w, h; // width and height (in pixels) of image frame
	PPC(float hfov, int w, int h); // Constructor from field of view, and w/h of image frame
	int project(V3 P, V3& Q); // project point onto image plane
	void translate(V3 tv); // translate camera
	void setPose(V3 newC, V3 newVD, V3 newUpG); // set camera to new pose
	V3 getVD(); // get view direction vector
	float getF(); // get 
	void renderWF(FrameBuffer *fb, float visf, PPC *visppc); // render wireframe of camera
};
