#include "V3.h"
#include "M33.h"
#include <ostream>
#include <istream>
#include <cmath>
#define PI 3.1415926535897932384626433832
using namespace std;

M33::M33(V3 v0, V3 v1, V3 v2) {
    rows[0] = v0;
    rows[1] = v1;
    rows[2] = v2;
} // Constructor with vectors

M33::M33(V3 axis, float angle) {
    V3 u = axis.normalize();
    float rad = PI * angle / 180;
    float c = cos(rad);
    float s = sin(rad);
    rows[0][0] = u[0] * u[0] * (1 - c) + c;
    rows[0][1] = u[0] * u[1] * (1 - c) - u[2] * s;
    rows[0][2] = u[0] * u[2] * (1 - c) + u[1] * s;

    rows[1][0] = u[1] * u[0] * (1 - c) + u[2] * s;
    rows[1][1] = u[1] * u[1] * (1 - c) + c;
    rows[1][2] = u[1] * u[2] * (1 - c) - u[0] * s;

    rows[2][0] = u[2] * u[0] * (1 - c) - u[1] * s;
    rows[2][1] = u[2] * u[1] * (1 - c) + u[0] * s;
    rows[2][2] = u[2] * u[2] * (1 - c) + c;
} // Construct matrix as rotation about {x|y|z} (works for any arbitrary) axis and angle

V3& M33::operator[](int i) {
    return rows[i];
} // Access row vector

M33 M33::operator*(float s) {
    return M33(rows[0] * s, rows[1] * s, rows[2] * s);
} // Matrix times a scalar

V3 M33::operator*(V3 v) {
    V3 ret;
    M33& m = *this;
    ret[0] = m[0] * v;
    ret[1] = m[1] * v;
    ret[2] = m[2] * v;
    return ret;
} // Matrix times col vector

M33 M33::operator*(M33 m) {
    M33 ret;
    M33& n = *this;
    for (int col = 0; col < 3; col++) {
        V3 v = m.getCol(col);
        for (int row = 0; row < 3; row++) {
            ret[row][col] = n[row] * v;
        }
    }
    return ret;
} // Matrix multiplication

V3 M33::operator^(V3 v) {
    V3 ret;
    M33& m = *this;
    ret = (m[0] * v[0]) + (m[1] * v[1]) + (m[2] * v[2]);
    return ret;
} // Scale each matrix row by vector element then add scaled vectors together

float M33::determinant() {
    return rows[0] * (rows[1] ^ rows[2]);
} // Get matrix determinant

M33 M33::transpose() {
    M33& m = *this;
    M33 ret;
    ret[0] = m.getCol(0);
    ret[1] = m.getCol(1);
    ret[2] = m.getCol(2);
    return ret;
} // Transpose matrix

M33 M33::invert() {

	M33 ret;
	V3 a = getCol(0), b = getCol(1), c = getCol(2);
	V3 _a = b ^ c; _a = _a * (1.0f / (a * _a));
	V3 _b = c ^ a; _b = _b * (1.0f / (b * _b));
	V3 _c = a ^ b; _c = _c * (1.0f / (c * _c));
	ret[0] = _a;
	ret[1] = _b;
	ret[2] = _c;

	return ret;

}

void M33::SetRotY(float theta) {

	float thetaRad = 3.1415926f / 180.0f*theta;
	M33 &m = *this;
	m[0] = V3(cosf(thetaRad), 0.0f, sinf(thetaRad));
	m[1] = V3(0.0f, 1.0f, 0.0f);
	m[2] = V3(-sinf(thetaRad), 0.0f, cosf(thetaRad));

}

V3 M33::getCol(int i) {
    return V3(rows[0][i], rows[1][i], rows[2][i]);
} // Get column i

void M33::setCol(int i, V3 v) {
    rows[0][i] = v[0];
    rows[1][i] = v[1];
    rows[2][i] = v[2];
} // Set column i to vector

ostream& operator<<(ostream& ostr, M33 m) {
    return ostr << m[0] << m[1] << m[2];
} // Stream output

istream& operator>>(istream& istr, M33& m) {
    return istr >> m[0] >> m[1] >> m[2];
} // Stream input