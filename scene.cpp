#include "scene.h"
#include "V3.h"
#include "M33.h"
#include "TM.h"
#include "easing.h"
Scene *scene;

using namespace std;

#include <iostream>
#include <fstream>
#include <strstream>

Scene::Scene() {
	int u0 = 20;
	int v0 = 40;
	int h = 400;
	int w = 600;
	fb = new FrameBuffer(u0, v0, w, h);
	fb->position(u0, v0);
	fb->label("SW Framebuffer");
	fb->show();
	fb->redraw();
	float hfov = 60.0f;
	ppc = new PPC(hfov, fb->w, fb->h);

	gui = new GUI();
	gui->show();
	gui->uiw->position(u0, v0 + fb->h + v0);

	tmsN = 6;
	tms = new TM[tmsN];
}

void Scene::Render() {
	if (!fb)
		return;
	fb->set(0xFFFFFFFF);
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].renderWF(fb, ppc);
	}
	fb->redraw();
}

void Scene::DBG() {
	cerr << endl;

	{
		tms[0].loadBin("geometry/teapot1K.bin");
		V3 centroid;
		tms[0].translate(V3(0.0f, 0.0f, -150.0f) - tms[0].centroid());
		tms[1] = tms[0].boundingbox();
		tms[2] = tms[1].boundingbox();
		tms[2].translate(V3(-150, 0.0f, -150.0f) - tms[2].centroid());
		//tms[3] = tms[2].boundingbox();
		//tms[3].translate(V3(-150, 0.0f, 150.0f) - tms[3].centroid());

		centroid = V3(0.0f, 0.0f, -150.0f);
		
		V3 newC = centroid + V3(100.0f, 150.0f, 0.0f);
		V3 newVD = (centroid - newC).normalize();
		V3 newUpG(0.0f, 1.0f, 0.0f);
		PPC ppc0 = *ppc;
		ppc->setPose(newC, newVD, newUpG);
		//ppc->pan(-30.0f);
		PPC ppc1 = *ppc;
		Render();
		fb->redraw();

		int loop = 1800;
		
		for (int fi = 8; fi < loop; fi++) {
			Render();
			//ppc2.renderWF(fb, 10.0f, ppc);
			fb->redraw();
			Fl::check();
			//ppc2.translate(V3(.1f, 0.0f, 0.0f));
			//tms[0].rotate(centroid, V3(0.0f, 1.0f, 0.0f), 0.5f);
			//tms[1].rotate(centroid, V3(0.0f, 1.0f, 0.0f), 0.5f);
			//tms[0].scaleInPlace(0.999f);
			//ppc->translate(V3(1.0f, 0.0f, 0.0f));
			float t = (float) fi / (float) loop;
			//ppc->pan(0.1f);
			ppc->interpCam(ppc0, ppc1, t, easeOutBounce);
		}
		*ppc = ppc0;
		return;
	}
}

void Scene::FreeCam() {
	cerr << "INFO: pressed Free Cam button on GUI" << endl;
	cerr << "Contols: "
			"\nW - Forward "
			"\nS - Backward "
			"\nA - Left "
			"\nD - Right"
			"\nQ - Up "
			"\nE - Down "
			"\nZ/X - Roll "
			"\nUp/Down Arrow - Tilt "
			"\nLeft/Right Arrow - Pan" << endl;
	tms[0].loadBin("geometry/teapot1K.bin");
	V3 centroid;
	tms[0].translate(V3(0.0f, 0.0f, -150.0f) - tms[0].centroid());
	Render();
	fb->addCam(ppc);
	fb->s = 2;
	int t = 0;
	while (true) {



		Render();
		fb->redraw();
		Fl::check();
		t++;
	}
}

void Scene::PathCam() {
	cerr << "INFO: pressed Path Cam button on GUI" << endl;
	ppc = new PPC(60.f, fb->w, fb->h);
}






/////////////////// funny stuff

void Scene::RotatingPoints() {
	V3 xAx(1, 0, 0);
	M33 xRot(xAx, 90);
	cout << xAx;
	cout << xRot;

	V3 point(6, 1, 1);
	V3 origin(3, 7, 5);
	V3 axis(9, 1, 5);

	M33 fun(point, origin, axis);
	cout << fun;
	cout << fun.invert();
	cout << fun * fun.invert();

	ofstream of("README.txt");
	ofstream gf("graphPoints.txt");
	of << "Point: " << point;
	of << "Origin: " << origin;
	of << "Axis: " << axis << "\n ===== POINTS =====\n";
	gf << "x,y,z\n";
	gf << origin[0] << "," << origin[1] << "," << origin[2] << endl;
	gf << axis[0] << "," << axis[1] << "," << axis[2] << endl;
	V3 rot;
	for (int i = 0; i < 360; i++) {
		rot = point.rotateAboutAxis(origin, axis, i * 2);
		of << rot;
		gf << rot[0] << "," << rot[1] << "," << rot[2] << endl;
	}
} // Generate points rotated about an axis and save to file

void Scene::NameCycle() {
	for (int pos = 0; pos < (fb->w + 220) * 3 + 1; pos += 5) {
		fb->set(0xFFFFFFFF);
		float i = pos % (fb->w + 220);
		V3 o((i - 220), 50);
		//V3 o(100, 50);
		V3 oR = o;
		V3 oA = o + V3(60, 0);
		V3 oM = o + V3(130, 0);
		V3 oI = o + V3(200, 0);
		unsigned int b = 0xff000000;
		unsigned int w = 0xffffffff;
		float h = 80.0f;

		// R leg
		fb->rasterizeTris(oR, oR + V3(0, h), oR + V3(50, h), b);
		fb->rasterizeTris(oR + V3(0, 20), oR + V3(0, h), oR + V3(35, h), w);
		// R backing
		fb->rasterizeTris(oR, oR + V3(0, h), oR + V3(15, h), b);
		fb->rasterizeTris(oR, oR + V3(15, h), oR + V3(15, 0), 0xff000000);
		// R round
		fb->rasterizeCircle(oR + V3(25, 25), 25, b);
		fb->rasterizeCircle(oR + V3(25, 25), 10, w);

		// A frame
		fb->rasterizeTris(oA + V3(30, 0), oA + V3(0, h), oA + V3(60, h), b);
		fb->rasterizeTris(oA + V3(30, 35), oA + V3(15, h), oA + V3(45, h), w);
		// A bridge
		fb->rasterizeTris(oA + V3(45, 60), oA + V3(15, 60), oA + V3(45, 70), 0xff000000);
		fb->rasterizeTris(oA + V3(15, 60), oA + V3(15, 70), oA + V3(45, 70), 0xff000000);

		// M left peaks
		fb->rasterizeTris(oM, oM + V3(0, 60), oM + V3(40, 60), b);
		fb->rasterizeTris(oM + V3(55, 60), oM + V3(55, 0), oM + V3(15, 60), b);
		fb->rasterizeTris(oM + V3(15, 45), oM + V3(15, 60), oM + V3(25, 60), w);
		fb->rasterizeTris(oM + V3(40, 45), oM + V3(30, 60), oM + V3(40, 60), w);
		// M legs
		fb->rasterizeTris(oM + V3(0, 60), oM + V3(15, 80), oM + V3(15, 60), b);
		fb->rasterizeTris(oM + V3(0, 60), oM + V3(0, 80), oM + V3(15, 80), b);
		fb->rasterizeTris(oM + V3(55, 60), oM + V3(40, 60), oM + V3(40, 80), b);
		fb->rasterizeTris(oM + V3(55, 80), oM + V3(55, 60), oM + V3(40, 80), b);

		// I
		fb->rasterizeTris(oI, oI + V3(15, 80), oI + V3(15, 0), b);
		fb->rasterizeTris(oI, oI + V3(0, 80), oI + V3(15, 80), b);

		fb->redraw();
		Fl::check();
		//fb->SaveAsTiff("name.tiff");
	}
} // rasterize my name and have it translate across frame
