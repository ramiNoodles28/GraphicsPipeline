// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef gui_h
#define gui_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>

class GUI {
public:
  GUI();
  Fl_Double_Window *uiw;
private:
  inline void cb_DBG_i(Fl_Button*, void*);
  static void cb_DBG(Fl_Button*, void*);
  inline void cb_FreeCam_i(Fl_Button*, void*);
  static void cb_FreeCam(Fl_Button*, void*);
  inline void cb_PathCam_i(Fl_Button*, void*);
  static void cb_PathCam(Fl_Button*, void*);

  inline void cb_LightControl_i(Fl_Button*, void*);
  static void cb_LightControl(Fl_Button*, void*);
  inline void cb_SM1_i(Fl_Button*, void*);
  static void cb_SM1(Fl_Button*, void*);
  inline void cb_SM2_i(Fl_Button*, void*);
  static void cb_SM2(Fl_Button*, void*);
  inline void cb_SM3_i(Fl_Button*, void*);
  static void cb_SM3(Fl_Button*, void*);
  inline void cb_SM23_i(Fl_Button*, void*);
  static void cb_SM23(Fl_Button*, void*);
  inline void cb_LightType_i(Fl_Button*, void*);
  static void cb_LightType(Fl_Button*, void*);
public:
  void show();
  void DBG_cb();
  void FreeCam_cb();
  void PathCam_cb();

  void LightControl_cb();
  void SM1_cb();
  void SM2_cb();
  void SM3_cb();
  void SM23_cb();
  void LightType_cb();
};
#endif
