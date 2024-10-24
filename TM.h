#pragma once

#include "V3.h"
#include "texture.h"

class TM{
public:
	V3 *verts, *colors, *bakedColors, *normals, *texCoords;
	int onFlag, texFlag;
	int vertsN;
	unsigned int *tris;
	int trisN;
	Texture *tex;
	int rw, rh; //texture map resolution

	TM(): onFlag(1), texFlag(0), verts(0), vertsN(0), tris(0), trisN(0),
	colors(0), bakedColors(0), normals(0), texCoords(0), tex(0), rw(0), rh(0) {}; // default constructor
	void setRectangle(float rw, float rh); // makes a basic rectangle mesh
	void setGroundPlane(V3 center, V3 color, float s); // makes a ground plane
	void allocateMemory(); // allocates memory for new meshes
	void loadBin(char *fname); // loads mesh from binary file
	//void setCylinder(float rad, float height, float radDivs);
	//void setSphere(float rad, float radDivs, float latDivs)

	V3 centroid(); // gets centroid of mesh
	void translate(V3 tv); // translates all points on mesh my given vector
	void scaleInPlace(float scf); // scales mesh by scalar factor
	void rotate(V3 aO, V3 aD, float theta); // rotate mesh around given axis by theta degrees
	TM boundingbox(); // creates a new bounding box mesh from given mesh

	void resetAllColors(); // resets vert colors to original baked values
	void setAllColors(V3 c); // sets all vertex colors to one color 
	void lightMeshDirRGB(V3 lv, float ka); // directionally light the mesh from light vector and min lighting 
	void lightMeshPointRGB(V3 lp, float ka); // point light the mesh from light point and min lighting 
	void lightMeshBW(V3 lv, float ks, float ka); // light the mesh from light vector and min lighting 
	
	void setTexRes(int w, int h);
	void scaleTex(float s);

	void setTexture(char* fname);
	void checkerTexture(int cSize, V3 rgb0, V3 rgb1);
	void xTexture(V3 rgb0, V3 rgb1);
};
