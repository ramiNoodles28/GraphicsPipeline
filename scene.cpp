#include <iostream>
#include <fstream>
#include <strstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include "scene.h"
#include "V3.h"
#include "M33.h"
#include "TM.h"
#include "easing.h"
Scene *scene;

using namespace std;



Scene::Scene() {
	int u0 = 20;
	int v0 = 40;
	int h = 480;
	int w = 640;
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

	tmsN = 3;
	tms = new TM[tmsN];

	lNum = 2;
	pLight = new PointLight[lNum];

	lightingMode = 2;
	lightType = 0;
	ka = 0.02f;
	kd = 1.0f;
	lv = V3(0, 0, 1);
	lp = V3(0, 10, -90);
	lp1 = V3(-150.0f, 0, -110);

}

void Scene::Render() {
	if (!fb) return;
	fb->set(0xFFFFFFFF);
	V3 lps[] = { lp, lp1 };
	/*for (int l = 0; l < lNum; l++) {
		pLight[l] = PointLight(lps[l], ka, kd, 512);
		pLight[l].setShadowMaps(tms, tmsN);
	}*/
	pLight[0] = PointLight(lp, ka, kd, 512);
	pLight[0].setShadowMaps(tms, tmsN);
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].resetAllColors();
		
		fb->renderPoint(pLight[0].lp, 3.5, V3(0, 0, 0), ppc);
		fb->renderTrisPointLight(tms[tmi], ppc, pLight[0]);
		//fb->renderWF(tms[tmi], ppc);
		
	}
	ppc->clearZB();
	fb->redraw();
}

void Scene::FreeCam() {
	cerr << "INFO: pressed Free Cam button on GUI" << endl;
	cerr << "Controls: "
			"\nW - Forward "
			"\nS - Backward "
			"\nA - Left "
			"\nD - Right"
			"\nQ - Up "
			"\nE - Down "
			"\nZ/X - Roll "
			"\nUp/Down Arrow - Tilt "
			"\nLeft/Right Arrow - Pan"
			"\nScroll Wheel - Zoom" << endl;
	fb->clear();
/*	{
		tms[0].loadBin("geometry/teapot57K.bin");
		tms[0].texFlag = 0;
		tms[0].translate(V3(0.0f, 0.0f, -150.0f) - tms[0].centroid());
		tms[1].loadBin("geometry/teapot1k.bin");
		tms[1].texFlag = 0;
		tms[1].translate(V3(80, 0.0f, -280.0f) - tms[1].centroid());
		tms[2].setGroundPlane(V3(0, -30, -150.0f), V3(0.5f, 0.5f, 0.5f), 100.0f);
		tms[2].texFlag = 0;
	} */
	{
		float rw = 100.0f;
		float rh = 50.0f;
		tms[0].setRectangle(rw, rh);
		tms[0].translate(V3(0.0f, 0.0f, -150.0f));
		
		tms[1].setRectangle(rw, rh);
		tms[1].rotate(tms[1].centroid(), V3(0, 1, 0), 180.0f);
		tms[1].translate(V3(0.0f, 0.0f, -150.0f));
		//tms[1].checkerTexture(5, V3(.9f, .9f, .9f), V3(0, 0, 0));
		//tms[0].checkerTexture(5, V3(.9f, .9f, .9f), V3(0, 0, 0));
		tms[0].xTexture(V3(.9f, .9f, .9f), V3(0, 0, 0));
		tms[1].xTexture(V3(.9f, .9f, .9f), V3(0, 0, 0));
		//tms[0].setTexture("textures/sultan.tif");
	}
	fb->addCam(ppc);
	fb->s = 2;
	float t = 0.0f;
	while (true) {
		tms[0].rotate(tms[0].centroid(), V3(0.0f, 1.0f, 0.0f), 0.4f);
		tms[1].rotate(tms[0].centroid(), V3(0.0f, 1.0f, 0.0f), 0.4f);
		//tms[0].rotate(tms[0].centroid(), V3(0.0f, 1.0f, 0.0f), cos(t) * 10.0f);
		//tms[1].scaleInPlace(1.0f + 0.01f * sin(t));
		//lv = lv.rotateAboutAxis(V3(0, 0, 0), V3(0, 1, 0), 4.0f);
		//lp = lp.rotateAboutAxis(tms[0].centroid(), V3(0, 1, 0), 4.0f);
		//lp1 = lp1.rotateAboutAxis(tms[0].centroid(), V3(0, 0, 1), 4.0f);
		Render();
		fb->redraw();
		Fl::check();
		t += 0.1f;
	}
}
void Scene::LightControl() {
	cerr << "INFO: pressed Light Control button on GUI" << endl;
	cerr << "Point Light Controls: "
		"\nI - Forward "
		"\nK - Backward "
		"\nJ - Left "
		"\nL - Right"
		"\nU - Up "
		"\nO - Down "  << endl;
	fb->clear();
	//tms[0].loadBin("geometry/teapot57K.bin");
	//tms[0].translate(V3(0.0f, 0.0f, -150.0f) - tms[0].centroid());
	lightType = 0;
	fb->lp = lp;
	while (true) {
		lp = fb->lp;

		Render();
		fb->redraw();
		Fl::check();
	}
}

void Scene::SM1() { 
	cerr << "Move Cam" << endl;
	//lightingMode = 0;
} // No lighting, using only baked colors

void Scene::SM2() {
	cerr << "Move Object" << endl;
	fb->clear();
	float t = 0.0f;
	while (true) {
		//tms[0].rotate(tms[0].centroid(), V3(0.0f, 1.0f, 0.0f), 0.5f);
		tms[0].translate(V3(cos(t) * 5.0f, 0.0f, 0.0f));
		Render();
		fb->redraw();
		Fl::check();
		t += 0.1f;
	}
} // Per Vertex Lighting

void Scene::SM3() {
	cerr << "sm3: per pixel lighting" << endl;
	//lightingMode = 2;
} // Per Pixel Lighting

void Scene::SM23() {
	cerr << "sm2 & sm3" << endl;
	//lightingMode = 3;
} // Per Pixel Lighting

void Scene::LightType() {
	lightType++;
	switch (lightType % 3) {
		case 0:
			cerr << "Point Light" << endl;
			break;
		default:
			cerr << "Directional Light" << endl;
	}
} // Per Pixel Lighting

/////////////////// funny stuff

void Scene::DBG() {
	cerr << endl;

	{
		float rw = 100.0f;
		float rh = 50.0f;
		tms[0].setRectangle(rw, rh);
		tms[0].setAllColors(V3(1.0f, 0.0f, 0.0f));
		tms[0].translate(V3(0.0f, 0.0f, -200.0f));
		Texture* tex = 0;
		tex = new Texture(200, 100);
		tex->loadTiff("textures/sultan.tif");
		tms[0].tex = tex;
		Render();

		return;
	}
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
			float t = (float)fi / (float)loop;
			//ppc->pan(0.1f);
			ppc->interpCam(ppc0, ppc1, t, easeOutBounce);
		}
		*ppc = ppc0;
		return;
	}
}

void Scene::loadCamsFromTxt(char* fname, vector<PPC>& ppcs, int camNum) {
	ifstream ifs(fname);
	if (!ifs.is_open()) {
		cerr << "ERROR: Could not open file: " << fname << endl;
		return;
	}
	for (int i = 0; i < camNum; i++) {
		ifs >> ppcs[i];
		cerr << ppcs[i].C << endl;
	}
	ifs.close();
}

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

void Scene::PathCam() {
	cerr << "INFO: pressed Path Cam button on GUI" << endl;
	ppc = new PPC(60.f, fb->w, fb->h);
	fb->clear();
	tms[0].loadBin("geometry/teapot1K.bin");
	tms[0].translate(V3(0.0f, 0.0f, -150.0f) - tms[0].centroid());
	tms[1] = tms[0].boundingbox();
	tms[2].loadBin("geometry/teapot1k.bin");
	tms[2].translate(V3(-150, 0.0f, -150.0f) - tms[2].centroid());
	tms[3] = tms[2].boundingbox();
	tms[4].loadBin("geometry/teapot1k.bin");
	tms[4].translate(V3(150, 0.0f, -150.0f) - tms[4].centroid());
	tms[5] = tms[4].boundingbox();
	Render();
	int camNum = 8;
	vector<PPC> ppcs(camNum);
	loadCamsFromTxt("camPaths.txt", ppcs, camNum);
	int frame = 0;
	for (int i = 0; i < camNum; i++) {
		PPC p0 = ppcs[i];
		PPC p1 = (i + 1 == camNum) ? ppcs[0] : ppcs[i + 1];
		float loop = 31.4;

		for (float t = 0.0f; t < loop; t += 0.6f) {
			ostringstream oss;
			//oss << "camFrames/frame" << setw(4) << setfill('0') << frame << ".tiff";
			string img = oss.str();
			fb->saveAsTiff((char*)img.c_str());
			tms[0].rotate(tms[0].centroid(), V3(0.0f, 1.0f, 0.0f), cos(t) * 10.0f);
			tms[2].scaleInPlace(1.0f + 0.01f * sin(t));
			tms[4].translate(V3(1, 0, 0) * cos(t));
			frame++;
			ppc->interpCam(p0, p1, t / loop);
			Render();
			fb->redraw();
			Fl::check();
		}
	}
}

