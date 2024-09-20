#pragma once

#include <ostream>
#include <istream>

using namespace std;

#include "V3.h"

class M33 {
public:
    V3 rows[3];
    M33() {}; // Empty constructor
    M33(V3 v0, V3 v1, V3 v2);
    M33(V3 axis, float angle); // Set matrix as rotation about {x|y|z} axis and angle
    V3& operator[](int i); // Access row vector

    M33 operator*(float s); // Matrix times a scalar
    V3 operator*(V3 v); // Matrix times col vector
    M33 operator*(M33 m); // Matrix multiplication
    float determinant(); // Get matrix determinant
    M33 transpose(); // Transpose matrix
    M33 invert(); // Invert matrix
    void SetRotY(float theta);

    V3 getCol(int i); // Get column i
    void setCol(int i, V3 v); // Set column i to vector

    friend ostream& operator<<(ostream& ostr, M33 m); // Stream output
    friend istream& operator>>(istream& istr, M33& m); // Stream input
};
