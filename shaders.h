#pragma once

#include "V3.h"
#include "PPC.h"
#include "M33.h"

class Shaders {
public:
	V3 simpleColors(V3 w, M33 colors);
	V3 directionalLighiting(V3 w, M33 colors, M33 norms, V3 lv, float ka);
	V3 pointLighting(V3 w, M33 verts, M33 color, M33 norms, V3 lp, float ka);
};