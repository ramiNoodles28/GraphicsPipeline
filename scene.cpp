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
	int h = 720;
	int w = 1280;
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


	lNum = 2;
	pLight = new PointLight[lNum];

	lightingMode = 2;
	lightType = 0;
	ka = 0.02f;
	kd = 1.0f;
	lv = V3(0, 0, 1);
	lp = V3(0, 50, -30);
	lp1 = V3(-150.0f, 0, -110);

	tex = new Texture(50, 50);
	mirrorTiles = 0;
	texType = 0;
	tileNum = 2;
	tex->setChecker(5, V3(.9f, .9f, .9f), V3(0, 0, 0));
	ppc->translate(V3(0, 0, 150));
}

void Scene::Render() {
	if (!fb) return;
	fb->set(0xFFFFFFFF);
	V3 lps[] = { lp, lp1 };

	pLight[0] = PointLight(lp, ka, kd, 512);
	pLight[0].setShadowMaps(tms, tmsN);
	fb->setBackgroundEnv(env, ppc);
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].resetAllColors();
		
		//fb->renderPoint(pLight[0].lp, 3.5, V3(0, 0, 0), ppc);
		
		fb->renderTrisReflective(tms[tmi], ppc, env);
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
	/*
	V3 center(0, 0, -150.0f);
	{
		float rw = 50.0f;
		float rh = 50.0f;
		float rd = 50.0f;
		tms[0].setRectangle(rw, rh); // front
		tms[1].setRectangle(rw, rh);
		tms[1].rotate(tms[1].centroid(), V3(0, 1, 0), 180.0f); // back

		tms[2].setRectangle(rd, rh);
		tms[2].rotate(tms[2].centroid(), V3(0, 1, 0), 90.0f);
		tms[3].setRectangle(rd, rh);
		tms[3].rotate(tms[3].centroid(), V3(0, -1, 0), 90.0f);

		tms[4].setRectangle(rw, rh);
		tms[4].rotate(tms[4].centroid(), V3(-1, 0, 0), 90.0f);
		tms[5].setRectangle(rw, rh);
		tms[5].rotate(tms[5].centroid(), V3(1, 0, 0), 90.0f);

		tms[0].translate(center + V3(0, 0, rd / 2.0f));
		tms[1].translate(center - V3(0, 0, rd / 2.0f));
		tms[2].translate(center + V3(rw / 2.0f, 0, 0));
		tms[3].translate(center - V3(rw / 2.0f, 0, 0));
		tms[4].translate(center + V3(0, rh / 2.0f, 0));
		tms[5].translate(center - V3(0, rh / 2.0f, 0));


		tileNum = 2.0f;
		for (int i = 0; i < 6; i++) {
			tms[i].tex = tex;
			tms[i].texFlag = 1;
			tms[i].scaleTex(tileNum);
		}
	}
	fb->addCam(ppc);
	fb->s = 2;
	float t = 0.0f;
	while (true) {
		for (int i = 0; i < 6; i++) {
			tms[i].rotate(center, V3(0.0f, 1.0f, 0.0f), cos(t));
			tms[i].rotate(center, V3(1.0f, 0.0f, 0.0f), 0.001f);
			tms[i].rotate(center, V3(0.0f, 0.0f, 1.0f), sin(t));
		}

		tex->mirrorTile = mirrorTiles;
		Render();
		fb->redraw();
		Fl::check();
		t += 0.01f;
	} 
	*/
	
	tms[0].loadBin("geometry/teapot57K.bin");
	tms[0].translate(V3(0.0f, 0.0f, 0.0f) - tms[0].centroid());
	//tms[1].setGroundPlane(V3(0, -30, -150.0f), V3(0.5f, 0.5f, 0.5f), 100.0f);
	//env = new EnvMap("environments/uffizi_cross.tiff");
	//env = new EnvMap("environments/testEnv.tiff");
	env = new EnvMap("environments/room.tiff");	
	//Render();
	fb->addCam(ppc);
	fb->s = 2;
	float t = 0.0f;
	while (true) {
		Render();
		fb->redraw();
		Fl::check();
		t += 0.1f;
	} 
}

void Scene::LightControl() {
	fb->clear();
	V3 newC = V3(0, 0, 150);
	V3 vd = V3(0, 0, -1);
	V3 upD = V3(0, 1, 0);
	ppc->setPose(newC, vd, upD);
	while (true) {
		newC = ppc->C.rotateAboutAxis(tms[0].centroid(), V3(0, 1, 0), 1.0f);
		vd = (V3(0, 0, 0) - newC).normalize();
		upD = (ppc->b * -1.0f).normalize();
		ppc->setPose(newC, vd, upD);
		Render();
		fb->redraw();
		Fl::check();
	}
	/*cerr << "INFO: pressed Light Control button on GUI" << endl;
	cerr << "Point Light Controls: "
		"\nI - Forward "
		"\nK - Backward "
		"\nJ - Left "
		"\nL - Right"
		"\nU - Up "
		"\nO - Down "  << endl;
	fb->clear();
	lightType = 0;
	fb->lp = lp;
	while (true) {
		lp = fb->lp;
		Render();
		fb->redraw();
		Fl::check();
	}*/
}

void Scene::TilePlus() {
	for (int i = 0; i < 6; i++) {
		tms[i].scaleTex(0.5f);
	}
}
void Scene::TileMinus() {
	for (int i = 0; i < 6; i++) {
		tms[i].scaleTex(2.0f);
	}
}
void Scene::TileMirror() {
	mirrorTiles = (mirrorTiles) ? 0 : 1;
}

void Scene::DBG() {

	
	texType = (texType > 1) ? 0 : texType + 1;
	switch (texType) {
	case 0:
		env = new EnvMap("environments/room.tiff");
		//tex->w = 50; tex->h = 50;
		//tex->setChecker(5, V3(.9f, .9f, .9f), V3(0, 0, 0));
		break;
	case 1:
		env = new EnvMap("environments/testEnv.tiff");
		//tex->w = 50; tex->h = 50;
		//tex->setXTex(V3(.9f, .9f, .9f), V3(0, 0, 0));
		break;
	//case 2:
		//tex->loadTiff("textures/brick.tif");
		//break;
	//case 3:
		//tex->loadTiff("textures/diamond_ore.tif");
		//break;
	default:
		env = new EnvMap("environments/uffizi_cross.tiff");
		//tex->loadTiff("textures/light_gray_glazed_terracotta.tif");
	}
	
}

/////////////////// funny stuff




