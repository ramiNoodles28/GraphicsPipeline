#include "V3.h"
#include "M33.h"
#include <ostream>
#include <istream>
#include <cmath>
#include <iostream>
#define M_PI 3.1415926535897932384626433832
using namespace std;

V3::V3(float x, float y, float z) {
	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = z;
} // Constructor with parameters

V3::V3(float x, float y) {
	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = 0;
} // Constructor 2D Vector

float& V3::operator[](int i) {
	return xyz[i];
} // Access vector element

V3 V3::operator+(V3 v) {
	V3 ret;
	V3& u = *this;
	ret[0] = u[0] + v[0];
	ret[1] = u[1] + v[1];
	ret[2] = u[2] + v[2];
	return ret;
} // Vector addition

V3 V3::operator-(V3 v) {
	V3 ret;
	V3& u = *this;
	ret[0] = u[0] - v[0];
	ret[1] = u[1] - v[1];
	ret[2] = u[2] - v[2];
	return ret;
} // Vector subtraction

V3 V3::operator*(float i) {
	V3 ret;
	V3& u = *this;
	ret[0] = u[0] * i;
	ret[1] = u[1] * i;
	ret[2] = u[2] * i;
	return ret;
} // Scalar multiplication

V3 V3::operator/(float i) {
	V3 ret;
	V3& u = *this;
	ret[0] = u[0] / i;
	ret[1] = u[1] / i;
	ret[2] = u[2] / i;
	return ret;
} // Scalar division

float V3::operator*(V3 v) {
	V3& u = *this;
	float ret = u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
	return ret;
} // Dot product

V3 V3::operator^(V3 v) {
	V3 ret;
	V3& u = *this;
	ret[0] = (u[1] * v[2]) - (u[2] * v[1]);
	ret[1] = (u[2] * v[0]) - (u[0] * v[2]);
	ret[2] = (u[0] * v[1]) - (u[1] * v[0]);
	return ret;
} // Cross product

ostream& operator<<(ostream& ostr, V3 v) {
	V3 u = v.zeroFix();
	// V3 u = v;
	return ostr << u[0] << " " << u[1] << " " << u[2] << endl;
} // Stream output

istream& operator>>(istream& istr, V3& v) {
	return istr >> v[0] >> v[1] >> v[2];
} // Stream input

V3 V3::zeroFix() {
	V3& v = *this;
	V3 u;
	u[0] = (v[0] < 0.001f) ? 0 : v[0];
	u[1] = (v[1] < 0.001f) ? 0 : v[1];
	u[2] = (v[2] < 0.001f) ? 0 : v[2];
	return u;
} // Round values that are very small to zero (for cleanliness' sake)

float V3::length() {
	V3& v = *this;
	float ret = sqrtf(v * v);
	return ret;
} // Vector length

V3 V3::normalize() {
	V3& u = *this;
	V3 ret = u / u.length();
	return ret;
} // Normalize Vector

V3 V3::rotateAboutAxis(V3 aO, V3 aD, float theta) {
	// build coordinate system with aO as origin and aD as one axis
	M33 cs;
	cs[1] = aD.normalize();
	V3 aux(1.0f, 0.0f, 0.0f);
	if (fabsf(aD[0]) > fabsf(aD[1]))
		aux = V3(0.0f, 1.0f, 0.0f);
	cs[2] = (aD ^ aux).normalize();
	cs[0] = cs[1] ^ cs[2];

	V3 &P = *this;
	// convert to cs
	V3 Pl = cs*(P - aO);

	// rotate in cs
	M33 roty; roty.SetRotY(theta);
	V3 PlR = roty * Pl;

	M33 cs_1; cs_1 = cs.invert();
	// come back to world
	V3 ret = cs_1 * PlR + aO;
	return ret;
} // Rotate vector (point) about axis

void V3::setFromColor(unsigned int color) {
	unsigned char* rgb = (unsigned char*) &color;
	V3 &v = *this;
	v[0] = ((float)rgb[0]) / 255.0f;
	v[1] = ((float)rgb[1]) / 255.0f;
	v[2] = ((float)rgb[2]) / 255.0f;
} // Set V3 from hex color (0xAABBGGRR) to RGB channels

unsigned int V3::getColor() {
	unsigned int ret;
	unsigned int rgb[3];
	V3 &v = *this;
	for (int ci = 0; ci < 3; ci++) {
		float fc = v[ci];
		if (fc < 0.0f)
			fc = 0.0f;
		if (fc > 1.0f)
			fc = 1.0f;
		rgb[ci] = (unsigned int) (255.0f * fc);
	}
	ret = 0xFF000000 + rgb[2] * 256 * 256 + rgb[1] * 256 + rgb[0];
	return ret;
} // Get hex color (0xAABBGGRR) from RGB channels
