
#include "PPC.h"

#include "M33.h"

#include "framebuffer.h"

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
	float f = getF();
	V3 newPP = newC + newVD * f;
	V3 newa = (newVD ^ newUpG).normalize();
	V3 newb = (newVD ^ newa).normalize();
	V3 newc = newPP - newa*((float)w / 2.0f) - newb*((float)h / 2.0f) - newC;

	a = newa;
	b = newb;
	c = newc;
	C = newC;
} // set camera to new pose


void PPC::renderWF(FrameBuffer *fb, float visf, PPC *visppc) {
	float scf = visf / getF();
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


V3 PPC::getVD() {
	return (a ^ b).normalize();
} // get view direction vector

float PPC::getF() {
	return c*getVD();
} 
