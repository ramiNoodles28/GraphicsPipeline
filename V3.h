
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
	V3 rotateDir(V3 dir, float angle); // Rotate vector about direction

	void setFromColor(unsigned int color); // Set V3 from hex color (0xAABBGGRR) to RGB channels
	unsigned int getColor(); // Get hex color (0xAABBGGRR) from RGB channels
};