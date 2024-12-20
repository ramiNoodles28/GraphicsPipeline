#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "V3.h"
#include "M33.h"
#include "pointlight.h"
#include "envmap.h"
#include "TM.h"
//class PPC;

class FrameBuffer : public Fl_Gl_Window, public V3{
public:
	unsigned int *pix; // pixel array
	int w, h; // width and height of frame
	float* zb; // z buffer
	int s; // scale of camera movement
	int isHW; // if fb is rendered using hardware
	PPC *cam;
	V3 lp;

	FrameBuffer(int u0, int v0, int _w, int _h);
	void addCam(PPC* c);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void loadTiff(char* fname);
	void saveAsTiff(char* fname);
	void set(unsigned int bgr);
	void set(int u, int v, unsigned int col);
	unsigned int get(int u, int v);
	void setGuarded(int u, int v, unsigned int col);
	int inBounds(V3 p);

	V3 perspectiveInterp(M33 m, V3 inv, V3 w);

	void rasterizeRectangle(int u0, int v0, int l, int h, unsigned int col);
	int clipRectangle(int& u0, int& v0, int& rw, int& rh);
	void rasterizeCircle(V3 center, float radius, unsigned int color);

	void rasterizeTriLines(V3 p0, V3 p1, V3 p2, unsigned int color, PPC* ppc);
	void rasterizeTris(V3 a, V3 b, V3 c, M33 colors, PPC* ppc); // unlit vector interpolated raster
	void rasterizeTrisDirLight(V3 a, V3 b, V3 c, 
		M33 color, M33 norms, V3 lv, float ka, PPC* ppc); // directionally lit per pixel raster
	void rasterizeTrisPointLight(V3 a, V3 b, V3 c,
		M33 verts, M33 color, M33 norms, PPC* ppc, PointLight pl, 
		M33 texCoords = M33(), Texture* tex = nullptr);  // point lit per pixel raster
	void rasterizeTrisReflective(V3 a, V3 b, V3 c,
		M33 verts, M33 color, M33 norms, PPC* ppc, EnvMap* env); // unlit per pixel rasterization of reflective surface

	void renderWF(TM tm, PPC* ppc); // renders wireframe of mesh
	void renderTris(TM tm, PPC* ppc); // renders filled in tri mesh
	void renderTrisDirLight(TM tm, PPC* ppc, V3 lv, float ka); // render mesh with directional light
	void renderTrisPointLight(TM tm, PPC* ppc, PointLight pl); // render mesh with point light
	void renderTrisReflective(TM tm, PPC* ppc, EnvMap *env); // render mesh with reflective surface


	void rasterize2DSegment(V3 p0, V3 p1, unsigned int color, PPC* ppc);
	void rasterize2DSegment(V3 p0, V3 p1, V3 c0, V3 c1, PPC* ppc);
	void render3DSegment(V3 p0, V3 p1, V3 c0, V3 c1, PPC *ppc);
	void renderPoint(V3 p, float r, V3 c, PPC* ppc);

	void setBackgroundEnv(EnvMap* env, PPC* ppc); // set background of scene to environment

	
};