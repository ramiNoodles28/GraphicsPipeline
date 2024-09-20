#include "scene.h"
#include "V3.h"
#include "M33.h"
#include "TM.h"

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

	tmsN = 1;
	tms = new TM[tmsN];


}


void Scene::DBG() {
	cerr << endl;

#if 0
	{
		unsigned int col = 0xFF000001;
		V3 colorv;
		colorv.SetFromColor(col);
		cerr << colorv << endl;
		return;
	}
#endif
	{
		tms[0].loadBin("geometry/teapot1K.bin");
		tms[0].onFlag = 1;
		V3 centroid = tms[0].centroid();

		tms[0].translate(V3(0.0f, 0.0f, -150.0f) - centroid);
		centroid = V3(0.0f, 0.0f, -150.0f);

		V3 newC = centroid + V3(100.0f, 150.0f, 0.0f);
		V3 newVD = (centroid - newC).normalize();
		V3 newUpG(0.0f, 1.0f, 0.0f);
		ppc->setPose(newC, newVD, newUpG);
		Render();
		fb->redraw();
		//PPC ppc2(90.0f, fb->w, fb->h);
		//ppc2.C = centroid;
		//ppc2.RenderWF(fb, 10.0f, ppc);
		fb->redraw();


		for (int fi = 0; fi < 3600; fi++) {
			Render();
			//ppc2.RenderWF(fb, 10.0f, ppc);
			fb->redraw();
			Fl::check();
			//ppc2.Translate(V3(.01f, 0.0f, 0.0f));
			tms[0].rotate(centroid, V3(0.0f, 1.0f, 0.0f), 0.5f);
			//tms[0].ScaleInPlace(0.999f);
			//ppc->Translate(V3(1.0f, 0.0f, 0.0f));
		}
		return;
	}

	{
		TM tm;
		float rw = 40.0f;
		float rh = 20.0f;
		tm.setRectangle(rw, rh);
		tm.translate(V3(0.0f, 0.0f, -40.0f));
		for (int fi = 0; fi < 1000; fi++) {
			fb->set(0xFFFFFFFF);
			tm.renderWF(fb, ppc);
			tm.translate(V3(0.0f, 0.0f, -.1f));
			fb->redraw();
			Fl::check();
		}
		return;
	}

	{
		V3 P(0.0f, 0.0f, -100.0f);
		V3 Q;
		if (!ppc->project(P, Q))
			return;
		fb->set(0xFFDDDDDD);
		fb->rasterizeCircle(Q, 3.0f, 0xFF0000FF);
		fb->redraw();
		return;
	}

	{
		int fN = 1000;
		V3 p0(20.0f, 30.3f, 0.0f);
		V3 p1(220.0f, 130.3f, 0.0f);
		for (int fi = 0; fi < fN; fi++) {
			fb->set(0xFFDDDDDD);
			V3 c0(0.0f, 0.0f, 0.0f);
			V3 c1(0.0f, 0.0f, 0.0f);
			fb->rasterize2DSegment(p0, p1, c0, c1);
			fb->redraw();
			Fl::check();
			p0 = p0 + V3(0.0f, +1.0f, 0.0f);
			p1 = p1 + V3(0.0f, -1.0f, 0.0f);
		}
		return;
	}
	{
		V3 center(100.3f, 200.9f, 0.0f);
		float radius = 34.1f;
		fb->set(0xFFFFFFFF);
		fb->rasterizeCircle(center, radius, 0xFF000000);
		fb->redraw();
		return;
	}


	{
		int u0 = 30;
		int v0 = 50;
		int l = 100;
		int h = 200;
		int fN = 600;
		for (int fi = 0; fi < fN; fi++) {
			fb->set(0xFFFFFFFF);
			unsigned int col = 0xFF00FF00;
			fb->rasterizeRectangle(u0, v0, l, h, col);
			fb->redraw();
			Fl::check();
			u0++;
		}
		return;
	}
	{
		M33 m;
		m[0] = V3(1.0f, 0.0f, 0.0f);
		m[1] = V3(0.0f, 1.0f, 0.0f);
		m[2] = V3(1.0f, 0.0f, 1.0f);
		cerr << m << endl;
		cerr << m.getCol(0);
		return;
		V3 v(10.0f, 9.0f, 3.0f);
		cerr << m*v << endl;
		return;
		cerr << "row 0: " << m[1] << endl;
		return;
	}
	{
		V3 v0(1.0f, 2.0f, 3.0f);
		cerr << endl << "v0: " << v0;
		return;
	}

	{
		V3 v0(1.0f, 2.0f, 3.0f);
		cerr << endl << v0[0] << " " << v0[1] << endl;
		v0[1] = 100.0f;
		return;
		V3 v1(2.0f, 2.0f, 2.0f);
		// v0[0]
		V3 v2 = v0 + v1;
		cerr << v2.xyz[0] << endl;
		return;
	}

	{
		cerr << "INFO: pressed DBG" << endl;
		fb->set(0xFF0000FF);
		fb->redraw();
	}
}

void Scene::NewButton() {
	cerr << "INFO: pressed New button on GUI" << endl;
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
