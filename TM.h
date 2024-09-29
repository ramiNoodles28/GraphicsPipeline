#pragma once

#include "V3.h"
#include "framebuffer.h"
#include "PPC.h"

class TM{
public:
	V3 *verts, *colors, *bakedColors, *normals;
	int onFlag;
	int vertsN;
	unsigned int *tris;
	int trisN;
	TM(): onFlag(1), verts(0), vertsN(0), tris(0), trisN(0), colors(0), bakedColors(0), normals(0) {}; // default constructor
	void setRectangle(float rw, float rh); // makes a basic rectangle mesh
	void allocateMemory(); // allocates memory for new meshes
	void loadBin(char *fname); // loads mesh from binary file
	//void setCylinder(float rad, float height, float radDivs);
	//void setSphere(float rad, float radDivs, float latDivs)

	V3 centroid(); // gets centroid of mesh
	void translate(V3 tv); // translates all points on mesh my given vector
	void scaleInPlace(float scf); // scales mesh by scalar factor
	void rotate(V3 aO, V3 aD, float theta); // rotate mesh around given axis by theta degrees
	TM boundingbox(); // creates a new bounding box mesh from given mesh
	void renderWF(FrameBuffer *fb, PPC *ppc); // renders wireframe of mesh
	void renderTris(FrameBuffer* fb, PPC* ppc); // renders filled in tri mesh
	void renderTrisDirLight(FrameBuffer* fb, PPC* ppc, V3 lv, float ka); // render mesh with directional light
	void renderTrisPointLight(FrameBuffer* fb, PPC* ppc, V3 lp, float ka); // render mesh with point light

	void resetAllColors(); // resets vert colors to original baked values
	void setAllColors(V3 c); // sets all vertex colors to one color 
	void lightMeshDirRGB(V3 lv, float ka); // directionally light the mesh from light vector and min lighting 
	void lightMeshPointRGB(V3 lp, float ka); // point light the mesh from light point and min lighting 

	void lightMeshBW(V3 lv, float ks, float ka); // light the mesh from light vector and min lighting 

};
