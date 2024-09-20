#pragma once

#include "V3.h"
#include "framebuffer.h"
#include "PPC.h"

class TM{
public:
	V3 *verts, *colors, *normals;
	int onFlag;
	int vertsN;
	unsigned int *tris;
	int trisN;
	TM(): onFlag(1), verts(0), vertsN(0), tris(0), trisN(0), colors(0), normals(0) {};
	void setRectangle(float rw, float rh);
	void allocateMemory();
	void translate(V3 tv);
	void renderWF(FrameBuffer *fb, PPC *ppc);
	V3 centroid();
	void loadBin(char *fname);
	void scaleInPlace(float scf);
	void rotate(V3 aO, V3 aD, float theta);

};
