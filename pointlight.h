#pragma once

#include <vector>

#include "PPC.h"
#include "V3.h"
#include "TM.h"
#include "M33.h"

class PointLight : public V3{
public:
	V3 lp; // location of point light
	float ka, kd; // ambient and diffuse constants
	int res; // square resolution of shadow map
	PPC cubeMap[6]; // a PPC for each axis, 90 deg FOV
	float ep; // epsilon for in shadow comparison

	PointLight() {};
	PointLight(V3 lp, float ka, float kd, int res);

	V3 lightPixel(V3 p, V3 pPos, V3 pCol, V3 pNorm);

	int selectCam(V3 p); // figures out which camera a given point should be projected to 
	void setShadowMaps(TM *tms, int tmsN); // set up z buffers for cameras to determine what pixels are in shadow
	void clipAndProjectTris(M33 tvs); // check if tri verts project to same cube face
	void updateZB(M33 tvs, int face); // update the z buffer of the given face
	bool inShadow(V3 p); // returns whether point is in shadow
	

	vector<M33> clipTrisToFaces(M33 tri, vector<int> faces);
	vector<M33> clipTriToFace(M33 tri, int face);
	vector<pair<V3, V3>> getClippingPlanes(int f);
	vector<V3> clipToPlane(vector<V3> tri, V3 planeNorm, V3 planePoint); 

};