#include <CTclAppI.h>
#include <CTclAppProcs.k>
#include <CFile.h>
#include <CStrUtil.h>
#include <CReadLine.h>
#include <iostream>
#include <cstring>

CTclApp *CTclApp::app_ = nullptr;

static Tk_ArgvInfo
argTable[] = {
  { (char *) "", TK_ARGV_END, },
};

CTclApp::
CTclApp(int argc, char **argv) :
 argc_(argc), argv_(argv), interactive_(false)
{
  char *p = strrchr(argv[0], '/');

  if (p)
    appName_ = p + 1;
  else
    appName_ = argv[0];
}

void
CTclApp::
setAppName(const std::string &appName)
{
  appName_ = appName;
}

void
CTclApp::
init()
{
  app_ = this;

  interp_ = Tcl_CreateInterp();

  if (Tk_ParseArgv(interp_, (Tk_Window) nullptr, &argc_, (const char **) argv_,
                   argTable, 0) != TCL_OK) {
    fprintf(stderr, "%s\n", Tcl_GetStringResult(interp_));
    exit(1);
  }

  Tcl_DeleteInterp(interp_);

  Tk_Main(argc_, argv_, &CTclApp::init_);

  app_ = nullptr;
}

int
CTclApp::
init_(Tcl_Interp *interp)
{
  app_->interp_ = interp;

  if (Tcl_Init(app_->interp_) == TCL_ERROR)
    return TCL_ERROR;

  if (Tk_Init(app_->interp_) == TCL_ERROR)
    return TCL_ERROR;

#ifdef USE_TIX
  if (Tix_Init(app_->interp_) == TCL_ERROR)
    return TCL_ERROR;
#endif

  app_->addCommands();

  Tcl_Eval(app_->interp_, CTclAppProcs_TK);

  std::string filename = app_->appName_ + ".tcl";

  if (CFile::exists(filename)) {
    std::cerr << "Using " << filename << std::endl;

    std::string cmd = "source \"" + filename + "\"";

    Tcl_Eval(app_->interp_, (char *) cmd.c_str());
  }
  else
    Tcl_Eval(app_->interp_, (char *) app_->getTclStr().c_str());

  app_->startup();

  app_->mainLoop();

  exit(0);

  return TCL_OK;
}

void
CTclApp::
mainLoop()
{
  if (app_->interactive_)
    tkInteractiveMainLoop();
  else
    tkMainLoop();
}

void
CTclApp::
tkMainLoop()
{
  Tk_MainLoop();
}

void
CTclApp::
tkInteractiveMainLoop()
{
  CReadLine readline;

  readline.setAutoHistory(true);

  std::string line;

  while (true) {
    while (Tcl_DoOneEvent(TCL_DONT_WAIT));

    readline.setPrompt("> ");

    line = readline.readLine();

    Tcl_Eval(app_->interp_, line.c_str());
  }
}

void
CTclApp::
tkIdleMainLoop()
{
  for (;;) {
    while (Tcl_DoOneEvent(TCL_DONT_WAIT));

    app_->idle();
  }
}

Display *
CTclApp::
getDisplay()
{
  return Tk_Display(Tk_MainWindow(interp_));
}

void
CTclApp::
setIntegerVar(const std::string &var, int value)
{
  setStringVar(var, CStrUtil::toString(value));
}

void
CTclApp::
setRealVar(const std::string &var, double value)
{
  setStringVar(var, CStrUtil::toString(value));
}

void
CTclApp::
setStringVar(const std::string &var, const std::string &value)
{
  Tcl_SetVar(app_->interp_, (char *) var.c_str(),
             (char *) value.c_str(), TCL_GLOBAL_ONLY);
}

int
CTclApp::
getIntegerVar(const std::string &var)
{
  return CStrUtil::toInteger(getStringVar(var));
}

double
CTclApp::
getRealVar(const std::string &var)
{
  return CStrUtil::toReal(getStringVar(var));
}

std::string
CTclApp::
getStringVar(const std::string &var)
{
  const char *value = Tcl_GetVar(app_->interp_, (char *) var.c_str(), TCL_GLOBAL_ONLY);

  if (! value)
    return "";

  return value;
}

void
CTclApp::
eval(const char *format, ...)
{
  va_list vargs;

  va_start(vargs, format);

  std::string str;

  CStrUtil::vsprintf(str, format, &vargs);

  Tcl_Eval(app_->interp_, (char *) str.c_str());

  va_end(vargs);
}
