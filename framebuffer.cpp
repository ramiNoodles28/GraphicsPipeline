
#include "framebuffer.h"
#include "math.h"
#include <iostream>
//#include "scene.h"

#include <tiffio.h>

using namespace std;

FrameBuffer::FrameBuffer(int u0, int v0, int _w, int _h) : 
	Fl_Gl_Window(u0, v0, _w, _h, 0) {
	w = _w;
	h = _h;
	pix = new unsigned int[w*h];
	zb = new float[w * h];
	cam = NULL;
	s = 1;
	lightType = 0;
	lv = V3(0, 0, 1);
	lp = V3(0, 0, -110);
}

void FrameBuffer::addCam(PPC *c) {
	cam = c;
}

void FrameBuffer::draw() {
	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);
}

int FrameBuffer::handle(int event) {
	switch (event)
	{
	case FL_KEYBOARD: {
		KeyboardHandle();
		return 0;
	}
	case FL_MOVE: {
		int u = Fl::event_x();
		int v = Fl::event_y();
		if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
			return 0;
		cerr << u << " " << v << "          \r";
		return 0;
	}
	case FL_MOUSEWHEEL: {
		int scroll = Fl::event_dy();
		if (scroll < 0) cam->zoom(1.1);
		else if (scroll > 0) cam->zoom(0.9);
		return 0;
	}
	default:
		return 0;
	}
	return 0;
}

void FrameBuffer::KeyboardHandle() {
	int key = Fl::event_key();
	switch (key) {
	case 'w':  
	case 'W': {
		cam->translate(cam->getViewDirection() * s);
		break;
	}
	case 'a':  
	case 'A': {
		cam->translate(cam->a.normalize() * -s);
		break;
	}
	case 's':  
	case 'S': {
		cam->translate(cam->getViewDirection() * -s);
		break;
	}
	case 'd':
	case 'D': {
		cam->translate(cam->a.normalize() * s);
		break;
	}
	case 'q':  
	case 'Q': {
		cam->translate(V3(0, -s, 0));
		break;
	}
	case 'e':
	case 'E': {
		cam->translate(V3(0, s, 0));
		break;
	}
	case 'z':
	case 'Z': {
		cam->roll(s);
		break;
	}
	case 'x': 
	case 'X': {
		cam->roll(-s);
		break;
	}
	case FL_Left: {
		cam->pan(s);
		break;
	}
	case FL_Right: {
		cam->pan(-s);
		break;
	}
	case FL_Up: {
		cam->tilt(s);
		break;
	}
	case FL_Down: {
		cam->tilt(-s);
		break;
	}
	case FL_Enter: {
		cam->saveToTxt("camPaths.txt");
		cerr << "INFO: pressed enter key : " << endl;
		break;
	}
	case 'i':
	case 'I': {
		lp = lp + V3(0,0,-s);
		break;
	}
	case 'j':
	case 'J': {
		lp = lp + V3(-s, 0, 0);
		break;
	}
	case 'k':
	case 'K': {
		lp = lp + V3(0, 0, s);
		break;
	}
	case 'l':
	case 'L': {
		lp = lp + V3(s, 0, 0);
		break;
	}
	case 'u':
	case 'U': {
		lp = lp + V3(0, s, 0);
		break;
	}
	case 'o':
	case 'O': {
		lp = lp + V3(0, -s, 0);
		break;
	}
	default:
		//cerr << "INFO: do not understand keypress : " << cam->C << endl;
		return;
	}
}

void FrameBuffer::loadTiff(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}
	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		w = width;
		h = height;
		delete[] pix;
		pix = new unsigned int[w*h];
		size(w, h);
		glFlush();
		glFlush();
	}
	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}
	TIFFClose(in);
} // load a tiff image to pixel buffer

void FrameBuffer::saveAsTiff(char *fname) {
	TIFF* out = TIFFOpen(fname, "w");
	if (out == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	for (uint32 row = 0; row < (unsigned int)h; row++) {
		TIFFWriteScanline(out, &pix[(h - row - 1) * w], row);
	}
	TIFFClose(out);
} // save as tiff image

void FrameBuffer::set(unsigned int bgr) {
	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			pix[(h - 1 - v)*w + u] = bgr;
		}
	}
}

void FrameBuffer::setGuarded(int u, int v, unsigned int col) {
	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return;
	set(u, v, col);
}

void FrameBuffer::set(int u, int v, unsigned int col) {
	pix[(h - 1 - v)*w + u] = col;
}

void FrameBuffer::rasterizeRectangle(int u0, int v0, int rw, int rh,unsigned int col) {
	if (!clipRectangle(u0, v0, rw, rh))
		return;
	for (int v = v0; v < v0 + rh; v++) {
		for (int u = u0; u < u0 + rw; u++) {
			set(u, v, col);
		}
	}
}

int FrameBuffer::clipRectangle(int& u0, int& v0, int& rw, int& rh) {
	if (u0 > w - 1 || u0 + rw - 1 < 0 || 
		v0 > h - 1 || v0 + rh - 1 < 0)
		return 0;
	if (u0 < 0) {
		rw = rw - (-u0);
		u0 = 0;
	}
	if (v0 < 0) {
		rh = rh - (-v0);
		v0 = 0;
	}
	if (u0 + rw - 1 > w - 1) {
		rw = w - u0;
	}
	if (v0 + rh - 1 > h - 1) {
		rh = h - v0;
	}
	return 1;
} // return 0 if entire rectangle is off-screen

void FrameBuffer::rasterizeCircle(V3 center, float radius, unsigned int color) {
	int u0, v0, rw, rh;
	u0 = (int)(center[0] - radius + 0.5f);
	v0 = (int)(center[1] - radius + 0.5f);
	rh = rw = (int)(2.0f * radius - 0.5f);
	if (!clipRectangle(u0, v0, rw, rh))
		return;
	center[2] = 0.0f;
	float r2 = radius * radius;
	for (int v = v0; v <= v0 + rh; v++) {
		for (int u = u0; u <= u0 + rw; u++) {
			V3 pixCenter(.5f + (float)u, .5f + (float)v, 0.0f);
			float d2 = (pixCenter - center)*(pixCenter - center);
			if (d2 > r2)
				continue;
			setGuarded(u, v, color);
		}
	}
}

void FrameBuffer::renderWF(TM tm, PPC* ppc) {
	if (!tm.onFlag)
		return;

	// go over all triangles
	for (int tri = 0; tri < tm.trisN; tri++) {
		V3 tvs[3];
		V3 pvs[3];
		unsigned int vinds[3];
		for (int vi = 0; vi < 3; vi++) {
			vinds[vi] = tm.tris[3 * tri + vi];
			tvs[vi] = tm.verts[vinds[vi]];
			ppc->project(tvs[vi], pvs[vi]);
		}
		for (int ei = 0; ei < 3; ei++) {
			rasterize2DSegment(pvs[ei], pvs[(ei + 1) % 3], 
				tm.colors[vinds[ei]], tm.colors[vinds[(ei + 1) % 3]]);
		}
	}
} // renders wireframe of mesh

void FrameBuffer::rasterize2DSegment(V3 p0, V3 p1, V3 c0, V3 c1) {
  // find the largest span (horizontal or vertical)
	float hspan = fabsf(p0[0] - p1[0]);
	float vspan = fabsf(p0[1] - p1[1]);
	int stepsN = (hspan < vspan) ? (int)(vspan) : (int)hspan;
	stepsN += 1;
	// that's how many steps n on the segment
  // walk on segment and take n steps
	for (int stepi = 0; stepi <= stepsN; stepi++) {
		// for each step, set current pixel
		float t = (float)stepi / (float)stepsN;
		V3 p = p0 + (p1 - p0)*t;
		V3 cc = c0 + (c1 - c0)*t;
		int u = (int)p[0];
		int v = (int)p[1];
		if (isFarther(u, v, p[2])) continue;
		setZB(u, v, p[2]);
		setGuarded(u, v, cc.getColor());
	}
}

void FrameBuffer::rasterizeTriLines(V3 p0, V3 p1, V3 p2, unsigned int color) {
	rasterize2DSegment(p0, p1, color);
	rasterize2DSegment(p1, p2, color);
	rasterize2DSegment(p2, p0, color);
}

void FrameBuffer::rasterizeTris(V3 a, V3 b, V3 c, unsigned int color) {
	V3 cl(0, 0, 0);
	cl.setFromColor(color);
	rasterizeTris(a, b, c, M33(cl, cl, cl));
}

void FrameBuffer::renderTris(TM tm, PPC* ppc) {
	if (!tm.onFlag)
		return;
	// go over all triangles
	for (int tri = 0; tri < tm.trisN; tri++) {
		V3 tvs[3];
		V3 pvs[3];
		unsigned int vinds[3];
		for (int vi = 0; vi < 3; vi++) {
			vinds[vi] = tm.tris[3 * tri + vi];
			tvs[vi] = tm.verts[vinds[vi]];
			ppc->project(tvs[vi], pvs[vi]);
		}
		rasterizeTris(pvs[0], pvs[1], pvs[2],
			M33(tm.colors[vinds[0]], tm.colors[vinds[1]], tm.colors[vinds[2]]));
	}
} // renders filled in tri mesh

void FrameBuffer::rasterizeTris(V3 a, V3 b, V3 c, M33 colors) {
	V3 p(0, 0);
	V3 mins = triMins(a, b, c);
	V3 maxes = triMaxes(a, b, c);
	float triArea = edgeFunction(a, b, c);
	for (p[1] = mins[1]; p[1] <= maxes[1]; p[1]++) {
		for (p[0] = mins[0]; p[0] <= maxes[0]; p[0]++) {
			if (!inBounds(p)) continue;
			V3 efs = edgeFunctions(a, b, c, p);
			if (efs[0] >= 0 && efs[1] >= 0 && efs[2] >= 0) {
				V3 w = efs / triArea;
				float depth = 1.0f / (w[0] * a[2] + w[1] * b[2] + w[2] * c[2]);
				if (!isCloser(p[0], p[1], depth)) continue;
				V3 color = colors ^ w;
				setGuarded(p[0], p[1], color.getColor());
			}
		}
	}
}

void FrameBuffer::renderTrisDirLight(TM tm, PPC* ppc, V3 lv, float ka) {
	if (!tm.onFlag)
		return;
	// go over all triangles
	for (int tri = 0; tri < tm.trisN; tri++) {
		V3 tvs[3];
		V3 pvs[3];
		unsigned int vinds[3];
		for (int vi = 0; vi < 3; vi++) {
			vinds[vi] = tm.tris[3 * tri + vi];
			tvs[vi] = tm.verts[vinds[vi]];
			ppc->project(tvs[vi], pvs[vi]);
		}
		rasterizeTrisDirLight(pvs[0], pvs[1], pvs[2],
			M33(tm.colors[vinds[0]], tm.colors[vinds[1]], tm.colors[vinds[2]]),
			M33(tm.normals[vinds[0]], tm.normals[vinds[1]], tm.normals[vinds[2]]), lv, ka);
	}
} // render mesh with directional light

void FrameBuffer::rasterizeTrisDirLight(V3 a, V3 b, V3 c, M33 color, M33 norms, V3 lv, float ka) {
	V3 p(0, 0);
	V3 mins = triMins(a, b, c);
	V3 maxes = triMaxes(a, b, c);
	float triArea = edgeFunction(a, b, c);
	for (p[1] = mins[1]; p[1] <= maxes[1]; p[1]++) {
		for (p[0] = mins[0]; p[0] <= maxes[0]; p[0]++) {
			if (!inBounds(p)) continue;
			V3 efs = edgeFunctions(a, b, c, p);
			if (efs[0] >= 0 && efs[1] >= 0 && efs[2] >= 0) {
				V3 w = efs / triArea;
				float depth = 1.0f / (w[0] * a[2] + w[1] * b[2] + w[2] * c[2]);
				if (!isCloser(p[0], p[1], depth)) continue;
				V3 pixelNormal = (norms ^ w).normalize();
				V3 baseColor = (color ^ w);
				V3 pixelColor = baseColor.lightColor(lv, ka, pixelNormal);
				setGuarded(p[0], p[1], pixelColor.getColor());
			}
		}
	}
}

void FrameBuffer::renderTrisPointLight(TM tm, PPC* ppc, PointLight pl) {
	if (!tm.onFlag)
		return;
	// go over all triangles
	V3 plp;
	ppc->project(pl.lp, plp);
	for (int tri = 0; tri < tm.trisN; tri++) {
		V3 tvs[3];
		V3 pvs[3];
		unsigned int vinds[3];
		for (int vi = 0; vi < 3; vi++) {
			vinds[vi] = tm.tris[3 * tri + vi];
			tvs[vi] = tm.verts[vinds[vi]];
			ppc->project(tvs[vi], pvs[vi]);
		}
		rasterizeTrisPointLight(pvs[0], pvs[1], pvs[2],
			M33(tvs[0], tvs[1], tvs[2]),
			M33(tm.colors[vinds[0]], tm.colors[vinds[1]], tm.colors[vinds[2]]),
			M33(tm.normals[vinds[0]], tm.normals[vinds[1]], tm.normals[vinds[2]]), pl);
	}
} // render mesh with point light

void FrameBuffer::rasterizeTrisPointLight(V3 a, V3 b, V3 c, M33 verts, M33 color, M33 norms, PointLight pl) {
	V3 p(0, 0);
	V3 mins = triMins(a, b, c);
	V3 maxes = triMaxes(a, b, c);
	float triArea = edgeFunction(a, b, c);
	for (p[1] = mins[1]; p[1] <= maxes[1]; p[1]++) {
		for (p[0] = mins[0]; p[0] <= maxes[0]; p[0]++) {
			if (!inBounds(p)) continue;
			V3 efs = edgeFunctions(a, b, c, p);
			if (efs[0] >= 0 && efs[1] >= 0 && efs[2] >= 0) {
				V3 w = efs / triArea;
				float depth = 1.0f / (w[0] * a[2] + w[1] * b[2] + w[2] * c[2]);
				if (!isCloser(p[0], p[1], depth)) continue;
				V3 pixelNormal = (norms ^ w).normalize();
				V3 baseColor = (color ^ w);
				V3 pixelPos = (verts ^ w);
				V3 lv = (pl.lp - pixelPos).normalize();
				float dist = (pl.lp - pixelPos).length();
				float atten = 1.0; /// (0.005 * dist * dist);
				float diffuse = max(0.0f, pixelNormal * lv);
				V3 pixelColor = baseColor.lightColor(lv, pl.ka, pixelNormal);
				pixelColor = baseColor * diffuse * atten + (baseColor * pl.ka);
				setGuarded(p[0], p[1], pixelColor.getColor());
			}
		}
	}
}

V3 FrameBuffer::triMins(V3 a, V3 b, V3 c) {
	return V3(floor(min(a[0], min(b[0], c[0]))),
		floor(min(a[1], min(b[1], c[1]))));
}

V3 FrameBuffer::triMaxes(V3 a, V3 b, V3 c) {
	return V3(ceil(max(a[0], max(b[0], c[0]))),
		ceil(max(a[1], max(b[1], c[1]))));
}

float FrameBuffer::edgeFunction(V3 a, V3 b, V3 p) {
	return p[0] * (b[1] - a[1]) - p[1] * (b[0] - a[0]) - (a[0] * b[1]) + (a[1] * b[0]);
} // returns if point is on right side of edge

V3 FrameBuffer::edgeFunctions(V3 a, V3 b, V3 c, V3 p) {
	V3 efs;
	efs[2] = p[0] * (b[1] - a[1]) - p[1] * (b[0] - a[0]) - (a[0] * b[1]) + (a[1] * b[0]);
	efs[0] = p[0] * (c[1] - b[1]) - p[1] * (c[0] - b[0]) - (b[0] * c[1]) + (b[1] * c[0]);
	efs[1] = p[0] * (a[1] - c[1]) - p[1] * (a[0] - c[0]) - (c[0] * a[1]) + (c[1] * a[0]);
	return efs;
} // returns if point is on right side of edge for multiple edges

int FrameBuffer::isCCW(V3 a, V3 b, V3 c) {
	return ((b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])) > 0;
} // Calculate the signed area using the cross product

int FrameBuffer::inBounds(V3 p) {
	return !(p[0] < 0 || p[0] > w - 1 || p[1] < 0 || p[1] > h - 1);
}

void FrameBuffer::rasterize2DSegment(V3 p0, V3 p1, unsigned int color) {
	V3 c;
	c.setFromColor(color);
	rasterize2DSegment(p0, p1, c, c);
}

void FrameBuffer::render3DSegment(V3 p0, V3 p1, V3 c0, V3 c1, PPC *ppc) {
	V3 pp0;
	if (!ppc->project(p0, pp0))
		return;
	V3 pp1;
	if (!ppc->project(p1, pp1))
		return;
	rasterize2DSegment(pp0, pp1, c0, c1);
	return;
}

void FrameBuffer::renderPoint(V3 p, float r, V3 c, PPC *ppc) {
	V3 pp;
	if (!ppc->project(p, pp) || !inBounds(pp)) return;
	float z = 1 / pp[2];
	if (isFarther(pp[0], pp[1], z)) return;
	setZB(pp[0], pp[1], z);
	rasterizeCircle(pp, r, c.getColor());
}

void FrameBuffer::clearZB() {
	for (int uv = 0; uv < w * h; uv++)
		zb[uv] = 0.0f;
} // clear the z buffer

int FrameBuffer::isFarther(int u, int v, float z) {
	if (getZB(u, v) > z)
		return 1;
	return 0;
} // check if current pixel z is farther than z buffer

int FrameBuffer::isCloser(int u, int v, float z) {
	if (getZB(u, v) < z) {
		setZB(u, v, z);
		return 1;
	}
	return 0;
} // check and set if current pixel z is closer than z buffer

float FrameBuffer::getZB(int u, int v) {
	return zb[(h - 1 - v) * w + u];
} // get z buffer from pixel coordinate

void FrameBuffer::setZB(int u, int v, float z) {
	zb[(h - 1 - v) * w + u] = z;
} // set z buffer at pixel coordinate

