#ifndef CTCL_CANVAS_2D_H
#define CTCL_CANVAS_2D_H

#include <CTclAppCanvas.h>
#include <CFont.h>
#include <CTransform2D.h>
#include <CRGB.h>

class CTclCanvas2D;
class CTclCanvasWidget2D;
class CXGraphics;
class CXScreen;

class CTclCanvas2DCmd : public CTclAppCanvasCmd {
 public:
  CTclCanvas2DCmd(CTclApp *app);
 ~CTclCanvas2DCmd();

  std::string getCommandName () const { return "CTclCanvas2D"; }
  std::string getCommandClass() const { return "CTclCanvas2D"; }

  CTclAppCanvas *createInstance(CTclApp *app);

 private:
  CTclCanvas2D* canvas_ { nullptr };
};

//---

class CTclCanvasWidget2D : public CTclAppCanvas {
 public:
  CTclCanvasWidget2D(CTclCanvas2D *canvas, CTclApp *app);
 ~CTclCanvasWidget2D();

  CXGraphics *getGraphics() { return graphics_; }
  CXGraphics *getCXXorGraphics() { return xor_graphics_; }

  bool instanceCommand(int argc, const char **argv);
  bool instanceConfigure(int argc, const char **argv);

  void redraw();

  void buttonPressCB();
  void buttonMotionCB();
  void buttonReleaseCB();
  void keyPressCB();
  void keyReleaseCB();

 private:
  CTclCanvas2D* canvas_       { nullptr };
  Window        window_       { 0 };
  CXGraphics*   graphics_     { nullptr };
  CXGraphics*   xor_graphics_ { nullptr };
};

//---

class CTclCanvas2D {
 public:
  CTclCanvas2D(CTclApp *app);

  virtual ~CTclCanvas2D();

  CTclCanvasWidget2D *getCanvasWidget () const { return canvas_widget_; }
  CXScreen           &getCXScreen     () const;
  CXGraphics         *getCXGraphics   () const { return graphics_     ; }
  CXGraphics         *getCXXorGraphics() const { return xor_graphics_ ; }
  Display            *getXDisplay     () const;
  Window              getXWindow      () const;

  void setGraphics(CXGraphics *graphics);
  void setXorGraphics(CXGraphics *xor_graphics);

  void setBounds(double x1, double y1, double x2, double y2);

  void loadImage(const std::string &name);
  void saveImage(const std::string &name);

  int getWidth ();
  int getHeight();

  void setBackground(const CRGB &rgb);
  void setForeground(const CRGB &rgb);

  void setBackgroundImage(CImagePtr cimage);

  void startDoubleBuffer();
  void endDoubleBuffer();

  void flush();

  void clear();

  void drawBezier(double x1, double y1, double x2, double y2,
                  double x3, double y3, double x4, double y4);

  void drawLine(double x1, double y1, double x2, double y2);

  void drawPoint(double x, double y);

  void drawRectangle(double x1, double y1, double x2, double y2);

  void fillPolygon(double *x, double *y, int num_xy);

  void drawCircle(double x, double y, double r);
  void fillCircle(double x, double y, double r);

  void drawImage(CImagePtr image, double x, double y);

  double getMouseX();
  double getMouseY();
  int    getMouseButton();

  bool isShiftKey();
  bool isControlKey();

  std::string getKeyStr();

  void redraw();

  bool instanceCommandI(int argc, const char **argv);
  bool instanceConfigureI(int argc, const char **argv);

  virtual bool instanceCommand(int, const char **) { return false; }
  virtual bool instanceConfigure(int, const char **) { return false; }

  void drawStart();

  virtual void draw() { }

  virtual void mousePress() { }
  virtual void mouseMotion() { }
  virtual void mouseRelease() { }
  virtual void keyPress() { }
  virtual void keyRelease() { }

 private:
  CTclCanvasWidget2D* canvas_widget_ { nullptr };
  CXGraphics*         graphics_      { nullptr };
  CXGraphics*         xor_graphics_  { nullptr };
  std::string         image_name_;
  CImagePtr           image_;
  CTransform2D        transform_;
};

#endif
