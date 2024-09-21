#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "V3.h"
#include "M33.h"

class PPC;

class FrameBuffer : public Fl_Gl_Window {
public:
	unsigned int *pix; // pixel array
	int w, h; // width and height of frame
	int s; // scale of camera movement
	PPC *cam;
	FrameBuffer(int u0, int v0, int _w, int _h);
	void addCam(PPC* c);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void loadTiff(char* fname);
	void saveAsTiff(char* fname);
	void set(unsigned int bgr);
	void set(int u, int v, unsigned int col);
	void setGuarded(int u, int v, unsigned int col);
	void rasterizeRectangle(int u0, int v0, int l, int h, unsigned int col);
	int clipRectangle(int& u0, int& v0, int& rw, int& rh);
	void rasterizeCircle(V3 center, float radius, unsigned int color);
	void rasterizeTriLines(V3 p0, V3 p1, V3 p2, unsigned int color);
	void rasterizeTris(V3 p0, V3 p1, V3 p2, unsigned int color);
	float edgeFunction(V3 a, V3 b, V3 p); // returns if point is on right side of edge

	void rasterize2DSegment(V3 p0, V3 p1, unsigned int color);
	void rasterize2DSegment(V3 p0, V3 p1, V3 c0, V3 c1);
	void render3DSegment(V3 p0, V3 p1, V3 c0, V3 c1, PPC *ppc);
};