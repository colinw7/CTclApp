#include <CTclAppI.h>
#include <CTclAppProcs.k>

#include <CFile.h>
#include <CStrUtil.h>
#include <CReadLine.h>

#include <iostream>
#include <cstring>
#include <cassert>

extern "C" {
#include <tcl.h>
#include <tk.h>
#include <tix.h>
}

CTclApp *CTclApp::app_ = nullptr;

static Tk_ArgvInfo
argTable[] = {
  { "", TK_ARGV_END, },
};

CTclApp::
CTclApp(int argc, const char **argv) :
 argc_(argc), argv_(argv)
{
  char *p = strrchr(const_cast<char *>(argv[0]), '/');

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
tkInit()
{
  isTk_ = true;

  initInterp();

  if (Tk_ParseArgv(interp_, (Tk_Window) nullptr, &argc_, (const char **) argv_,
                   argTable, 0) != TCL_OK) {
    std::cerr << getStringResult() << "\n";
    exit(1);
  }

  Tcl_DeleteInterp(interp_);

  app_ = this;

  Tk_Main(argc_, const_cast<char **>(argv_), &CTclApp::tkInit_);

  app_ = nullptr;
}

bool
CTclApp::
tclInit()
{
  initInterp();

  app_ = this;

  if (Tcl_Init(app_->interp_) == TCL_ERROR)
    return false;

  return true;
}

void
CTclApp::
init()
{
  app_ = this;

  initInterp();

  initCommands();

  initInteractive();
}

int
CTclApp::
tkInit_(Tcl_Interp *interp)
{
  assert(app_->isTk_);

  app_->setInteractive(false);

  app_->interp_ = interp;

  if (Tcl_Init(app_->interp_) == TCL_ERROR)
    return TCL_ERROR;

  if (Tk_Init(app_->interp_) == TCL_ERROR)
    return TCL_ERROR;

#ifdef USE_TIX
  if (Tix_Init(app_->interp_) == TCL_ERROR)
    return TCL_ERROR;
#endif

  app_->initCommands();

  (void) app_->eval(CTclAppProcs_TK);

  auto filename = app_->appName_ + ".tcl";

  if (CFile::exists(filename)) {
    std::cerr << "Using " << filename << std::endl;

    (void) app_->evalFile(filename);
  }
  else {
    (void) app_->eval(app_->getTclStr());
  }

  app_->initInteractive();

  exit(0);

  return TCL_OK;
}

void
CTclApp::
initInterp()
{
  if (! interp_)
    interp_ = Tcl_CreateInterp();
}

void
CTclApp::
initCommands()
{
  addCommands();
}

void
CTclApp::
initInteractive()
{
  app_->startup();

  app_->mainLoop();
}

void
CTclApp::
mainLoop()
{
  if (isInteractive())
    interactiveMainLoop();
  else if (isTk_)
    tkMainLoop();
}

void
CTclApp::
interactiveMainLoop()
{
  CReadLine readline;

  readline.setAutoHistory(true);

  std::string line;

  while (true) {
    while (Tcl_DoOneEvent(TCL_DONT_WAIT));

    readline.setPrompt("> ");

    line = readline.readLine();

    (void) eval(line);
  }
}

void
CTclApp::
tkMainLoop()
{
  assert(isTk_);

  Tk_MainLoop();
}

void
CTclApp::
tkIdleMainLoop()
{
  assert(isTk_);

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

//---

void
CTclApp::
setIntegerResult(int value)
{
  setStringResult(CStrUtil::toString(value));
}

void
CTclApp::
setRealResult(double value)
{
  setStringResult(CStrUtil::toString(value));
}

void
CTclApp::
setStringResult(const std::string &value)
{
  auto *sobj = Tcl_NewStringObj(value.c_str(), int(value.size()));

  setObjResult(sobj);
}

void
CTclApp::
setIntegerArrayResult(int *values, int num_values)
{
  std::vector<std::string> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setIntegerArrayResult(const std::vector<int> &values)
{
  std::vector<std::string> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setRealArrayResult(double *values, int num_values)
{
  std::vector<std::string> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setRealArrayResult(const std::vector<double> &values)
{
  std::vector<std::string> strs;

  int num_values = values.size();

  for (int i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayResult(strs);
}

void
CTclApp::
setStringArrayResult(char **values, int num_values)
{
  std::vector<std::string> strs;

  for (int i = 0; i < num_values; ++i)
    strs.push_back(values[i]);

  setStringArrayResult(strs);
}

void
CTclApp::
setStringArrayResult(const std::vector<std::string> &values)
{
  auto *obj = Tcl_NewListObj(0, nullptr);

  int num_values = values.size();

  for (int i = 0; i < num_values; ++i) {
    auto *sobj = Tcl_NewStringObj(values[i].c_str(), int(values[i].size()));

    Tcl_ListObjAppendElement(app_->getInterp(), obj, sobj);
  }

  setObjResult(obj);
}

void
CTclApp::
setObjResult(Tcl_Obj *obj)
{
  if (obj) {
    resultSet_ = true;

    Tcl_SetObjResult(app_->getInterp(), obj);
  }
  else
    resultSet_ = false;
}

//---

std::string
CTclApp::
getStringResult() const
{
  auto *res = Tcl_GetStringResult(app_->getInterp());

  return std::string(res);
}

//---

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
  Tcl_SetVar(app_->interp_, var.c_str(), value.c_str(), TCL_GLOBAL_ONLY);
}

void
CTclApp::
setBoolVar(const std::string &name, bool b)
{
  setStringVar(name, b ? "1" : "0");
}

void
CTclApp::
setStringArrayVar(const std::string &var, const std::vector<std::string> &strs)
{
  setStringVar(var, mergeList(strs));
}

void
CTclApp::
setIntegerArrayVar(const std::string &var, const std::vector<int> &values)
{
  std::vector<std::string> strs;

  auto num_values = values.size();

  for (uint i = 0; i < num_values; ++i)
    strs.push_back(CStrUtil::toString(values[i]));

  setStringArrayVar(var, strs);
}

//---

bool
CTclApp::
hasVar(const std::string &var) const
{
  const char *value = Tcl_GetVar(app_->interp_, var.c_str(), TCL_GLOBAL_ONLY);

  return value;
}

int
CTclApp::
getIntegerVar(const std::string &var) const
{
  return CStrUtil::toInteger(getStringVar(var));
}

double
CTclApp::
getRealVar(const std::string &var) const
{
  return CStrUtil::toReal(getStringVar(var));
}

std::string
CTclApp::
getStringVar(const std::string &var) const
{
  const char *value = Tcl_GetVar(app_->interp_, var.c_str(), TCL_GLOBAL_ONLY);
  if (! value) return "";

  return value;
}

bool
CTclApp::
getBoolVar(const std::string &var) const
{
  bool b;

  if (! CStrUtil::toBool(getStringVar(var), &b))
    b = false;

  return b;
}

bool
CTclApp::
getStringArrayVar(const std::string &var, std::vector<std::string> &strs) const
{
  const char *value = Tcl_GetVar(app_->interp_, var.c_str(), TCL_GLOBAL_ONLY);
  if (! value) return false;

  int    argc;
  char **argv;

  int rc = Tcl_SplitList(app_->interp_, value, &argc, const_cast<const char ***>(&argv));

  if (rc != TCL_OK) {
    std::cerr << errorInfo_(rc) << "\n";
    return false;
  }

  for (int i = 0; i < argc; ++i)
    strs.push_back(std::string(argv[i]));

  Tcl_Free(reinterpret_cast<char *>(argv));

  return true;
}

//---

bool
CTclApp::
splitList(const std::string &str, std::vector<std::string> &strs) const
{
  int    argc;
  char **argv;

  int rc = Tcl_SplitList(app_->interp_, str.c_str(), &argc, const_cast<const char ***>(&argv));
  if (rc != TCL_OK) return false;

  for (int i = 0; i < argc; ++i)
    strs.push_back(std::string(argv[i]));

  Tcl_Free(reinterpret_cast<char *>(argv));

  return true;
}

std::string
CTclApp::
mergeList(const std::vector<std::string> &strs) const
{
  auto argc = strs.size();

  std::vector<char *> argv;

  argv.resize(argc);

  for (size_t i = 0; i < argc; ++i)
    argv[i] = strdup(strs[i].c_str());

  char *res = Tcl_Merge(int(argc), &argv[0]);

  std::string str(res);

  for (size_t i = 0; i < argc; ++i)
    free(argv[i]);

  Tcl_Free(res);

  return str;
}

//---

void
CTclApp::
traceVar(const std::string &name, CTclTraceProc *proc)
{
  auto name1 = name;

  auto pos = name1.find('(');

  if (pos != std::string::npos)
    name1 = name1.substr(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn != traceProcs_.end()) {
    auto pp = (*pn).second.find(proc);

    if (pp != (*pn).second.end()) {
      std::cerr << "Multiple traces on same var '" << name1 << "'\n";
      return;
    }
  }

  traceProcs_[name1].insert(proc);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

  auto data = Tcl_VarTraceInfo(interp_, name1.c_str(), flags, &traceProc, nullptr);

  if (! data)
    Tcl_TraceVar(interp_, name1.c_str(), flags, &traceProc, static_cast<ClientData>(this));
}

void
CTclApp::
traceVar(const std::string &name)
{
  auto name1 = name;

  auto pos = name1.find('(');

  if (pos != std::string::npos)
    name1 = name1.substr(0, pos);

  //---

  auto pn = traces_.find(name1);

  if (pn != traces_.end()) {
    std::cerr << "Multiple traces on same var '" << name1 << "'\n";
    return;
  }

  traces_.insert(name1);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

  auto data = Tcl_VarTraceInfo(interp_, name1.c_str(), flags, &genTraceProc, nullptr);

  if (! data)
    Tcl_TraceVar(interp_, name1.c_str(), flags, &genTraceProc, static_cast<ClientData>(this));
}

void
CTclApp::
untraceVar(const std::string &name, CTclTraceProc *proc)
{
  auto name1 = name;

  auto pos = name1.find('(');

  if (pos != std::string::npos)
    name1 = name1.substr(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn == traceProcs_.end()) {
    std::cerr << "No trace on var '" << name1 << "'\n";
    return;
  }

  auto pp = (*pn).second.find(proc);

  if (pp == (*pn).second.end()) {
    std::cerr << "No trace on var '" << name1 << "'\n";
    return;
  }

  (*pn).second.erase(pp);

  //---

  if ((*pn).second.empty()) {
    int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

    Tcl_UntraceVar(interp_, name1.c_str(), flags, &traceProc, static_cast<ClientData>(this));
  }
}

void
CTclApp::
untraceVar(const std::string &name)
{
  auto name1 = name;

  auto pos = name1.find('(');

  if (pos != std::string::npos)
    name1 = name1.substr(0, pos);

  //---

  auto pn = traceProcs_.find(name1);

  if (pn == traceProcs_.end()) {
    std::cerr << "No trace on var '" << name1 << "'\n";
    return;
  }

  traces_.erase(name1);

  //---

  int flags = TCL_TRACE_READS | TCL_TRACE_WRITES | TCL_TRACE_UNSETS | TCL_GLOBAL_ONLY;

  Tcl_UntraceVar(interp_, name1.c_str(), flags, &genTraceProc, static_cast<ClientData>(this));
}

void
CTclApp::
handleTrace(const char *name, int flags)
{
  // ignore unset called on trace destruction
  if (flags & TCL_TRACE_UNSETS) return;

  bool handled = false;

  if (flags & TCL_TRACE_READS ) { handleRead (name); handled = true; }
  if (flags & TCL_TRACE_WRITES) { handleWrite(name); handled = true; }
//if (flags & TCL_TRACE_UNSETS) { handleUnset(name); handled = true; }

  assert(handled);
}

char *
CTclApp::
traceProc(ClientData data, Tcl_Interp *, const char *name1, const char *, int flags)
{
  // ignore unset called on trace destruction
  if (flags & TCL_TRACE_UNSETS) return nullptr;

  auto *th = static_cast<CTclApp *>(data);
  assert(th);

  auto pn = th->traceProcs_.find(name1);
  //assert(pn != th->traceProcs_.end());
  if (pn == th->traceProcs_.end()) {
    std::cerr << "Failed to find trace proc '" << name1 << "'\n";
    return nullptr;
  }

  for (auto *proc : (*pn).second) {
    if (! proc->isEnabled())
      continue;

    bool handled = false;

    if (flags & TCL_TRACE_READS ) { proc->handleRead (name1); handled = true; }
    if (flags & TCL_TRACE_WRITES) { proc->handleWrite(name1); handled = true; }
  //if (flags & TCL_TRACE_UNSETS) { proc->handleUnset(name1); handled = true; }

    assert(handled);
  }

  return nullptr;
}

char *
CTclApp::
genTraceProc(ClientData data, Tcl_Interp *, const char *name1, const char *, int flags)
{
  auto *th = static_cast<CTclApp *>(data);
  assert(th);

  th->handleTrace(name1, flags);

  return nullptr;
}

void
CTclApp::
handleRead(const char *name)
{
  std::cerr << "CTclApp::handleRead " << name << "\n";
}

void
CTclApp::
handleWrite(const char *name)
{
  std::cerr << "CTclApp::handleWrite " << name << "\n";
}

#if 0
void
CTclApp::
handleUnset(const char *name)
{
  std::cerr << "CTcl::handleUnset " << name << "\n";
}
#endif

//---

bool
CTclApp::
evalFile(const std::string &filename) const
{
#if 0
  std::string cmd = "source \"" + filename + "\"";

  return eval(cmd);
#endif

  auto rc = Tcl_EvalFile(interp_, filename.c_str());

  if (rc != TCL_OK)
    std::cerr << errorInfo_(rc) << "\n";

  return (rc == TCL_OK);
}

bool
CTclApp::
eval(const std::string &str) const
{
  auto rc = Tcl_Eval(interp_, str.c_str());

  if (rc != TCL_OK)
    std::cerr << errorInfo_(rc) << "\n";

  return (rc == TCL_OK);
}

bool
CTclApp::
eval(const char *format, ...) const
{
  va_list vargs;

  va_start(vargs, format);

  std::string str;

  CStrUtil::vsprintf(str, format, &vargs);

  auto rc = eval(str);

  va_end(vargs);

  return rc;
}

//---

std::string
CTclApp::
errorInfo_(int rc) const
{
  auto stringFromObj = [](Tcl_Obj *obj) {
    int len = 0;

    char *str = Tcl_GetStringFromObj(obj, &len);

    return std::string(str, size_t(len));
  };

  auto *options = Tcl_GetReturnOptions(interp_, rc);

  auto *key1 = Tcl_NewStringObj("-errorcode", -1);
  auto *key2 = Tcl_NewStringObj("-errorinfo", -1);

  Tcl_Obj *errorMsg;
  Tcl_IncrRefCount(key1);
  Tcl_DictObjGet(nullptr, options, key1, &errorMsg);
  Tcl_DecrRefCount(key1);

  auto msg = stringFromObj(errorMsg);

  Tcl_Obj *stackTrace;
  Tcl_IncrRefCount(key2);
  Tcl_DictObjGet(nullptr, options, key2, &stackTrace);
  Tcl_DecrRefCount(key2);

  auto trace = stringFromObj(stackTrace);

  Tcl_DecrRefCount(options);

  return msg + "\n" + trace;
}
