#pragma once
#include "texture.h"
#include "PPC.h"
#include "M33.h"


class EnvMap: public Texture {
public:
	Texture cross; // env map image as a cross
	float size; //  side length of cube map
	Texture textures[6]; // environment textures
	PPC cams[6]; // environment cameras


	EnvMap() {};
	EnvMap(char* fname);
	int selectFace(V3 ray); // figures out which face of the cubemap the ray is pointing to
	V3 lookup(V3 ray); // returns color of pixel hit by eye ray
};
