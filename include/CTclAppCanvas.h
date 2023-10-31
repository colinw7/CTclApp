#ifndef CTCL_APP_CANVAS_H
#define CTCL_APP_CANVAS_H

#include <CTclApp.h>
#include <CEvent.h>

#include <tcl.h>
#include <tk.h>

class  CTclAppCanvas;
struct CTclAppCanvasConfigData;

class CTclAppCanvasCmd {
 protected:
  CTclAppCanvasCmd(CTclApp *app);

 public:
  virtual ~CTclAppCanvasCmd();

  void init();

  virtual CTclAppCanvas *createInstance(CTclApp *app) = 0;

  virtual void initInstance(CTclApp *) { }

  virtual std::string getCommandName () const = 0;
  virtual std::string getCommandClass() const = 0;

  CTclApp *getTclApp() const { return app_; }

 private:
  CTclApp*    app_ { nullptr };
  Tcl_Command tcl_command_;
};

//---

class CTclAppCanvas {
 protected:
  CTclAppCanvas(CTclApp *app);

 public:
  virtual ~CTclAppCanvas();

  CTclApp *getApp() const { return app_; }

  Tcl_Interp *getTclInterp() const { return app_->getInterp(); }

  void setTclInstanceCommand(Tcl_Command tcl_instance_command) {
    tcl_instance_command_ = tcl_instance_command;
  }

  Tcl_Command getTclInstanceCommand() const { return tcl_instance_command_; }

  void setTkWindow(Tk_Window tk_window) {
    tk_window_ = tk_window;

    if (tk_window_)
      display_ = Tk_Display(tk_window_);
    else
      display_ = nullptr;
  }

  Tk_Window getTkWindow() const { return tk_window_; }

  Display *getDisplay() const { return display_; }

  CTclAppCanvasConfigData *getConfigData() { return config_data_; }

  int getWidth() const { return width_; }
  void setWidth(int width) { width_ = width; }

  int getHeight() const { return height_; }
  void setHeight(int height) { height_ = height; }

  bool getRedrawPending() { return redraw_pending_; }
  void setRedrawPending(bool redraw_pending) { redraw_pending_ = redraw_pending; }

  char *getPathName() const;

  void resize(int width, int height);

  void buttonPress  (int button, int x, int y);
  void buttonMotion (int x, int y);
  void buttonRelease(int button, int x, int y);

  void keyPress  (KeySym, const std::string &str, int x, int y);
  void keyRelease(int x, int y);

  void setModifiers(bool shift_key, bool control_key, bool mod_key);
  void setMultiClick(int multiclick_count);

  void setKeyAutoRepeatRate(unsigned int timeout, unsigned int interval);

  int getEventX() const { return event_x_; }
  int getEventY() const { return event_y_; }

  int  getButtonNum() const { return button_num_; }
  bool getButtonPressed() const { return button_pressed_; }

  KeySym      getKeySym() const { return key_sym_; }
  std::string getKeyStr() const { return key_str_; }

  bool getKeyPressed() const { return key_pressed_; }

  bool isShiftKey  () const { return shift_key_; }
  bool isControlKey() const { return control_key_; }
  bool isModKey    () const { return mod_key_; }

  int getMultiClick() const { return multiclick_count_; }

  CMouseEvent getMouseEvent() const;
  CKeyEvent   getKeyEvent  () const;

  virtual bool instanceCommand  (int, const char **) { return false; }
  virtual bool instanceConfigure(int, const char **) { return false; }

  virtual void redraw() = 0;

  virtual void buttonPressCB  () { }
  virtual void buttonMotionCB () { }
  virtual void buttonReleaseCB() { }

  virtual void keyPressCB  () { }
  virtual void keyReleaseCB() { }

 private:
  CTclApp*                 app_                  { nullptr };
  Tcl_Command              tcl_instance_command_ { nullptr };
  Tk_Window                tk_window_            { 0 };
  Display*                 display_              { nullptr };
  CTclAppCanvasConfigData* config_data_          { nullptr };
  bool                     redraw_pending_       { false };
  int                      event_x_              { 0 };
  int                      event_y_              { 0 };
  int                      button_num_           { 1 };
  bool                     button_pressed_       { false };
  KeySym                   key_sym_              { 0 };
  std::string              key_str_;
  bool                     key_pressed_          { false };
  bool                     shift_key_            { false };
  bool                     control_key_          { false };
  bool                     mod_key_              { false };
  int                      multiclick_count_     { 1 };
  int                      width_                { 300 };
  int                      height_               { 300 };
};

#endif
