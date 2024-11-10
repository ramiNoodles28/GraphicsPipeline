
#pragma once

#include <ostream>
#include <istream>

using namespace std;

class V3 {
public:
	float xyz[3]{};
	V3() {}; // Empty constructor
	V3(float x, float y, float z); // Constructor with parameters
	V3(float x, float y); // Construct 2D vector

	float& operator[](int i); // Access vector element
	V3 operator+(V3 v); // Vector addition
	V3 operator-(V3 v); // Vector subtraction
	V3 operator*(float i); // Scalar multiplication
	V3 operator/(float i); // Scalar division
	float operator*(V3 v); // Dot product
	V3 operator^(V3 v); // Cross product

	friend ostream& operator<<(ostream& ostr, V3 v); // Stream output
	friend istream& operator>>(istream& istr, V3& v); // Stream input
	V3 zeroFix(); // Round values that are very small to zero (for cleanliness' sake)

	float length(); // Vector length
	V3 normalize(); // Normalize Vector
	V3 rotateAboutAxis(V3 aO, V3 aD, float theta); // Rotate vector (point) about axis
	V3 rotateAboutOrigin(V3 aD, float theta); // rotate vector about axis that crosses origin
	V3 rotateDir(V3 dir, float angle); // Rotate vector about direction
	V3 reflectAboutThisNorm(V3 ray); // assumes this is a normal, and reflects input about this

	V3 triMins(V3 a, V3 b, V3 c); // gets tri min bounds
	V3 triMaxes(V3 a, V3 b, V3 c); // gets tri max bounds
	float edgeFunction(V3 a, V3 b, V3 p); // returns if point is on right side of edge
	V3 edgeFunctions(V3 a, V3 b, V3 c, V3 p); // returns if point is on right side of edge for multiple edges
	int isCCW(V3 a, V3 b, V3 c); // Calculate the signed area using the cross product

	void setFromColor(unsigned int color); // Set V3 from hex color (0xAABBGGRR) to RGB channels
	unsigned int getColor(); // Get hex color (0xAABBGGRR) from RGB channels
	V3 lightColor(V3 lv, float ka, V3 normal); // apply lighting to color vector
};