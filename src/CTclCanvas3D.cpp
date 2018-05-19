#include <CTclCanvas3D.h>
#include <CTclAppCanvas.h>
#include <CTclCanvasPixelRenderer.h>
#include <CTclApp.h>

#include <CCanvas3D.h>
#include <CCanvas3DObject.h>
#include <CGeomBox3D.h>
#include <CGeomCone3D.h>
#include <CGeomCube3D.h>
#include <CGeomCylinder3D.h>
#include <CGeomPyramid3D.h>
#include <CGeomSphere3D.h>
#include <CGeomTorus3D.h>

#include <CXMachine.h>
#include <CXGraphics.h>
#include <CStrUtil.h>
#include <std_Xt.h>

class CTclCanvas3D;

class CTclCanvas3DWidget : public CTclAppCanvas {
 public:
  CTclCanvas3DWidget(CTclCanvas3D *canvas, CTclApp *app);

  CXGraphics *getGraphics() { return graphics_; }

  bool instanceCommand(int argc, const char **argv);
  bool instanceConfigure(int argc, const char **argv);

  void redraw();

  void buttonPressCB();
  void buttonMotionCB();
  void buttonReleaseCB();
  void keyPressCB();
  void keyReleaseCB();

 private:
  bool createCommand(int argc, const char **argv);
  bool deleteCommand(int argc, const char **argv);
  bool zoomCommand(int argc, const char **argv);
  bool scrollCommand(int argc, const char **argv);

 private:
  CTclCanvas3D* canvas_   { nullptr };
  Window        window_   { 0 };
  CXGraphics*   graphics_ { nullptr };
};

//---

class CTclCanvas3D : public CCanvas3D {
 public:
  CTclCanvas3D(CTclApp *app);
 ~CTclCanvas3D();

  CTclCanvas3DWidget *getCanvasWidget() const { return canvas_widget_; }

  void setGraphics(CXGraphics *graphics);

  int canvasGetWidth ();
  int canvasGetHeight();

  void canvasSetBackground(const CRGBA &rgba);
  void canvasSetForeground(const CRGBA &rgba);

  void canvasSetLineWidth(double width);

  void canvasSetDashPattern(double *array, int nun, double offset);

  void canvasSetBackgroundImage(CImagePtr cimage);

  void canvasClear();

  void canvasDrawLine(int x1, int y1, int x2, int y2);

  void canvasDrawPoint(int x, int y);

  void canvasFillPolygon(int *x, int *y, int num_xy);

  void canvasDrawCircle(int x, int y, int r);
  void canvasFillCircle(int x, int y, int r);

  int canvasGetMouseX();
  int canvasGetMouseY();
  int canvasGetMouseButton();

  bool canvasIsShiftKey();
  bool canvasIsControlKey();

  void canvasRedraw();

 private:
  CTclCanvas3DWidget* canvas_widget_ { nullptr };
  CXGraphics*         graphics_ { nullptr };
};

//---

CTclCanvas3DCmd::
CTclCanvas3DCmd(CTclApp *app) :
 CTclAppCanvasCmd(app)
{
  init();
}

CTclAppCanvas *
CTclCanvas3DCmd::
createInstance(CTclApp *app)
{
  canvas_ = new CTclCanvas3D(app);

  initCanvas();

  return canvas_->getCanvasWidget();
}

CCanvas3D *
CTclCanvas3DCmd::
getCanvas3D()
{
  return (CCanvas3D *) canvas_;
}

CTclCanvas3DWidget::
CTclCanvas3DWidget(CTclCanvas3D *canvas, CTclApp *app) :
 CTclAppCanvas(app), canvas_(canvas)
{
}

bool
CTclCanvas3DWidget::
instanceCommand(int argc, const char **argv)
{
  if      (CStrUtil::casecmp(argv[0], "create") == 0) {
    if (! createCommand(argc - 1, &argv[1]))
      return false;
  }
  else if (CStrUtil::casecmp(argv[0], "delete") == 0) {
    if (! deleteCommand(argc - 1, &argv[1]))
      return false;
  }
  else if (CStrUtil::casecmp(argv[0], "zoom") == 0) {
    if (! zoomCommand(argc - 1, &argv[1]))
      return false;
  }
  else if (CStrUtil::casecmp(argv[0], "scroll") == 0) {
    if (! scrollCommand(argc - 1, &argv[1]))
      return false;
  }
  else
    return false;

  return true;
}

bool
CTclCanvas3DWidget::
createCommand(int argc, const char **argv)
{
  if (argc < 1)
    return false;

  if      (CStrUtil::casecmp(argv[0], "box") == 0) {
    CMaterial material(CRGBA(0,0,1), CRGBA(0,0,1), CRGBA(1,1,1), 50);

    CCanvas3DObject *object = new CCanvas3DObject;

    CGeomBox3D::addGeometry(object, 0, 0, 0, 1, 3, 5);

    object->setFrontMaterial(material);

    canvas_->addObject(object);

    canvas_->initView();

    canvas_->canvasRedraw();
  }
  else if (CStrUtil::casecmp(argv[0], "cone") == 0) {
    CMaterial material(CRGBA(0,0,1), CRGBA(0,0,1), CRGBA(1,1,1), 50);

    CCanvas3DObject *object = new CCanvas3DObject;

    CGeomCone3D::addGeometry(object, 0, 0, 0, 1, 3);

    object->setFrontMaterial(material);

    canvas_->addObject(object);

    canvas_->initView();

    canvas_->canvasRedraw();
  }
  else if (CStrUtil::casecmp(argv[0], "cube") == 0) {
    CMaterial material(CRGBA(0,0,1), CRGBA(0,0,1), CRGBA(1,1,1), 50);

    CCanvas3DObject *object = new CCanvas3DObject;

    CGeomCube3D::addGeometry(object, 0, 0, 0, 1);

    object->setFrontMaterial(material);

    canvas_->addObject(object);

    canvas_->initView();

    canvas_->canvasRedraw();
  }
  else if (CStrUtil::casecmp(argv[0], "cylinder") == 0) {
    CMaterial material(CRGBA(0,0,1), CRGBA(0,0,1), CRGBA(1,1,1), 50);

    CCanvas3DObject *object = new CCanvas3DObject;

    CGeomCylinder3D::addGeometry(object, 0, 0, 0, 1, 3);

    object->setFrontMaterial(material);

    canvas_->addObject(object);

    canvas_->initView();

    canvas_->canvasRedraw();
  }
  else if (CStrUtil::casecmp(argv[0], "pyramid") == 0) {
    CMaterial material(CRGBA(0,0,1), CRGBA(0,0,1), CRGBA(1,1,1), 50);

    CCanvas3DObject *object = new CCanvas3DObject;

    CGeomPyramid3D::addGeometry(object, 0, 0, 0, 1, 1);

    object->setFrontMaterial(material);

    canvas_->addObject(object);

    canvas_->initView();

    canvas_->canvasRedraw();
  }
  else if (CStrUtil::casecmp(argv[0], "sphere") == 0) {
    CMaterial material(CRGBA(0,0,1), CRGBA(0,0,1), CRGBA(1,1,1), 50);

    CCanvas3DObject *object = new CCanvas3DObject;

    CGeomSphere3D::addGeometry(object, CPoint3D(0, 0, 0), 1);

    object->setFrontMaterial(material);

    canvas_->addObject(object);

    canvas_->initView();

    canvas_->canvasRedraw();
  }
  else if (CStrUtil::casecmp(argv[0], "torus") == 0) {
    CMaterial material(CRGBA(0,0,1), CRGBA(0,0,1), CRGBA(1,1,1), 50);

    CCanvas3DObject *object = new CCanvas3DObject;

    CGeomTorus3D::addGeometry(object, 0, 0, 0, 1.0, 0.25, 1.0, 0.2, 36, 36);

    object->setFrontMaterial(material);

    canvas_->addObject(object);

    canvas_->initView();

    canvas_->canvasRedraw();
  }
  else
    return false;

  return true;
}

bool
CTclCanvas3DWidget::
deleteCommand(int argc, const char **argv)
{
  if (argc < 1)
    return false;

  if (CStrUtil::casecmp(argv[0], "all") == 0) {
    canvas_->deleteAll();
  }
  else
    return false;

  return true;
}

bool
CTclCanvas3DWidget::
zoomCommand(int argc, const char **argv)
{
  if (argc < 1)
    return false;

  double factor;

  if (! CStrUtil::toReal(argv[0], &factor))
    return false;

  canvas_->zoom(factor);

  return true;
}

bool
CTclCanvas3DWidget::
scrollCommand(int argc, const char **argv)
{
  if (argc < 2)
    return false;

  double x, y;

  if (! CStrUtil::toReal(argv[0], &x))
    return false;

  if (! CStrUtil::toReal(argv[1], &y))
    return false;

  canvas_->scroll(x, y);

  return true;
}

bool
CTclCanvas3DWidget::
instanceConfigure(int argc, const char **argv)
{
  for (int i = 0; i < argc; i++) {
    if      (CStrUtil::casecmp(argv[i], "-drawmode") == 0) {
      i++;

      if (i >= argc)
        return false;

      if      (CStrUtil::casecmp(argv[i], "wireframe") == 0)
        canvas_->setFaceDrawType(CCanvas3DFace::DRAW_WIREFRAME);
      else if (CStrUtil::casecmp(argv[i], "solid"    ) == 0)
        canvas_->setFaceDrawType(CCanvas3DFace::DRAW_FLAT_FILLED);
      else
        return false;
    }
    else if (CStrUtil::casecmp(argv[i], "-fillmode") == 0) {
      i++;

      if (i >= argc)
        return false;

      if      (CStrUtil::casecmp(argv[i], "flat"   ) == 0)
        canvas_->setFaceDrawType(CCanvas3DFace::DRAW_FLAT_FILLED);
      else if (CStrUtil::casecmp(argv[i], "gouraud") == 0)
        canvas_->setFaceDrawType(CCanvas3DFace::DRAW_GOURAUD_FILLED);
      else if (CStrUtil::casecmp(argv[i], "phong"  ) == 0)
        canvas_->setFaceDrawType(CCanvas3DFace::DRAW_PHONG_FILLED);
      else
        return false;
    }
    else if (CStrUtil::casecmp(argv[i], "-useambient" ) == 0) {
      i++;

      if (i >= argc)
        return false;

      int shade_flags = canvas_->getShade();

      shade_flags &= ~CCanvas3DPointShade::SHADE_ALL;

      if (CStrUtil::toBool(argv[i]))
        shade_flags |= CCanvas3DPointShade::SHADE_AMBIENT;

      canvas_->setShade(shade_flags);
    }
    else if (CStrUtil::casecmp(argv[i], "-usediffuse" ) == 0) {
      i++;

      if (i >= argc)
        return false;

      int shade_flags = canvas_->getShade();

      shade_flags &= ~CCanvas3DPointShade::SHADE_ALL;

      if (CStrUtil::toBool(argv[i]))
        shade_flags |= CCanvas3DPointShade::SHADE_AMBIENT |
                       CCanvas3DPointShade::SHADE_DIFFUSE;

      canvas_->setShade(shade_flags);
    }
    else if (CStrUtil::casecmp(argv[i], "-usespecular") == 0) {
      i++;

      if (i >= argc)
        return false;

      int shade_flags = canvas_->getShade();

      shade_flags &= ~CCanvas3DPointShade::SHADE_ALL;

      if (CStrUtil::toBool(argv[i]))
        shade_flags |= CCanvas3DPointShade::SHADE_ALL;

      canvas_->setShade(shade_flags);
    }
    else
      return false;
  }

  return true;
}

void
CTclCanvas3DWidget::
redraw()
{
  if (window_ == 0) {
    window_ = Tk_WindowId(getTkWindow());

    if (window_ != 0) {
      CXMachineInst->openDisplay("");

      CXScreen *screen = CXMachineInst->getCXScreen(0);

      graphics_ = new CXGraphics(*screen, window_);

      canvas_->setGraphics(graphics_);
    }
  }

  if (window_ == 0)
    return;

  graphics_->startDoubleBuffer();

  canvas_->draw(nullptr, 0, 0);

  graphics_->endDoubleBuffer();

  graphics_->flushEvents();
}

void
CTclCanvas3DWidget::
buttonPressCB()
{
  // canvas_->mousePress();
}

void
CTclCanvas3DWidget::
buttonMotionCB()
{
  // canvas_->mouseMotion();
}

void
CTclCanvas3DWidget::
buttonReleaseCB()
{
  // canvas_->mouseRelease();
}

void
CTclCanvas3DWidget::
keyPressCB()
{
  // canvas_->keyPress();
}

void
CTclCanvas3DWidget::
keyReleaseCB()
{
  // canvas_->keyRelease();
}

CTclCanvas3D::
CTclCanvas3D(CTclApp *app) :
 CCanvas3D()
{
  canvas_widget_ = new CTclCanvas3DWidget(this, app);
}

CTclCanvas3D::
~CTclCanvas3D()
{
}

void
CTclCanvas3D::
setGraphics(CXGraphics *graphics)
{
  graphics_ = graphics;
}

int
CTclCanvas3D::
canvasGetWidth()
{
  return canvas_widget_->getWidth();
}

int
CTclCanvas3D::
canvasGetHeight()
{
  return canvas_widget_->getHeight();
}

void
CTclCanvas3D::
canvasSetBackground(const CRGBA &rgba)
{
  if (graphics_)
    graphics_->setBackground(rgba);
}

void
CTclCanvas3D::
canvasSetForeground(const CRGBA &rgba)
{
  if (graphics_)
    graphics_->setForeground(rgba);
}

void
CTclCanvas3D::
canvasSetLineWidth(double width)
{
  if (graphics_)
    graphics_->setLineWidth((int) width);
}

void
CTclCanvas3D::
canvasSetDashPattern(double * /*array*/, int /*num*/, double /*offset*/)
{
}

void
CTclCanvas3D::
canvasSetBackgroundImage(CImagePtr /*cimage*/)
{
}

void
CTclCanvas3D::
canvasClear()
{
  if (graphics_)
    graphics_->clear(false);
}

void
CTclCanvas3D::
canvasDrawLine(int x1, int y1, int x2, int y2)
{
  if (graphics_)
    graphics_->drawLine(x1, y1, x2, y2);
}

void
CTclCanvas3D::
canvasDrawPoint(int x, int y)
{
  if (graphics_)
    graphics_->drawPoint(x, y);
}

void
CTclCanvas3D::
canvasFillPolygon(int *x, int *y, int num_xy)
{
  if (graphics_)
    graphics_->fillPolygon(x, y, num_xy);
}

void
CTclCanvas3D::
canvasDrawCircle(int x, int y, int r)
{
  if (graphics_)
    graphics_->drawCircle(x, y, r);
}

void
CTclCanvas3D::
canvasFillCircle(int x, int y, int r)
{
  if (graphics_)
    graphics_->fillCircle(x, y, r);
}

int
CTclCanvas3D::
canvasGetMouseX()
{
  return canvas_widget_->getEventX();
}

int
CTclCanvas3D::
canvasGetMouseY()
{
  return canvas_widget_->getEventY();
}

int
CTclCanvas3D::
canvasGetMouseButton()
{
  return canvas_widget_->getButtonNum();
}

bool
CTclCanvas3D::
canvasIsShiftKey()
{
  return canvas_widget_->isShiftKey();
}

bool
CTclCanvas3D::
canvasIsControlKey()
{
  return canvas_widget_->isControlKey();
}

void
CTclCanvas3D::
canvasRedraw()
{
  canvas_widget_->redraw();
}
