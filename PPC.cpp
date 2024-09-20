
#include "PPC.h"
#include "M33.h"
#include "framebuffer.h"
#include <iostream>
#include <fstream>
#include <strstream>

PPC::PPC(float hfov, int _w, int _h) {
	w = _w;
	h = _h;
	C = V3(0.0f, 0.0f, 0.0f);
	a = V3(1.0f, 0.0f, 0.0f);
	b = V3(0.0f, -1.0f, 0.0f);
	float hfovr = hfov / 180.0f * 3.1415926f;
	float f = (float)w / (2.0f * tanf(hfovr/2.0f));
	c = V3(-(float)w/2.0f, (float)h/2.0f, -f);
} // Constructor from field of view, and w/h of image frame

int PPC::project(V3 P, V3& Q) {
	M33 cam; 
	cam.setCol(0, a);
	cam.setCol(1, b);
	cam.setCol(2, c);
	M33 cami = cam.invert();
	Q = cami*(P - C);
	if (Q[2] < 0.0f)
		return 0;
	Q[0] = Q[0] / Q[2];
	Q[1] = Q[1] / Q[2];
	return 1;
} // project point onto image plane

void PPC::translate(V3 tv) {
	C = C + tv;
} // translate camera

void PPC::setPose(V3 newC, V3 newVD, V3 newUpG) {
	float f = getFocalLength();
	V3 newPP = newC + newVD * f;
	V3 newa = (newVD ^ newUpG).normalize();
	V3 newb = (newVD ^ newa).normalize();
	V3 newc = newPP - newa*((float)w / 2.0f) - newb*((float)h / 2.0f) - newC;
	a = newa;
	b = newb;
	c = newc;
	C = newC;
} // set camera to new pose

void PPC::pan(float theta) {
	a = a.rotateAboutAxis(C, C - b, theta);
	b = b.rotateAboutAxis(C, C - b, theta);
	c = c.rotateAboutAxis(C, C - b, theta);
} // pan camera (rotate about y axis of eye)

void PPC::tilt(float theta) {
	a = a.rotateAboutAxis(C, C + a, theta);
	b = b.rotateAboutAxis(C, C + a, theta);
	c = c.rotateAboutAxis(C, C + a, theta);
} // tilt camera (rotate about x axis of eye)

void PPC::roll(float theta) {
	a = a.rotateAboutAxis(C, C + a ^ b, theta);
	b = b.rotateAboutAxis(C, C + a ^ b, theta);
	c = c.rotateAboutAxis(C, C + a ^ b, theta);
} // roll camera (rotate about z axis of eye)

void PPC::zoom(float scale) {
	float f = getFocalLength() * scale;
	c = (getViewDirection() * f) - (a * w/2.0f) - (b * h/2.0f);
} // camera zoom in/out (change focal length)


void PPC::interpCam(PPC start, PPC end, float interval, float (*interpFunc)(float)) {
	float t = interpFunc(interval);
	a = start.a + (end.a - start.a) * t;
	b = start.b + (end.b - start.b) * t;
	c = start.c + (end.c - start.c) * t;
	C = start.C + (end.C - start.C) * t;
} // interpolate camera between start and end camera given easing function

void PPC::interpCam(PPC start, PPC end, float t) {
	a = start.a + (end.a - start.a) * t;
	b = start.b + (end.b - start.b) * t;
	c = start.c + (end.c - start.c) * t;
	C = start.C + (end.C - start.C) * t;
} // interpolate camera between start and end camera linearly


void PPC::renderWF(FrameBuffer *fb, float visf, PPC *visppc) {
	float scf = visf / getFocalLength();
	V3 bv(0.0f, 0.0f, 0.0f);
	fb->render3DSegment(
		C, 
		C + c*scf, bv, bv, visppc);
	fb->render3DSegment(
		C + c*scf,
		C + (c + a * (float)w)*scf, 
		bv, bv, visppc);
	fb->render3DSegment(
		C + (c + a * (float)w)*scf,
		C + (c + a * (float)w + b * (float) h)*scf, 
		bv, bv, visppc);
	fb->render3DSegment(
		C + (c + a * (float)w + b * (float)h)*scf,
		C + (c + b * (float)h)*scf,
		bv, bv, visppc);
	fb->render3DSegment(
		C + (c + b * (float)h)*scf,
		C + c*scf,
		bv, bv, visppc);
} // render wireframe of camera


V3 PPC::getViewDirection() {
	return (a ^ b).normalize();
} // get view direction vector

float PPC::getFocalLength() {
	return c * getViewDirection();
} 

void PPC::loadFromTxt(char *fname) {
	ifstream ifs(fname);
	if (ifs.is_open()) {
		ifs >> a >> b >> c >> C;
		ifs.close();
	} else {
		cerr << "INFO: cannot open file: " << fname << endl;
	}
	
}

void PPC::saveToTxt(char* fname) {
	ofstream ofs(fname);
	if (ofs.is_open()) {
		ofs << a << " " << b << " " << c << " " << C << endl;
		ofs.close();
	} else {
		cerr << "INFO: cannot open file: " << fname << endl;
	}
	
}

