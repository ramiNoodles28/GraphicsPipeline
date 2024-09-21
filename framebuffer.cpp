
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
	pix = new unsigned int[w*h];
	cam = NULL;
	s = 1;
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
	case 'w':  // ASCII value for 'w'
	case 'W': {
		//cerr << "INFO: pressed W (move forward) : " << cam->C << endl;
		cam->translate(cam->getViewDirection() * s);
		break;
	}
	case 'a':  // ASCII value for 'a'
	case 'A': {
		//cerr << "INFO: pressed A (move left) : " << cam->C << endl;
		cam->translate(cam->a.normalize() * -s);
		break;
	}
	case 's':  // ASCII value for 's'
	case 'S': {
		//cerr << "INFO: pressed S (move backwards) : " << cam->C << endl;
		cam->translate(cam->getViewDirection() * -s);
		break;
	}
	case 'd':  // ASCII value for 'd'
	case 'D': {
		//cerr << "INFO: pressed D (move right) : " << cam->C << endl;
		cam->translate(cam->a.normalize() * s);
		break;
	}
	case 'q':  // ASCII value for 'q'
	case 'Q': {
		//cerr << "INFO: pressed Q (move down) : " << cam->C << endl;
		cam->translate(V3(0, -s, 0));
		break;
	}
	case 'e':  // ASCII value for 'd'
	case 'E': {
		//cerr << "INFO: pressed E (move up) : " << cam->C << endl;
		cam->translate(V3(0, s, 0));
		break;
	}
	case 'z':  // ASCII value for 'z'
	case 'Z': {
		//cerr << "INFO: pressed Z (roll+) : " << cam->C << endl;
		cam->roll(s);
		break;
	}
	case 'x':  // ASCII value for 'd'
	case 'X': {
		//cerr << "INFO: pressed X (roll-) : " << cam->C << endl;
		cam->roll(-s);
		break;
	}
	case FL_Left: {
		//cerr << "INFO: pressed left arrow key : " << cam->C << endl;
		cam->pan(s);
		break;
	}
	case FL_Right: {
		//cerr << "INFO: pressed right arrow key : " << cam->C << endl;
		cam->pan(-s);
		break;
	}
	case FL_Up: {
		//cerr << "INFO: pressed up arrow key : " << cam->C << endl;
		cam->tilt(s);
		break;
	}
	case FL_Down: {
		//cerr << "INFO: pressed down arrow key : " << cam->C << endl;
		cam->tilt(-s);
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


void FrameBuffer::rasterizeRectangle(int u0, int v0, int rw, int rh,
	unsigned int col) {
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
} // return 0 if entire rectangle is off screen

void FrameBuffer::rasterizeCircle(V3 center, float radius,
	unsigned int color) {
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
			set(u, v, color);
		}
	}
}


// p = p0 + (p1-p0)*t
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
		setGuarded(u, v, cc.getColor());
	}
}

void FrameBuffer::rasterizeTriLines(V3 p0, V3 p1, V3 p2, unsigned int color) {
	rasterize2DSegment(p0, p1, color);
	rasterize2DSegment(p1, p2, color);
	rasterize2DSegment(p2, p0, color);
}

void FrameBuffer::rasterizeTris(V3 a, V3 b, V3 c, unsigned int color) {
	V3 p(0, 0);
	V3 mins(min(a[0], min(b[0], c[0])), min(a[1], min(b[1], c[1])));
	V3 maxes(max(a[0], max(b[0], c[0])), max(a[1], max(b[1], c[1])));
	for (p[1] = mins[1]; p[1] < maxes[1]; p[1]++) {
		for (p[0] = mins[0]; p[0] < maxes[0]; p[0]++) {
			float e0 = edgeFunction(a, b, p);
			float e1 = edgeFunction(b, c, p);
			float e2 = edgeFunction(c, a, p);

			if (e0 >= 0 && e1 >= 0 && e2 >= 0) setGuarded(p[0], p[1], color);
		}
	}
}

float FrameBuffer::edgeFunction(V3 a, V3 b, V3 p) {
	return p[0] * (b[1] - a[1]) - p[1] * (b[0] - a[0]) - (a[0] * b[1]) + (a[1] * b[0]);
} // returns if point is on right side of edge


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
