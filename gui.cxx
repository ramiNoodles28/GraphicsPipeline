// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "gui.h"

void GUI::cb_DBG_i(Fl_Button*, void*) {
  DBG_cb();
}
void GUI::cb_DBG(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_DBG_i(o,v);
}

void GUI::cb_FreeCam_i(Fl_Button*, void*) {
    FreeCam_cb();
}
void GUI::cb_FreeCam(Fl_Button* o, void* v) {
  ((GUI*)(o->parent()->user_data()))->cb_FreeCam_i(o,v);
}

void GUI::cb_LightControl_i(Fl_Button*, void*) {
    LightControl_cb();
}
void GUI::cb_LightControl(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_LightControl_i(o, v);
}

void GUI::cb_LightType_i(Fl_Button*, void*) {
    LightType_cb();
}
void GUI::cb_LightType(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_LightType_i(o, v);
}

void GUI::cb_TilePlus_i(Fl_Button*, void*) {
    TilePlus_cb();
}
void GUI::cb_TilePlus(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_TilePlus_i(o, v);
}

void GUI::cb_TileMinus_i(Fl_Button*, void*) {
    TileMinus_cb();
}
void GUI::cb_TileMinus(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_TileMinus_i(o, v);
}

void GUI::cb_TileMirror_i(Fl_Button*, void*) {
    TileMirror_cb();
}
void GUI::cb_TileMirror(Fl_Button* o, void* v) {
    ((GUI*)(o->parent()->user_data()))->cb_TileMirror_i(o, v);
}


#include "scene.h"

GUI::GUI() {
    int h = 30; int w = 80; int p = 15;
  { uiw = new Fl_Double_Window(300, 250, "GUI");
    uiw->user_data((void*)(this));
    { Fl_Button* o = new Fl_Button(p, 2*(p + h) + p, w, h, "Cycle Tex");
      o->selection_color(FL_DARK_RED);
      o->callback((Fl_Callback*)cb_DBG);
    } // Fl_Button* o 
    { Fl_Button* o = new Fl_Button(p, p, w, h, "Free Cam");
      o->callback((Fl_Callback*)cb_FreeCam);
    } // Fl_Return_Button* o
    { Fl_Button* o = new Fl_Button(p, (p + h) + p, w, h, "Light Control");
  	  o->callback((Fl_Callback*)cb_LightControl);
    }

        // lighting buttons
    { Fl_Button* o = new Fl_Button(2 * p + w, p, 2 * w, h, "Tile +");
  	  o->callback((Fl_Callback*)cb_TilePlus);
    } // Fl_Button* o 
    { Fl_Button* o = new Fl_Button(2 * p + w, (p + h) + p, 2 * w, h, "Mirror Tiles Toggle");
      o->callback((Fl_Callback*)cb_TileMirror);
    } // Fl_Return_Button* o
    
    { Fl_Button* o = new Fl_Button(2 * p + w, 2 * (p + h) + p, 2 * w, h, "Tile -");
      o->callback((Fl_Callback*)cb_TileMinus);
    } // Fl_Return_Button* o
    
    uiw->end();
  } // Fl_Double_Window* uiw
}

int main(int argc, char **argv) {
  scene = new Scene;
  return Fl::run();
}

void GUI::show() {
  uiw->show();
}

void GUI::DBG_cb() {
  scene->DBG();
}

void GUI::FreeCam_cb() {
  scene->FreeCam();
}

void GUI::LightControl_cb() {
    scene->LightControl();
}

void GUI::LightType_cb() {
    scene->LightType();
}

void GUI::TilePlus_cb() {
    scene->TilePlus();
}

void GUI::TileMinus_cb() {
    scene->TileMinus();
}

void GUI::TileMirror_cb() {
    scene->TileMirror();
}