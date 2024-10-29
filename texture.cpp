#include "texture.h"

#include <iostream>
#include <tiffio.h>
//#include <GL/gl.h>
using namespace std;

Texture::Texture(int _w, int _h) {
	w = _w;
	h = _h;
	pix = new unsigned int[w * h];
	mirrorTile = 0;
}

unsigned int Texture::get(int u, int v) {
	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return pix[0];
	return pix[(h - 1 - v) * w + u];
}

void Texture::set(int u, int v, V3 color) {
	pix[(h - 1 - v) * w + u] = color.getColor();
}

V3 Texture::getTex(V3 texCoord) {
	// TODO get mirroring to work with tiling
	float s = texCoord[0];
	float sT = (float)((int)texCoord[0]);
	float t = texCoord[1];
	float tT = (float)((int)texCoord[1]);
	s -= sT;
	t -= tT;
	if (mirrorTile) {
		s = (int(sT) % 2 == 0) ? s : 1.f - (s);
		t = (int(tT) % 2 == 0) ? t : 1.f - (t);
	}
	int tu = (int)(s * (float)w);
	int tv = (int)(t * (float)h);
	V3 ret;
	ret.setFromColor(get(tu, tv));
	return ret;
}

Texture Texture::getRange(V3 min, float size) {
	Texture t(size, size);
	V3 color(0,0,0);
	for (int x = 0; x < size; x++) {
		for (int y = 0; y < size; y++) {
			color.setFromColor(get(x + min[0], y + min[1]));
			t.set(x, y, color);
		}
	}
	return t;
}


/// Set Texture types

void Texture::loadTiff(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		std::cerr << fname << " could not be opened" << endl;
		return;
	}
	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		w = width;
		h = height;
		delete[] pix;
		pix = new unsigned int[w * h];
	}
	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}
	TIFFClose(in);
} // load a tiff image to pixel buffer

void Texture::setChecker(int cSize, V3 rgb0, V3 rgb1) {
	for (int u = 0; u < w; u++) {
		for (int v = 0; v < h; v++) {
			int cv = v / cSize;
			int cu = u / cSize;
			set(u, v, ((cu + cv) % 2) ? rgb0 : rgb1);
		}
	}
}

void Texture::setXTex(V3 rgb0, V3 rgb1) {
	for (int u = 0; u < w; u++) {
		for (int v = 0; v < h; v++) {
			if ((u == (v * w / h)))
				set(u, v, rgb0);
			else if (u == ((-v * w / h) + w))
				set(u, v, rgb0);
			else
				set(u, v, rgb1);
		}
	}
}

