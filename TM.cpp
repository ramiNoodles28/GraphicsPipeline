#include "TM.h"

using namespace std;

#include <iostream>
#include <fstream>
#include <strstream>

void TM::allocateMemory() {
	verts = new V3[vertsN];
	colors = new V3[vertsN];
	bakedColors = new V3[vertsN];
	normals = new V3[vertsN];
	tris = new unsigned int[3*trisN];
}

void TM::setRectangle(float rw, float rh) {
	vertsN = 4;
	trisN = 2;
	allocateMemory();
	verts[0] = V3(-rw / 2.0f, +rh / 2.0f, 0.0f);
	verts[1] = V3(-rw / 2.0f, -rh / 2.0f, 0.0f);
	verts[2] = V3(+rw / 2.0f, -rh / 2.0f, 0.0f);
	verts[3] = V3(+rw / 2.0f, +rh / 2.0f, 0.0f);
	int tri = 0;
	tris[tri * 3 + 0] = 0; tris[tri * 3 + 1] = 1; tris[tri * 3 + 2] = 2; tri++;
	tris[tri * 3 + 0] = 2; tris[tri * 3 + 1] = 3; tris[tri * 3 + 2] = 0; tri++;
}

void TM::setGroundPlane(V3 center, V3 color, float s) {
	vertsN = 4;
	trisN = 2;
	allocateMemory();
	verts[0] = center + V3(s/2.0f, 0, s / 2.0f);
	verts[1] = center + V3(s / 2.0f, 0, s / -2.0f);
	verts[2] = center + V3(s / -2.0f, 0, s / -2.0f);
	verts[3] = center + V3(s / -2.0f, 0, s / 2.0f);
	int tri = 0;
	tris[tri * 3 + 0] = 0; tris[tri * 3 + 1] = 1; tris[tri * 3 + 2] = 2; tri++;
	tris[tri * 3 + 0] = 2; tris[tri * 3 + 1] = 3; tris[tri * 3 + 2] = 0; tri++;
	for (int i = 0; i < 4; i++) {
		normals[i] = V3(0, 1, 0);
		colors[i] = color;
		bakedColors[i] = color;
	}
}

TM TM::boundingbox() {
	TM& t = *this;
	V3 maxes = verts[0];
	V3 mins = verts[0];
	for (int i = 1; i < vertsN; i++) {
		for (int axis = 0; axis < 3; axis++) {
			maxes[axis] = (maxes[axis] < verts[i][axis]) ? verts[i][axis] : maxes[axis];
			mins[axis] = (mins[axis] > verts[i][axis]) ? verts[i][axis] : mins[axis];
		}
	}
	TM box;
	box.vertsN = 8;
	box.trisN = 12;
	box.allocateMemory();
	box.verts[0] = maxes;
	box.verts[1] = V3(maxes[0], maxes[1], mins[2]);
	box.verts[2] = V3(maxes[0], mins[1], maxes[2]);
	box.verts[3] = V3(mins[0], maxes[1], maxes[2]);
	box.verts[4] = V3(mins[0], mins[1], maxes[2]);
	box.verts[5] = V3(mins[0], maxes[1], mins[2]);
	box.verts[6] = V3(maxes[0], mins[1], mins[2]);
	box.verts[7] = mins;

	int tri = 0;
	// top face
	box.tris[tri * 3] = 0; box.tris[tri * 3 + 1] = 5; box.tris[tri * 3 + 2] = 3; tri++;
	box.tris[tri * 3] = 0; box.tris[tri * 3 + 1] = 1; box.tris[tri * 3 + 2] = 5; tri++;
	// front face
	box.tris[tri * 3] = 7; box.tris[tri * 3 + 1] = 1; box.tris[tri * 3 + 2] = 6; tri++;
	box.tris[tri * 3] = 7; box.tris[tri * 3 + 1] = 5; box.tris[tri * 3 + 2] = 1; tri++;
	// right face
	box.tris[tri * 3] = 0; box.tris[tri * 3 + 1] = 6; box.tris[tri * 3 + 2] = 1; tri++;
	box.tris[tri * 3] = 0; box.tris[tri * 3 + 1] = 2; box.tris[tri * 3 + 2] = 6; tri++;
	// back face
	box.tris[tri * 3] = 0; box.tris[tri * 3 + 1] = 4; box.tris[tri * 3 + 2] = 2; tri++;
	box.tris[tri * 3] = 0; box.tris[tri * 3 + 1] = 3; box.tris[tri * 3 + 2] = 4; tri++;
	// left face
	box.tris[tri * 3] = 7; box.tris[tri * 3 + 1] = 3; box.tris[tri * 3 + 2] = 5; tri++;
	box.tris[tri * 3] = 7; box.tris[tri * 3 + 1] = 4; box.tris[tri * 3 + 2] = 3; tri++;
	// bottom face
	box.tris[tri * 3] = 7; box.tris[tri * 3 + 1] = 2; box.tris[tri * 3 + 2] = 4; tri++;
	box.tris[tri * 3] = 7; box.tris[tri * 3 + 1] = 6; box.tris[tri * 3 + 2] = 2; tri++;
	return box;
}

void TM::translate(V3 tv) {

	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi] + tv;
	}

}

V3 TM::centroid() {

	V3 ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < vertsN; vi++) {
		ret = ret + verts[vi];
	}
	ret = ret * (1.0f / (float)vertsN);
	return ret;

}

void TM::loadBin(char *fname) {
	ifstream ifs(fname, ios::binary);
	if (ifs.fail()) {
		cerr << "INFO: cannot open file: " << fname << endl;
		return;
	}
	ifs.read((char*)&vertsN, sizeof(int));
	char yn;
	ifs.read(&yn, 1); // always xyz
	if (yn != 'y') {
		cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
		return;
	}
	if (verts)
		delete verts;
	verts = new V3[vertsN];
	ifs.read(&yn, 1); // cols 3 floats
	if (colors) {
		delete colors;
		colors = 0;
	}
	if (yn == 'y')
		colors = new V3[vertsN];
	
	if (bakedColors) {
		delete bakedColors;
		bakedColors = 0;
	}
	if (yn == 'y')
		bakedColors = new V3[vertsN];
		
	ifs.read(&yn, 1); // normals 3 floats
	if (normals)
		delete normals;
	normals = 0;
	if (yn == 'y')
		normals = new V3[vertsN];
	ifs.read(&yn, 1); // texture coordinates 2 floats
	float *tcs = 0; // don't have texture coordinates for now
	if (tcs)
		delete tcs;
	tcs = 0;
	if (yn == 'y')
		tcs = new float[vertsN * 2];
	ifs.read((char*)verts, vertsN * 3 * sizeof(float)); // load verts
	if (colors)
		ifs.read((char*)colors, vertsN * 3 * sizeof(float)); // load cols
	if (normals)
		ifs.read((char*)normals, vertsN * 3 * sizeof(float)); // load normals
	if (tcs)
		ifs.read((char*)tcs, vertsN * 2 * sizeof(float)); // load texture coordinates
	ifs.read((char*)&trisN, sizeof(int));
	if (tris)
		delete tris;
	tris = new unsigned int[trisN * 3];
	for (int i = 0; i < vertsN; i++) {
		bakedColors[i] = colors[i];
	}
	ifs.read((char*)tris, trisN * 3 * sizeof(unsigned int)); // read tiangles
	ifs.close();
	cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	cerr << "      xyz " << ((colors) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;
}

void TM::scaleInPlace(float scf) {
	V3 oldCenter = centroid();
	translate(oldCenter * -1.0f);
	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi] * scf;
	translate(oldCenter);
}

void TM::rotate(V3 aO, V3 aD, float theta) {
	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi].rotateAboutAxis(aO, aD, theta);
}

void TM::resetAllColors() {
	for (int i = 0; i < vertsN; i++) {
		colors[i] = bakedColors[i];
	}
}

void TM::setAllColors(V3 c) {
	if (!colors)
		return;
	for (int vi = 0; vi < vertsN; vi++)
		colors[vi] = c;
}

void TM::lightMeshDirRGB(V3 lv, float ka) {
	if (!colors)
		return;
	for (int vi = 0; vi < vertsN; vi++)
		colors[vi] = colors[vi].lightColor(lv, ka, normals[vi]);
}

void TM::lightMeshPointRGB(V3 lp, float ka) {
	if (!colors)
		return;
	for (int vi = 0; vi < vertsN; vi++)
		colors[vi] = colors[vi].lightColor((lp - verts[vi]).normalize(), ka, normals[vi]);
}

void TM::lightMeshBW(V3 lv, float ks, float ka) {
	if (!colors)
		return;
	for (int vi = 0; vi < vertsN; vi++)
		colors[vi] = V3(ks,ks,ks).lightColor(lv, ka, normals[vi]);
}
