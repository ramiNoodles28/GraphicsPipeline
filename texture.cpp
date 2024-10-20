#include "texture.h"

#include <iostream>
#include <tiffio.h>
//#include <GL/gl.h>
using namespace std;

Texture::Texture(int _w, int _h) {
	w = _w;
	h = _h;
	pix = new unsigned int[w * h];
}

unsigned int Texture::get(int u, int v) {
	return pix[(h - 1 - v) * w + u];
}

void Texture::set(int u, int v, V3 color) {
	pix[(h - 1 - v) * w + u] = color.getColor();
}


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

