#include "pointlight.h"

#include <iostream>

using namespace std;

PointLight::PointLight(V3 lp, float ka, float kd, int res) {
	this->lp = lp;
	this->ka = ka;
	this->kd = kd;
	this->res = res;
	ep = 0.5f;
	
	for (int i = 0; i < 6; i++) {
		cubeMap[i] = PPC(90, res, res);
	}
	cubeMap[0].setPose(lp, V3(1.0f, 0.0f, 0.0f), V3(0.0f, 1.0f, 0.0f)); // +x
	cubeMap[1].setPose(lp, V3(0.0f, 1.0f, 0.0f), V3(0.0f, 0.0f, 1.0f)); // +y
	cubeMap[2].setPose(lp, V3(0.0f, 0.0f, 1.0f), V3(0.0f, 1.0f, 0.0f)); // +z
	cubeMap[3].setPose(lp, V3(-1.0f, 0.0f, 0.0f), V3(0.0f, -1.0f, 0.0f)); // -x
	cubeMap[4].setPose(lp, V3(0.0f, -1.0f, 0.0f), V3(0.0f, 0.0f, -1.0f)); // -y
	cubeMap[5].setPose(lp, V3(0.0f, 0.0f, -1.0f), V3(0.0f, -1.0f, 0.0f)); // -z
}

V3 PointLight::lightPixel(V3 p, V3 pPos, V3 pCol, V3 pNorm) {
	if (inShadow(pPos)) return (pCol * ka);
	V3 lv = (lp - pPos).normalize();
	float dist = (lp - pPos).length();
	float atten = 1.0f;
	float diffuse = max(0.0f, pNorm * lv);
	V3 pixelColor = pCol.lightColor(lv, ka, pNorm);
	pixelColor = pCol * diffuse * atten + (pCol * ka);
	return pixelColor;
}

int PointLight::selectCam(V3 p) {
	V3 dir = p - lp;
	dir = dir.normalize();
	float x = fabs(dir[0]);
	float y = fabs(dir[1]);
	float z = fabs(dir[2]);
	if (x >= y && x >= z) return (dir[0] > 0) ? 0 : 3;
	if (y >= x && y >= z) return (dir[1] > 0) ? 1 : 4;
	return (dir[2] > 0) ? 2 : 5;
}// figures out which camera a given point should be projected to 

void PointLight::setShadowMaps(TM* tms, int tmsN) {
	for (auto& ppc : cubeMap) ppc.clearZB();
	for (int i = 0; i < tmsN; i++) {
		for (int t = 0; t < tms[i].trisN; t++) {
			V3 tvs[3];
			unsigned int vinds[3];
			for (int vi = 0; vi < 3; vi++) {
				vinds[vi] = tms[i].tris[3 * t + vi];
				tvs[vi] = tms[i].verts[vinds[vi]];
			}
			clipAndProjectTris(M33(tvs[0], tvs[1], tvs[2]));
		}
	}
} // set up z buffers for cameras to determine what pixels are in shadow

void PointLight::updateZB(M33 tvs, int face) {
	V3 a, b, c;
	cubeMap[face].project(tvs[0], a);
	cubeMap[face].project(tvs[1], b);
	cubeMap[face].project(tvs[2], c);
	V3 p(0, 0);
	V3 mins = triMins(a, b, c);
	V3 maxes = triMaxes(a, b, c);
	float triArea = edgeFunction(a, b, c);
	for (p[1] = mins[1]; p[1] <= maxes[1]; p[1]++) {
		for (p[0] = mins[0]; p[0] <= maxes[0]; p[0]++) {
			V3 efs = edgeFunctions(a, b, c, p);
			if (efs[0] >= 0 && efs[1] >= 0 && efs[2] >= 0) {
				V3 w = efs / triArea;
				float depth = 1.0f / (w[0] * a[2] + w[1] * b[2] + w[2] * c[2]);
				cubeMap[face].isCloser(p[0], p[1], depth);
			}
		}
	}
} // update the z buffer of the given face

bool PointLight::inShadow(V3 p) {
	int f = selectCam(p);
	V3 pp;
	cubeMap[f].project(p, pp);
	float z = 1.0f / pp[2];
	return cubeMap[f].isFarther(pp[0], pp[1], (z + ep));
}

void PointLight::clipAndProjectTris(M33 tvs) {
	int faceA = selectCam(tvs[0]);
	int faceB = selectCam(tvs[1]);
	int faceC = selectCam(tvs[2]);
	
	if (faceA == faceB && faceB == faceC) {
		updateZB(tvs, faceA);
	}
	else { // if tri is projected onto different faces of cubeMap, need to clip tris
		// TODO: DOES NOT WORK AS INTENDED
		vector<int> faces = { faceA, faceB, faceC };
		vector<M33> tris = clipTrisToFaces(tvs, faces);
		for (M33 tri : tris) {
			int f = selectCam(tri[0]);
			updateZB(tvs, f);
		}
	}
	
} // check if tri verts project to same cube face

vector<M33> PointLight::clipTrisToFaces(M33 tri, vector<int> faces) {
	vector<M33> tris;
	for (int f : faces) {
		vector<M33> clipped = clipTriToFace(tri, f);
		tris.insert(tris.end(), clipped.begin(), clipped.end());
	}
	return tris;
}

vector<M33> PointLight::clipTriToFace(M33 tri, int face) {
	vector<V3> newTri = { tri[0], tri[1], tri[2] };
	vector<pair<V3, V3>> planes = getClippingPlanes(face);

	for (const auto& plane : planes) {
		V3 pNorm = plane.first;
		V3 pPoint = plane.second;
		newTri = clipToPlane(newTri, pNorm, pPoint);
		
		if (newTri.size() < 3) return {};
	}
	if (newTri.size() == 3) {
		return { M33(newTri[0], newTri[1], newTri[2]) };
	} else if (newTri.size() == 4) {
		return { M33(newTri[0], newTri[1], newTri[2]),
					M33(newTri[0], newTri[2], newTri[3]) };
	}
	return {};
}

vector<V3> PointLight::clipToPlane(vector<V3> tri, V3 planeNorm, V3 planePoint) {
	vector<V3> clipVerts;
	for (int i = 0; i < 3; i++) {
		V3 v0 = tri[i];
		V3 v1 = tri[(i + 1) % 3];
		float d0 = (v0 - planePoint) * planeNorm;
		float d1 = (v1 - planePoint) * planeNorm;
		if (d0 >= 0) clipVerts.push_back(v0);
		if (d0 * d1 < 0) {
			float t = d0 / (d0 - d1);
			V3 intersect = v0 + (v1 - v0) * t;
			clipVerts.push_back(intersect);
		}
	}
	
	return clipVerts;
}

vector<pair<V3, V3>> PointLight::getClippingPlanes(int f) {
	vector<pair<V3, V3>> clipPlanes;
	switch (f) {
	case 0: // +x
		clipPlanes.push_back(make_pair(V3(-1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, -1.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, 1.0f, 0.0f), lp));
		break;
	case 1: // +y
		clipPlanes.push_back(make_pair(V3(-1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, 0.0f, -1.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, 0.0f, 1.0f), lp));
		break;
	case 2: // +z
		clipPlanes.push_back(make_pair(V3(-1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, -1.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, 1.0f, 0.0f), lp));
		break;
	case 3: // -x
		clipPlanes.push_back(make_pair(V3(1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(-1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, -1.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, 1.0f, 0.0f), lp));
		break;
	case 4: // -y
		clipPlanes.push_back(make_pair(V3(-1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, 0.0f, 1.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, 0.0f, -1.0f), lp));
		break;
	case 5: // -z
		clipPlanes.push_back(make_pair(V3(1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(-1.0f, 0.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, -1.0f, 0.0f), lp));
		clipPlanes.push_back(make_pair(V3(0.0f, 1.0f, 0.0f), lp));
		break;
	default: // invalid
		break;
	}
	return clipPlanes;
}
