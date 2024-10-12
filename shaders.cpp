#include "shaders.h"

V3 Shaders::simpleColors(V3 w, M33 colors) {
	return colors * w;
}

V3 Shaders::directionalLighiting(V3 w, M33 colors, M33 norms, V3 lv, float ka) {
	V3 pixelNormal = (norms ^ w).normalize();
	V3 baseColor = (colors ^ w);
	return baseColor.lightColor(lv, ka, pixelNormal);
}

V3 Shaders::pointLighting(V3 w, M33 verts, M33 color, M33 norms, V3 lp, float ka) {
	V3 pixelNormal = (norms ^ w).normalize();
	V3 baseColor = (color ^ w);
	V3 pixelPos = (verts ^ w);
	V3 lv = (lp - pixelPos).normalize();
	float dist = (lp - pixelPos).length();
	float atten = 1.0 / (0.005 * dist * dist);
	float diffuse = max(0.0f, pixelNormal * lv);
	V3 pixelColor = baseColor.lightColor(lv, ka, pixelNormal);
	pixelColor = baseColor * diffuse * atten + (baseColor * ka);
	return pixelColor;
}

