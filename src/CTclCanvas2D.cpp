#include <CTclCanvas2D.h>
#include <CTclAppCanvas.h>
#include <CTclApp.h>

#include <CXMachine.h>
#include <CXGraphics.h>
#include <CXImage.h>
#include <CImageLib.h>
#include <CFileUtil.h>
#include <CMathMacros.h>
#include <CLine2D.h>
#include <CThrow.h>

#include <std_Xt.h>

CTclCanvas2DCmd::
CTclCanvas2DCmd(CTclApp *app) :
 CTclAppCanvasCmd(app)
{
  init();
}

CTclCanvas2DCmd::
~CTclCanvas2DCmd()
{
  delete canvas_;
}

CTclAppCanvas *
CTclCanvas2DCmd::
createInstance(CTclApp *app)
{
  canvas_ = new CTclCanvas2D(app);

  return (CTclAppCanvas *) canvas_->getCanvasWidget();
}

//---

CTclCanvasWidget2D::
CTclCanvasWidget2D(CTclCanvas2D *canvas, CTclApp *app) :
 CTclAppCanvas(app), canvas_(canvas)
{
}

CTclCanvasWidget2D::
~CTclCanvasWidget2D()
{
  delete graphics_;
}

bool
CTclCanvasWidget2D::
instanceCommand(int argc, const char **argv)
{
  return canvas_->instanceCommandI(argc, argv);
}

bool
CTclCanvasWidget2D::
instanceConfigure(int argc, const char **argv)
{
  return canvas_->instanceConfigureI(argc, argv);
}

void
CTclCanvasWidget2D::
redraw()
{
  if (window_ == 0) {
    window_ = Tk_WindowId(getTkWindow());

    if (window_ != 0) {
      CXScreen *screen = CXMachineInst->getCXScreen(0);

      graphics_ = new CXGraphics(*screen, window_);

      canvas_->setGraphics(graphics_);
    }
  }

  if (! graphics_)
    return;

  graphics_->startDoubleBuffer();

  canvas_->drawStart();

  canvas_->draw();

  graphics_->endDoubleBuffer();

  graphics_->flushEvents();
}

void
CTclCanvasWidget2D::
buttonPressCB()
{
  canvas_->mousePress();
}

void
CTclCanvasWidget2D::
buttonMotionCB()
{
  canvas_->mouseMotion();
}

void
CTclCanvasWidget2D::
buttonReleaseCB()
{
  canvas_->mouseRelease();
}

void
CTclCanvasWidget2D::
keyPressCB()
{
  canvas_->keyPress();
}

void
CTclCanvasWidget2D::
keyReleaseCB()
{
  canvas_->keyRelease();
}

//---

CTclCanvas2D::
CTclCanvas2D(CTclApp *app)
{
  if (! CXMachineInst->getDisplay())
    CXMachineInst->openDisplay("");

  canvas_widget_ = new CTclCanvasWidget2D(this, app);

  setBounds(0, 0, 1, 1);
}

CTclCanvas2D::
~CTclCanvas2D()
{
  delete canvas_widget_;
}

CXScreen &
CTclCanvas2D::
getCXScreen() const
{
  if (! graphics_)
    CTHROW("No Screen");

  return graphics_->getCXScreen();
}

Window
CTclCanvas2D::
getXWindow() const
{
  if (! graphics_)
    CTHROW("No Window");

  return graphics_->getXWindow();
}

void
CTclCanvas2D::
setGraphics(CXGraphics *graphics)
{
  graphics_ = graphics;
}

void
CTclCanvas2D::
setBounds(double x1, double y1, double x2, double y2)
{
  transform_ =
    CTransform2D(0, getHeight() - 1, getWidth() - 1, 0, x1, y1, x2, y2);
}

void
CTclCanvas2D::
loadImage(const std::string &name)
{
  image_name_ = name;

  CFile file(image_name_);

  CImageFileSrc src(file);

  image_ = CImageMgrInst->createImage(src);
}

void
CTclCanvas2D::
saveImage(const std::string &name)
{
  if (image_.isValid())
    image_->write(name, CFileUtil::getImageTypeFromName(name));
}

int
CTclCanvas2D::
getWidth()
{
  return canvas_widget_->getWidth();
}

int
CTclCanvas2D::
getHeight()
{
  return canvas_widget_->getHeight();
}

void
CTclCanvas2D::
setBackground(const CRGB &rgb)
{
  if (! graphics_)
    return;

  graphics_->setBackground(rgb);
}

void
CTclCanvas2D::
setForeground(const CRGB &rgb)
{
  if (! graphics_)
    return;

  graphics_->setForeground(rgb);
}

void
CTclCanvas2D::
startDoubleBuffer()
{
  if (! graphics_)
    return;

  graphics_->startDoubleBuffer();
}

void
CTclCanvas2D::
endDoubleBuffer()
{
  if (! graphics_)
    return;

  graphics_->endDoubleBuffer();
}

void
CTclCanvas2D::
setBackgroundImage(CImagePtr)
{
}

void
CTclCanvas2D::
flush()
{
  if (! graphics_)
    return;

  graphics_->flushEvents();
}

void
CTclCanvas2D::
clear()
{
  if (! graphics_)
    return;

  graphics_->clear(false);
}

void
CTclCanvas2D::
drawBezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
  CLine2D line(x1, y1, x4, y4);

  CBBox2D bbox(transform_.getXMin2(), transform_.getYMin2(),
               transform_.getXMax2(), transform_.getYMax2());

  CLine2D cline;

  if (! line.clip(bbox, cline))
    return;

  double pixel_x1, pixel_y1, pixel_x2, pixel_y2;

  transform_.iconv(cline.start().x, cline.start().y, &pixel_x1, &pixel_y1);
  transform_.iconv(cline.end  ().x, cline.end  ().y, &pixel_x2, &pixel_y2);

  if (ROUND(pixel_x1) == ROUND(pixel_x2) &&
      ROUND(pixel_y1) == ROUND(pixel_y2)) {
    drawPoint(cline.start().x, cline.start().y);
    return;
  }

  /* Calculate number of straight lines to draw */

  int num_lines = ROUND(std::max(fabs(pixel_x2 - pixel_x1), fabs(pixel_y2 - pixel_y1))/4.0);

  /* Limit number of lines to the range
     (2 -> max(pixel width, pixel height)/2 */

  num_lines = std::max(num_lines, 2);
  num_lines = std::min(num_lines, std::max(getWidth(), getHeight())/2);

  /* Interpolate onto cubic at each sub-point, cubic goes from
     0.0 to 1.0 in parametric coordinates so each point is at
     '(j + 1)/num_lines'. */

  double ti = 1.0/num_lines;

  double x11 = x1, y11 = y1, x22, y22;
  double t11 = ti, t12, t13, t21, t22, t23;

  for (int i = 0; i < num_lines; ++i) {
    t21 = (1.0 - t11);

    t12 = t11*t11;
    t13 = t12*t11;

    t22 = t21*t21;
    t23 = t22*t21;

    x22 = x1*t23 + 3.0*x2*t11*t22 + 3.0*x3*t12*t21 + x4*t13;
    y22 = y1*t23 + 3.0*y2*t11*t22 + 3.0*y3*t12*t21 + y4*t13;

    drawLine(x11, y11, x22, y22);

    t11 += ti;

    x11 = x22;
    y11 = y22;
  }
}

void
CTclCanvas2D::
drawLine(double x1, double y1, double x2, double y2)
{
  if (! graphics_)
    return;

  double px1, py1, px2, py2;

  transform_.iconv(x1, y1, &px1, &py1);
  transform_.iconv(x2, y2, &px2, &py2);

  graphics_->drawLine(int(px1), int(py1), int(px2), int(py2));
}

void
CTclCanvas2D::
drawPoint(double x, double y)
{
  if (! graphics_)
    return;

  double px, py;

  transform_.iconv(x, y, &px, &py);

  graphics_->drawPoint(int(px), int(py));
}

void
CTclCanvas2D::
drawRectangle(double x, double y, double width, double height)
{
  if (! graphics_)
    return;

  double px1, py1, px2, py2;

  transform_.iconv(x        , y         , &px1, &py1);
  transform_.iconv(x + width, y + height, &px2, &py2);

  double px = std::min(px1, px2);
  double py = std::min(py1, py2);
  double pw = fabs(px2 - px1);
  double ph = fabs(py2 - py1);

  graphics_->drawRectangle(int(px), int(py), int(pw), int(ph));
}

void
CTclCanvas2D::
fillPolygon(double *x, double *y, int num_xy)
{
  if (! graphics_)
    return;

  int *px = new int [2*num_xy];
  int *py = &px[num_xy];

  double tpx, tpy;

  for (int i = 0; i < num_xy; ++i) {
    transform_.iconv(x[i], y[i], &tpx, &tpy);

    px[i] = int(tpx);
    py[i] = int(tpy);
  }

  graphics_->fillPolygon(px, py, num_xy);

  delete [] px;
}

void
CTclCanvas2D::
drawCircle(double x, double y, double r)
{
  if (! graphics_)
    return;

  double px1, py1, px2, py2;

  transform_.iconv(x    , y    , &px1, &py1);
  transform_.iconv(x + r, y + r, &px2, &py2);

  graphics_->drawCircle(int(px1), int(py1), int(fabs(px2 - px1)));
}

void
CTclCanvas2D::
fillCircle(double x, double y, double r)
{
  if (! graphics_)
    return;

  double px1, py1, px2, py2;

  transform_.iconv(x    , y    , &px1, &py1);
  transform_.iconv(x + r, y + r, &px2, &py2);

  graphics_->fillCircle(int(px1), int(py1), int(fabs(px2 - px1)));
}

void
CTclCanvas2D::
drawImage(CImagePtr image, double x, double y)
{
  if (! graphics_)
    return;

  double px, py;

  transform_.iconv(x, y, &px, &py);

  graphics_->drawImage(image, int(px), int(py));
}

double
CTclCanvas2D::
getMouseX()
{
  int x = canvas_widget_->getEventX();

  double wx, wy;

  transform_.conv(x, 0, &wx, &wy);

  return wx;
}

double
CTclCanvas2D::
getMouseY()
{
  int y = canvas_widget_->getEventY();

  double wx, wy;

  transform_.conv(0, y, &wx, &wy);

  return wy;
}

int
CTclCanvas2D::
getMouseButton()
{
  return canvas_widget_->getButtonNum();
}

bool
CTclCanvas2D::
isShiftKey()
{
  return canvas_widget_->isShiftKey();
}

bool
CTclCanvas2D::
isControlKey()
{
  return canvas_widget_->isControlKey();
}

std::string
CTclCanvas2D::
getKeyStr()
{
  return canvas_widget_->getKeyStr();
}

void
CTclCanvas2D::
redraw()
{
  canvas_widget_->redraw();
}

void
CTclCanvas2D::
drawStart()
{
  clear();

  graphics_->drawImage(image_, 0, 0);
}

bool
CTclCanvas2D::
instanceCommandI(int argc, const char **argv)
{
  if (argc <= 0)
    return false;

  if      (strcmp(argv[0], "loadImage") == 0) {
    if (argc <= 1)
      return false;

    loadImage(argv[1]);

    return true;
  }
  else if (strcmp(argv[0], "saveImage") == 0) {
    if (argc <= 1)
      return false;

    saveImage(argv[1]);

    return true;
  }
  else
    return instanceCommand(argc, argv);
}

bool
CTclCanvas2D::
instanceConfigureI(int argc, const char **argv)
{
  bool found = false;

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-image") == 0) {
      i++;

      if (i >= argc)
        return false;

      loadImage(argv[i]);

      found = true;
    }
  }

  if (found)
    return true;

  return instanceConfigure(argc, argv);
}
