#include <CTclAppI.h>
#include <CTclAppCanvas.h>
#include <CXMachine.h>
#include <std_Xt.h>
#include <X11/XKBlib.h>

struct CTclAppCanvasConfigData {
  XColor *foreground { nullptr };
  XColor *background { nullptr };
  int     width      { -1 };
  int     height     { -1 };
};

static Tk_ConfigSpec
CTclAppCanvasConfigSpecs[] = {
  {TK_CONFIG_COLOR  , (char *) "-background", "background", "Background",
   "black", Tk_Offset(CTclAppCanvasConfigData, background), 0},
  {TK_CONFIG_SYNONYM, (char *) "-bg"        , "background", nullptr     ,
   nullptr, 0                                             , 0},

  {TK_CONFIG_COLOR  , (char *) "-foreground", "foreground", "Foreground",
   "white", Tk_Offset(CTclAppCanvasConfigData, foreground), 0},
  {TK_CONFIG_SYNONYM, (char *) "-fg"        , "foreground", nullptr     ,
   nullptr, 0                                             , 0},

  {TK_CONFIG_INT    , (char *) "-width"     , "width"     , "Width"     ,
   "300"  , Tk_Offset(CTclAppCanvasConfigData, width      ), 0},
  {TK_CONFIG_INT    , (char *) "-height"    , "height"    , "Height"    ,
   "300"  , Tk_Offset(CTclAppCanvasConfigData, height     ), 0},

  {TK_CONFIG_END, nullptr, nullptr, nullptr, nullptr, 0, 0},
};

static int  CTclAppCanvasClassCmd
             (ClientData clientData, Tcl_Interp *tcl_interp, int argc, const char **argv);
static int  CTclAppCanvasInstanceCmd
             (ClientData clientData, Tcl_Interp *tcl_interp, int argc, const char **argv);
static int  CTclAppCanvasConfigureProc
             (Tcl_Interp *tcl_interp, CTclAppCanvas *canvas, int argc, const char **argv,
              int flags);
static void CTclAppCanvasCalcGeometry
             (CTclAppCanvas *canvas);
static void CTclAppCanvasDisplayProc
             (ClientData clientData);
static void CTclAppCanvasExposureProc
             (ClientData clientData, XEvent *event);
static void CTclAppCanvasButtonPressProc
             (ClientData clientData, XEvent *event);
static void CTclAppCanvasButtonMotionProc
             (ClientData clientData, XEvent *event);
static void CTclAppCanvasButtonReleaseProc
             (ClientData clientData, XEvent *event);
static void CTclAppCanvasKeyPressProc
             (ClientData clientData, XEvent *event);
static void CTclAppCanvasKeyReleaseProc
             (ClientData clientData, XEvent *event);
static void CTclAppCanvasPointerMotionProc
             (ClientData clientData, XEvent *event);
static void CTclAppCanvasButtonEnterLeaveProc
             (ClientData clientData, XEvent *event);
static void CTclAppCanvasDestroyProc
             (ClientData clientData);

CTclAppCanvasCmd::
CTclAppCanvasCmd(CTclApp *app) :
 app_(app)
{
}

CTclAppCanvasCmd::
~CTclAppCanvasCmd()
{
}

void
CTclAppCanvasCmd::
init()
{
  tcl_command_ =
    Tcl_CreateCommand(app_->getInterp(), (char *) getCommandName().c_str(),
                      (Tcl_CmdProc *) CTclAppCanvasClassCmd, (ClientData) this,
                      (Tcl_CmdDeleteProc *) nullptr);
}

CTclAppCanvas::
CTclAppCanvas(CTclApp *app) :
 app_(app)
{
  config_data_ = new CTclAppCanvasConfigData();

  config_data_->foreground = nullptr;
  config_data_->background = nullptr;
  config_data_->width      = 300;
  config_data_->height     = 300;
}

CTclAppCanvas::
~CTclAppCanvas()
{
  delete config_data_;
}

char *
CTclAppCanvas::
getPathName() const
{
  return Tk_PathName(tk_window_);
}

void
CTclAppCanvas::
resize(int width, int height)
{
  width_  = width;
  height_ = height;

//Tk_ResizeWindow(getTkWindow(), getWidth(), getHeight());

  Tk_GeometryRequest(getTkWindow(), getWidth(), getHeight());
}

void
CTclAppCanvas::
buttonPress(int button, int x, int y)
{
  button_num_     = button;
  event_x_        = x;
  event_y_        = y;
  button_pressed_ = true;

  buttonPressCB();
}

void
CTclAppCanvas::
buttonMotion(int x, int y)
{
  event_x_ = x;
  event_y_ = y;

  if (button_pressed_)
    buttonMotionCB();
}

void
CTclAppCanvas::
buttonRelease(int button, int x, int y)
{
  button_num_     = button;
  event_x_        = x;
  event_y_        = y;
  button_pressed_ = false;

  buttonReleaseCB();
}

void
CTclAppCanvas::
keyPress(KeySym keysym, const std::string &str, int x, int y)
{
  key_sym_     = keysym;
  key_str_     = str;
  event_x_     = x;
  event_y_     = y;
  key_pressed_ = true;

  keyPressCB();
}

void
CTclAppCanvas::
keyRelease(int x, int y)
{
  event_x_     = x;
  event_y_     = y;
  key_pressed_ = false;

  keyReleaseCB();
}

void
CTclAppCanvas::
setModifiers(bool shift_key, bool control_key, bool mod_key)
{
  shift_key_   = shift_key;
  control_key_ = control_key;
  mod_key_     = mod_key;
}

void
CTclAppCanvas::
setMultiClick(int multiclick_count)
{
  multiclick_count_ = multiclick_count;
}

void
CTclAppCanvas::
setKeyAutoRepeatRate(unsigned int timeout, unsigned int interval)
{
  XkbSetAutoRepeatRate(Tk_Display(getTkWindow()), XkbUseCoreKbd, timeout, interval);
}

CMouseEvent
CTclAppCanvas::
getMouseEvent() const
{
  uint modifier = CMODIFIER_NONE;

  if (isShiftKey  ()) modifier |= CMODIFIER_SHIFT;
  if (isControlKey()) modifier |= CMODIFIER_CONTROL;
  if (isModKey    ()) modifier |= CMODIFIER_ALT;

  return CMouseEvent(CIPoint2D(getEventX(), getEventY()), (CMouseButton) getButtonNum(),
                     getMultiClick(), (CEventModifier) modifier);
}

CKeyEvent
CTclAppCanvas::
getKeyEvent() const
{
  uint modifier = 0;

  if (isShiftKey  ()) modifier |= CMODIFIER_SHIFT;
  if (isControlKey()) modifier |= CMODIFIER_CONTROL;
  if (isModKey    ()) modifier |= CMODIFIER_ALT;

  return CKeyEvent(CIPoint2D(getEventX(), getEventY()), (CKeyType) getKeySym(),
                   getKeyStr(), (CEventModifier) modifier);
}

static int
CTclAppCanvasClassCmd(ClientData clientData, Tcl_Interp *tcl_interp, int argc, const char **argv)
{
  auto *cmd = static_cast<CTclAppCanvasCmd *>(clientData);

  if (argc < 2) {
    Tcl_AppendResult(tcl_interp, "wrong # args: should be \"",
                     argv[0], " pathName ?options?\"", (char *) nullptr);
    return TCL_ERROR;
  }

  Tk_Window tk_window =
    Tk_CreateWindowFromPath(tcl_interp, Tk_MainWindow(tcl_interp), argv[1], (char *) nullptr);

  if (! tk_window)
    return TCL_ERROR;

  auto *tclApp = cmd->getTclApp();

  auto *canvas = cmd->createInstance(tclApp);

  canvas->setTkWindow(tk_window);

  Tk_SetClass(tk_window, (char *) cmd->getCommandClass().c_str());

  Tk_CreateEventHandler(tk_window, ExposureMask | StructureNotifyMask,
                        CTclAppCanvasExposureProc, (ClientData) canvas);
  Tk_CreateEventHandler(tk_window, ButtonPressMask,
                        CTclAppCanvasButtonPressProc, (ClientData) canvas);
  Tk_CreateEventHandler(tk_window, ButtonMotionMask,
                        CTclAppCanvasButtonMotionProc, (ClientData) canvas);
  Tk_CreateEventHandler(tk_window, ButtonReleaseMask,
                        CTclAppCanvasButtonReleaseProc, (ClientData) canvas);
  Tk_CreateEventHandler(tk_window, KeyPressMask,
                        CTclAppCanvasKeyPressProc, (ClientData) canvas);
  Tk_CreateEventHandler(tk_window, KeyReleaseMask,
                        CTclAppCanvasKeyReleaseProc, (ClientData) canvas);
  Tk_CreateEventHandler(tk_window, PointerMotionMask,
                        CTclAppCanvasPointerMotionProc, (ClientData) canvas );
  Tk_CreateEventHandler(tk_window, EnterWindowMask | LeaveWindowMask,
                        CTclAppCanvasButtonEnterLeaveProc, (ClientData) canvas);

  Tcl_Command tcl_instance_command =
    Tcl_CreateCommand(tcl_interp, Tk_PathName(tk_window),
                      (Tcl_CmdProc *) CTclAppCanvasInstanceCmd, (ClientData) canvas,
                      (Tcl_CmdDeleteProc *) nullptr);

  canvas->setTclInstanceCommand(tcl_instance_command);

  if (CTclAppCanvasConfigureProc(tcl_interp, canvas, argc, argv, 0) != TCL_OK) {
    Tk_DestroyWindow(tk_window);
    return TCL_ERROR;
  }

  tclApp->setObjResult(Tcl_NewStringObj(Tk_PathName(tk_window), -1));

  cmd->initInstance(tclApp);

  return TCL_OK;
}

static int
CTclAppCanvasInstanceCmd(ClientData clientData, Tcl_Interp *tcl_interp,
                         int argc, const char **argv)
{
  CTclAppCanvas *canvas = (CTclAppCanvas *) clientData;

  if (argc < 2) {
    Tcl_AppendResult(tcl_interp, "wrong # args: should be \"",
                     argv[0], " option ?arg arg ...?\"", (char *) nullptr);
    return TCL_ERROR;
  }

  if (strcmp(argv[1], "cget") == 0) {
    if (argc != 3) {
      Tcl_AppendResult(tcl_interp, "wrong # args: should be \"",
                     argv[0], " cget option\"", (char *) nullptr);
      return TCL_ERROR;
    }

    return Tk_ConfigureValue(tcl_interp, canvas->getTkWindow(),
                             CTclAppCanvasConfigSpecs, (char *) canvas->getConfigData(),
                             argv[2], 0);
  }

  if (strcmp(argv[1], "configure") == 0) {
    if (argc == 2)
      return Tk_ConfigureInfo(tcl_interp, canvas->getTkWindow(),
                              CTclAppCanvasConfigSpecs, (char *) canvas->getConfigData(),
                              (char *) nullptr, 0);

    if (argc == 3)
      return Tk_ConfigureInfo(tcl_interp, canvas->getTkWindow(),
                              CTclAppCanvasConfigSpecs, (char *) canvas->getConfigData(),
                              argv[2], 0);

    return CTclAppCanvasConfigureProc(tcl_interp, canvas, argc, argv,
                                     TK_CONFIG_ARGV_ONLY);
  }

  if (canvas->instanceCommand(argc - 1, argv + 1))
    return TCL_OK;

  Tcl_AppendResult(tcl_interp, "bad option \"", argv[1],
                   "\": must be cget, configure, position, or size",
                   (char *) nullptr);

  return TCL_ERROR;
}

static int
CTclAppCanvasConfigureProc(Tcl_Interp *tcl_interp, CTclAppCanvas *canvas,
                           int argc, const char **argv, int flags)
{
  if (Tk_ConfigureWidget(tcl_interp, canvas->getTkWindow(), CTclAppCanvasConfigSpecs,
                         argc - 2, argv + 2, (char *) canvas->getConfigData(), flags) != TCL_OK) {
    for (int j = 0; CTclAppCanvasConfigSpecs[j].type != TK_CONFIG_END; ++j) {
      for (int i = 2; i < argc; ++i) {
        if (! argv[i])
          continue;

        if (strcmp(CTclAppCanvasConfigSpecs[j].argvName, argv[i]) == 0) {
          argv[i    ] = nullptr;
          argv[i + 1] = nullptr;
          break;
        }
      }
    }

    int    argc1 = 0;
    char **argv1 = new char * [argc - 2];

    for (int i = 2; i < argc; ++i)
      if (argv[i])
        argv1[argc1++] = (char *) argv[i];

    if (! canvas->instanceConfigure(argc1, (const char **) argv1)) {
      delete [] argv1;
      return TCL_ERROR;
    }

    delete [] argv1;
  }

  canvas->setWidth (canvas->getConfigData()->width );
  canvas->setHeight(canvas->getConfigData()->height);

  CTclAppCanvasCalcGeometry(canvas);

  if (! canvas->getTkWindow() || ! Tk_IsMapped(canvas->getTkWindow()))
    return TCL_OK;

  if (! canvas->getRedrawPending()) {
    canvas->setRedrawPending(true);

    Tk_DoWhenIdle(CTclAppCanvasDisplayProc, (ClientData) canvas);
  }

  return TCL_OK;
}

static void
CTclAppCanvasCalcGeometry(CTclAppCanvas *canvas)
{
  Tk_GeometryRequest(canvas->getTkWindow(), canvas->getWidth(), canvas->getHeight());
}

static void
CTclAppCanvasDisplayProc(ClientData clientData)
{
  CTclAppCanvas *canvas = (CTclAppCanvas *) clientData;

  canvas->setRedrawPending(false);

  if (! canvas->getTkWindow() || ! Tk_IsMapped(canvas->getTkWindow()))
    return;

  int width  = Tk_Width (canvas->getTkWindow());
  int height = Tk_Height(canvas->getTkWindow());

  canvas->setWidth (width );
  canvas->setHeight(height);

  canvas->getConfigData()->width  = width;
  canvas->getConfigData()->height = height;

  canvas->redraw();
}

static void
CTclAppCanvasExposureProc(ClientData clientData, XEvent *event)
{
  CTclAppCanvas *canvas = (CTclAppCanvas *) clientData;

  if      (event->type == Expose && event->xexpose.count == 0) {
    if (! canvas->getRedrawPending()) {
      canvas->setRedrawPending(true);

      Tk_DoWhenIdle(CTclAppCanvasDisplayProc, (ClientData) canvas);
    }
  }
  else if (event->type == DestroyNotify) {
    Tcl_DeleteCommandFromToken(canvas->getTclInterp(), canvas->getTclInstanceCommand());

    canvas->setTkWindow(nullptr);

    if (canvas->getRedrawPending()) {
      Tk_CancelIdleCall(CTclAppCanvasDisplayProc, (ClientData) canvas);

      canvas->setRedrawPending(false);
    }

    Tk_EventuallyFree((ClientData) canvas, (Tcl_FreeProc *) CTclAppCanvasDestroyProc);

    return;
  }
}

static void
CTclAppCanvasButtonPressProc(ClientData clientData, XEvent *event)
{
  static Time         last_event_time   = 0;
  static Window       last_event_window = 0;
  static unsigned int last_event_button = 0;
  static int          multiclick_count  = 1;

  XButtonPressedEvent *event1 = (XButtonPressedEvent *) event;

  if (event1->window == last_event_window &&
      event1->button == last_event_button &&
      event1->time > last_event_time &&
      (event1->time - last_event_time) <= 400)
    multiclick_count++;
  else
    multiclick_count = 1;

  last_event_window = event1->window;
  last_event_button = event1->button;
  last_event_time   = event1->time;

  CTclAppCanvas *canvas = (CTclAppCanvas *) clientData;

  canvas->setModifiers((event1->state & ShiftMask  ),
                       (event1->state & ControlMask),
                       (event1->state & Mod1Mask   ));

  canvas->setMultiClick(multiclick_count);

  canvas->buttonPress(event1->button, event1->x, event1->y);
}

static void
CTclAppCanvasButtonMotionProc(ClientData clientData, XEvent *event)
{
  XMotionEvent *event1 = (XMotionEvent *) event;

  CTclAppCanvas *canvas = (CTclAppCanvas *) clientData;

  canvas->buttonMotion(event1->x, event1->y);
}

static void
CTclAppCanvasButtonReleaseProc(ClientData clientData, XEvent *event)
{
  XButtonPressedEvent *event1 = (XButtonReleasedEvent *) event;

  CTclAppCanvas *canvas = (CTclAppCanvas *) clientData;

  canvas->buttonRelease(event1->button, event1->x, event1->y);
}

static void
CTclAppCanvasKeyPressProc(ClientData clientData, XEvent *event)
{
  XKeyEvent *event1 = (XKeyEvent *) event;

  CTclAppCanvas *canvas = (CTclAppCanvas *) clientData;

  canvas->setModifiers((event1->state & ShiftMask  ),
                       (event1->state & ControlMask),
                       (event1->state & Mod1Mask   ));

  KeySym keysym;
  char   buffer[64];

  int len = XLookupString(event1, buffer, sizeof(buffer) - 1, &keysym, nullptr);

  buffer[len] = '\0';

//if (event1->state & ShiftMask)
//  keysym &= 0xCF;

  if (event1->state & ControlMask)
    keysym &= 0x9F;

  char *str = CXMachineInst->keysymToString(keysym);

  canvas->keyPress(keysym, str, event1->x, event1->y);
}

static void
CTclAppCanvasKeyReleaseProc(ClientData clientData, XEvent *event)
{
  XKeyEvent *event1 = (XKeyEvent *) event;

  CTclAppCanvas *canvas = (CTclAppCanvas *) clientData;

  canvas->setModifiers((event1->state & ShiftMask  ),
                       (event1->state & ControlMask),
                       (event1->state & Mod1Mask   ));

  canvas->keyRelease(event1->x, event1->y);
}

static void
CTclAppCanvasPointerMotionProc(ClientData clientData, XEvent *event)
{
//XMotionEvent *event1 = (XMotionEvent *) event;
}

static void
CTclAppCanvasButtonEnterLeaveProc(ClientData clientData, XEvent *event)
{
//XCrossingEvent *event1 = (XCrossingEvent *) event;
}

static void
CTclAppCanvasDestroyProc(ClientData clientData)
{
  CTclAppCanvas *canvas = (CTclAppCanvas *) clientData;

  if (canvas->getRedrawPending())
    Tk_CancelIdleCall(CTclAppCanvasDisplayProc, (ClientData) canvas);

  Tk_FreeOptions(CTclAppCanvasConfigSpecs, (char *) canvas->getConfigData(),
                 canvas->getDisplay(), 0);

  delete canvas;
}
