
#include "framebuffer.h"
#include "math.h"
#include <iostream>
#include "scene.h"

#include <tiffio.h>

using namespace std;

FrameBuffer::FrameBuffer(int u0, int v0, int _w, int _h) : 
	Fl_Gl_Window(u0, v0, _w, _h, 0) {
	w = _w;
	h = _h;
	isHW = 0;
	pix = new unsigned int[w*h];
	zb = new float[w * h];
	cam = NULL;
	s = 1;
	lp = V3(0, 0, 0);
}

void FrameBuffer::addCam(PPC *c) {
	cam = c;
}

void FrameBuffer::draw() {
	if (!isHW) glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);
	else {
		scene->RenderHW();
	}
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

unsigned int FrameBuffer::get(int u, int v) {
	return pix[(h - 1 - v) * w + u];
}

//// Rasterization Functions

V3 FrameBuffer::perspectiveInterp(M33 m, V3 inv, V3 w) {
	V3 vA = m.getCol(0) * inv[0];
	V3 vB = m.getCol(1) * inv[1];
	V3 vC = m.getCol(2) * inv[2];
	return (vA * w[0] + vB * w[1] + vC * w[2]) / (w * inv);
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
				tm.colors[vinds[ei]], tm.colors[vinds[(ei + 1) % 3]], ppc);
		}
	}
} // renders wireframe of mesh

void FrameBuffer::rasterize2DSegment(V3 p0, V3 p1, V3 c0, V3 c1, PPC* ppc) {
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
		setGuarded(u, v, cc.getColor());
	}
}

void FrameBuffer::rasterizeTriLines(V3 p0, V3 p1, V3 p2, unsigned int color, PPC* ppc) {
	rasterize2DSegment(p0, p1, color, ppc);
	rasterize2DSegment(p1, p2, color, ppc);
	rasterize2DSegment(p2, p0, color, ppc);
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
			M33(tm.colors[vinds[0]], tm.colors[vinds[1]], tm.colors[vinds[2]]), ppc);
	}
} // renders filled in tri mesh

void FrameBuffer::rasterizeTris(V3 a, V3 b, V3 c, M33 colors, PPC* ppc) {
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
				if (!ppc->isCloser(p[0], p[1], depth)) continue;
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
			M33(tm.normals[vinds[0]], tm.normals[vinds[1]], tm.normals[vinds[2]]), lv, ka, ppc);
	}
} // render mesh with directional light

void FrameBuffer::rasterizeTrisDirLight(V3 a, V3 b, V3 c, M33 color, M33 norms, V3 lv, float ka, PPC* ppc) {
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
				if (!ppc->isCloser(p[0], p[1], depth)) continue;
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
		if (tm.texFlag)
			rasterizeTrisPointLight(pvs[0], pvs[1], pvs[2],
				M33(tvs[0], tvs[1], tvs[2]),
				M33(tm.colors[vinds[0]], tm.colors[vinds[1]], tm.colors[vinds[2]]),
				M33(tm.normals[vinds[0]], tm.normals[vinds[1]], tm.normals[vinds[2]]), 
				ppc, pl, 
				M33(tm.texCoords[vinds[0]], tm.texCoords[vinds[1]], tm.texCoords[vinds[2]]), tm.tex);
		else
			rasterizeTrisPointLight(pvs[0], pvs[1], pvs[2],
				M33(tvs[0], tvs[1], tvs[2]),
				M33(tm.colors[vinds[0]], tm.colors[vinds[1]], tm.colors[vinds[2]]),
				M33(tm.normals[vinds[0]], tm.normals[vinds[1]], tm.normals[vinds[2]]),
				ppc, pl);
	}
} // render mesh with point light

void FrameBuffer::rasterizeTrisPointLight(V3 a, V3 b, V3 c, 
			M33 verts, M33 color, M33 norms, PPC* ppc, PointLight pl, M33 texCoords, Texture *tex) {
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
				V3 inv = V3(1.0f / a[2], 1.0f / b[2], 1.0f / c[2]);
				float invDepths = w * inv;
				float depth = invDepths;
				if (!ppc->isCloser(p[0], p[1], depth)) continue;
				V3 baseColor;
				if (tex) {
					V3 pixelTexCoord = perspectiveInterp(texCoords, inv, w);
					baseColor = tex->getTex(pixelTexCoord);
				} else baseColor = color ^ w;
				V3 pixelPos = verts ^ w;
				V3 pixelNormal = norms ^ w;

				V3 pixelColor = pl.lightPixel(p, pixelPos, baseColor, pixelNormal);

				setGuarded(p[0], p[1], pixelColor.getColor());
			}
		}
	}
}

void FrameBuffer::renderTrisReflective(TM tm, PPC* ppc, EnvMap *env) {
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
		rasterizeTrisReflective(pvs[0], pvs[1], pvs[2],
			M33(tvs[0], tvs[1], tvs[2]),
			M33(tm.colors[vinds[0]], tm.colors[vinds[1]], tm.colors[vinds[2]]),
			M33(tm.normals[vinds[0]], tm.normals[vinds[1]], tm.normals[vinds[2]]),
			ppc, env);
	}
} // render mesh with point light

void FrameBuffer::rasterizeTrisReflective(V3 a, V3 b, V3 c,
	M33 verts, M33 color, M33 norms, PPC* ppc, EnvMap *env) {
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
				V3 inv = V3(1.0f / a[2], 1.0f / b[2], 1.0f / c[2]);
				float invDepths = w * inv;
				float depth = invDepths;
				if (!ppc->isCloser(p[0], p[1], depth)) continue;
				V3 baseColor = color ^ w;
				V3 pixelPos = verts ^ w;
				V3 pixelNormal = norms ^ w;

				V3 eyeRay = ppc->C - pixelPos;
				V3 refl = pixelNormal.reflectAboutThisNorm(eyeRay);
				/*
				int visStep = 50;
				if ((int)p[0] % visStep == 0 && (int)p[1] % visStep==0) {
					V3 R = pixelPos + refl.normalize() * 5.0f;
					render3DSegment(pixelPos, R, V3(0, 0, 0), V3(1, 0, 0), ppc);
					R = pixelPos + pixelNormal * 5.0f;
					render3DSegment(pixelPos, R, V3(0, 0, 0), V3(0, 0, 1), ppc);
				}
				*/
				setGuarded(p[0], p[1], env->lookup(refl));
			}
		}
	}
} // unlit per pixel rasterization of reflective surface


int FrameBuffer::inBounds(V3 p) {
	return !(p[0] < 0 || p[0] > w - 1 || p[1] < 0 || p[1] > h - 1);
}

void FrameBuffer::rasterize2DSegment(V3 p0, V3 p1, unsigned int color, PPC* ppc) {
	V3 c;
	c.setFromColor(color);
	rasterize2DSegment(p0, p1, c, c, ppc);
}

void FrameBuffer::render3DSegment(V3 p0, V3 p1, V3 c0, V3 c1, PPC *ppc) {
	V3 pp0;
	if (!ppc->project(p0, pp0))
		return;
	V3 pp1;
	if (!ppc->project(p1, pp1))
		return;
	rasterize2DSegment(pp0, pp1, c0, c1, ppc);
	return;
}

void FrameBuffer::renderPoint(V3 p, float r, V3 c, PPC *ppc) {
	V3 pp;
	if (!ppc->project(p, pp) || !inBounds(pp)) return;
	float z = 1 / pp[2];
	if (ppc->isFarther(pp[0], pp[1], z)) return;
	ppc->setZB(pp[0], pp[1], z);
	rasterizeCircle(pp, r, c.getColor());
}

void FrameBuffer::setBackgroundEnv(EnvMap* env, PPC* ppc) {
	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			V3 eyeRay = ppc->getRay(u, v);
			set(u, v, env->lookup(eyeRay));
		}
	}
	ppc->clearZB();
} // set background of scene to environment



