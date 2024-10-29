#include "envmap.h"

EnvMap::EnvMap(char* fname) {
	cross.loadTiff(fname);
	size = cross.w / 3.0f;

	textures[0] = cross.getRange(V3(0, size), size); // +x (left)
	textures[1] = cross.getRange(V3(size, 0), size); // +y (top)
	textures[2] = cross.getRange(V3(size, size), size); // +z (front)
	textures[3] = cross.getRange(V3(size * 2.0f, size), size); // -x (right)
	textures[4] = cross.getRange(V3(size, size * 2.0f), size); // -y (bottom)
	textures[5] = cross.getRange(V3(size, size * 2.0f), size); // -z (back)
}

int EnvMap::selectFace(V3 ray) {
	float x = fabs(ray[0]);
	float y = fabs(ray[1]);
	float z = fabs(ray[2]);
	if (x >= y && x >= z) return (ray[0] > 0) ? 0 : 3;
	if (y >= x && y >= z) return (ray[1] > 0) ? 1 : 4;
	return (ray[2] > 0) ? 2 : 5;
} // figures out which face of the cubemap the ray is pointing to


V3 EnvMap::lookup(V3 ray) {
	int face = selectFace(ray);

	return V3(1, 0, 0);
} // returns color of pixel hit by eye ray

