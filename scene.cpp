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
	int h = 400;
	int w = 600;
	fb = new FrameBuffer(u0, v0, w, h);
	//fb->position(u0, v0);
	//fb->label("SW Framebuffer");
	//fb->show();
	//fb->redraw();

	hwfb = new FrameBuffer(u0, v0, w, h);
	hwfb->isHW = 1;
	//hwfb->position(2*u0 + w, v0);
	hwfb->position(u0, v0);
	hwfb->label("HW Framebuffer");
	hwfb->show();
	hwfb->redraw();
	renderMode = 0;

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
	fps = 0.0f;
	tex = new Texture(50, 50);
	mirrorTiles = 0;
	texType = 0;
	tileNum = 1;
	tex->loadTiff("textures/diamond_ore.tif");
	ppc->translate(V3(0, 0, 150));
}

void Scene::Render() {
	if (!fb) return;
	fb->set(0xFFFFFFFF);

	pLight[0] = PointLight(lp, ka, kd, 512);
	//pLight[0].setShadowMaps(tms, tmsN);
	//fb->setBackgroundEnv(env, ppc);
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].resetAllColors();
		switch (renderMode) {
		case 0:
			fb->renderWF(tms[tmi], ppc);
			break;
		case 1:
			fb->renderTris(tms[tmi], ppc);
			break;
		default:
			fb->renderTrisPointLight(tms[tmi], ppc, pLight[0]);
		}
	}
	ppc->clearZB();
	fb->redraw();
}

void Scene::RenderHW() {
	if (!hwfb) return;
	static double prevTime = 0.0;
	static int frameCount = 0;
	double currTime = glutGet(GLUT_ELAPSED_TIME);
	frameCount++;
	
	if (currTime - prevTime >= 100.0) {
		fps = (double(frameCount) / (currTime - prevTime)) * 1000.0f;
		prevTime = currTime;
		frameCount = 0;
	}

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ppc->setViewHW();
	for (int tmi = 0; tmi < tmsN; tmi++) {
		if (!tms[tmi].onFlag) continue;
		tms[tmi].resetAllColors();
		tms[tmi].renderHW(renderMode);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	renderText(10.0f, hwfb->h - 20, ("FPS: " + std::to_string(fps)).c_str());
	hwfb->redraw();
}

void Scene::renderText(float x, float y, const char* text) {
	// Save the current projection matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// Set up an orthographic projection
	glOrtho(0, hwfb->w, 0, hwfb->h, -1, 1);

	// Save the current modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2f(x, y);
	for (const char* c = text; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
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
	hwfb->clear();
	//fb->clear();
	
	V3 center(0, 0, -150.0f);
	{
		float rw = 50.0f;
		float rh = 50.0f;
		float rd = 50.0f;
		tms[0].setRectangle(rw, rh); // front
		tms[1].setRectangle(rw, rh);
		tms[1].rotate(tms[1].centroid(), V3(0, 1, 0), 180.0f); // back

		tms[2].setRectangle(rd, rh);
		tms[2].rotate(tms[2].centroid(), V3(0, 1, 0), 90.0f); // left
		tms[3].setRectangle(rd, rh);
		tms[3].rotate(tms[3].centroid(), V3(0, -1, 0), 90.0f); // right

		tms[4].setRectangle(rw, rh);
		tms[4].rotate(tms[4].centroid(), V3(-1, 0, 0), 90.0f); // top
		tms[5].setRectangle(rw, rh);
		tms[5].rotate(tms[5].centroid(), V3(1, 0, 0), 90.0f); // bottom

		tms[0].translate(center + V3(0, 0, rd / 2.0f));
		tms[1].translate(center - V3(0, 0, rd / 2.0f));
		tms[2].translate(center + V3(rw / 2.0f, 0, 0));
		tms[3].translate(center - V3(rw / 2.0f, 0, 0));
		tms[4].translate(center + V3(0, rh / 2.0f, 0));
		tms[5].translate(center - V3(0, rh / 2.0f, 0));

		tms[0].bakedColors[0] = V3(0, 1, 1);
		tms[0].bakedColors[1] = V3(0, 0, 1);
		tms[0].bakedColors[2] = V3(0, 0, 0);
		tms[0].bakedColors[3] = V3(0, 1, 0);

		tms[1].bakedColors[0] = V3(1, 1, 0);
		tms[1].bakedColors[1] = V3(1, 0, 0);
		tms[1].bakedColors[2] = V3(1, 0, 1);
		tms[1].bakedColors[3] = V3(1, 1, 1);
		
		tms[2].bakedColors[0] = V3(0, 1, 0);
		tms[2].bakedColors[1] = V3(0, 0, 0);
		tms[2].bakedColors[2] = V3(1, 0, 0);
		tms[2].bakedColors[3] = V3(1, 1, 0);

		tms[3].bakedColors[0] = V3(1, 1, 1);
		tms[3].bakedColors[1] = V3(1, 0, 1);
		tms[3].bakedColors[2] = V3(0, 0, 1);
		tms[3].bakedColors[3] = V3(0, 1, 1);

		tms[4].bakedColors[0] = V3(1, 1, 1);
		tms[4].bakedColors[1] = V3(0, 1, 1);
		tms[4].bakedColors[2] = V3(0, 1, 0);
		tms[4].bakedColors[3] = V3(1, 1, 0);

		tms[5].bakedColors[0] = V3(0, 0, 1);
		tms[5].bakedColors[1] = V3(1, 0, 1);
		tms[5].bakedColors[2] = V3(1, 0, 0);
		tms[5].bakedColors[3] = V3(0, 0, 0);


		tileNum = 1.0f;
		for (int i = 0; i < 6; i++) {
			
			tms[i].tex = tex;
			tms[i].texFlag = 1;
			tms[i].scaleTex(tileNum);
			tms[i].loadTexture();
		}
	}
	hwfb->addCam(ppc);
	hwfb->s = 2;
	//fb->addCam(ppc);
	//fb->s = 2;
	
	float t = 0.0f;
	while (true) {
		for (int i = 0; i < 6; i++) {
			tms[i].rotate(center, V3(0.0f, 1.0f, 0.0f), 1);
			tms[i].rotate(center, V3(1.0f, 0.0f, 0.0f), 1);
			tms[i].rotate(center, V3(0.0f, 0.0f, 1.0f), sin(t));
		}

		tex->mirrorTile = mirrorTiles;
		//Render();
		//fb->redraw();
		hwfb->redraw();
		Fl::check();
		t += 0.01f;
	} 
	
	/*
	tms[0].loadBin("geometry/teapot57K.bin");
	tms[0].translate(V3(0.0f, 0.0f, 0.0f) - tms[0].centroid());
	//tms[1].setGroundPlane(V3(0, -30, -150.0f), V3(0.5f, 0.5f, 0.5f), 100.0f);
	//env = new EnvMap("environments/room.tiff");	
	//Render();
	
	fb->addCam(ppc);
	fb->s = 2;
	float t = 0.0f;
	while (true) {
		//RenderHW();
		Render();
		fb->redraw();
		hwfb->redraw();
		Fl::check();
		tms[0].rotate(tms[0].centroid(), V3(0, 1, 0), 1.0f);
		t += 0.1f;
	} */
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
	ppc->saveToTxt("camPaths.txt");
	/*
	for (int i = 0; i < 6; i++) {
		tms[i].scaleTex(0.5f);
	}*/
}
void Scene::TileMinus() {
	int camNum = 5;
	vector<PPC> ppcs(camNum);
	loadCamsFromTxt("camPaths.txt", ppcs, camNum);
	for (int i = 0; i < camNum; ++i) {
		PPC p0 = ppcs[i];
		PPC p1 = (i + 1 == camNum) ? ppcs[0] : ppcs[i + 1];
		float dist = (p0.C - p1.C).length();
		for (float t = 0.0f; t < dist; t += dist / 100.f) {
			ppc->interpCam(p0, p1, t / dist);
			hwfb->redraw();
			Fl::check();
		}

	}


	/*
	for (int i = 0; i < 6; i++) {
		tms[i].scaleTex(2.0f);
	}*/
}
void Scene::TileMirror() {
	mirrorTiles = (mirrorTiles) ? 0 : 1;
}

void Scene::DBG() {
	renderMode = (renderMode > 1) ? 0 : renderMode + 1;
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

/////////////////// funny stuff




