#ifndef CTCL_APP_H
#define CTCL_APP_H

#include <string>
#include <vector>
#include <set>
#include <map>

using ClientData = void *;

struct Tcl_Interp;
struct Tcl_Obj;

#if 0
struct Display;
#else
typedef struct _XDisplay Display;
#endif

using CTclNonConstProc = int (*)(ClientData clientData, Tcl_Interp *interp,
                                 int argc, char **argv);
using CTclConstProc    = int (*)(ClientData clientData, Tcl_Interp *interp,
                                 int argc, const char **argv);

//---

class CTclApp;

class CTclTraceProc {
 public:
  explicit CTclTraceProc(CTclApp *app) : app_(app) { }

  virtual ~CTclTraceProc() = default;

  virtual void handleRead (const char *) { }
  virtual void handleWrite(const char *) { }

  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b) { enabled_ = b; }

 protected:
  CTclApp *app_     { nullptr };
  bool     enabled_ { true };
};

//---

class CTclApp {
 public:
  Tcl_Interp *getInterp() { return interp_; }

  Display *getDisplay();

  bool isInteractive() const { return interactive_; }
  void setInteractive(bool interactive) { interactive_ = interactive; }

  virtual void mainLoop();

  virtual void idle() { }

  void interactiveMainLoop();

  void tkMainLoop();
  void tkIdleMainLoop();

  //---

  bool hasResult() const { return resultSet_; }

  void setIntegerResult(int value);
  void setRealResult   (double value);
  void setStringResult (const std::string &value);

  void setIntegerArrayResult(int *values, int num_values);
  void setIntegerArrayResult(const std::vector<int> &values);
  void setRealArrayResult   (double *values, int num_values);
  void setRealArrayResult   (const std::vector<double> &values);
  void setStringArrayResult (char **values, int num_values);
  void setStringArrayResult (const std::vector<std::string> &strs);

  void setObjResult(Tcl_Obj *obj);

  //---

  std::string getStringResult() const;

  //---

  void setIntegerVar(const std::string &var, int value);
  void setRealVar   (const std::string &var, double value);
  void setStringVar (const std::string &var, const std::string &value);
  void setBoolVar   (const std::string &var, bool value);

  void setStringArrayVar (const std::string &var, const std::vector<std::string> &strs);
  void setIntegerArrayVar(const std::string &var, const std::vector<int> &strs);

  //---

  bool hasVar(const std::string &var) const;

  int         getIntegerVar(const std::string &var) const;
  double      getRealVar   (const std::string &var) const;
  std::string getStringVar (const std::string &var) const;
  bool        getBoolVar   (const std::string &var) const;

  bool getStringArrayVar(const std::string &var, std::vector<std::string> &strs) const;

  //---

  bool splitList(const std::string &str, std::vector<std::string> &strs) const;

  std::string mergeList(const std::vector<std::string> &strs) const;

  //---

  void traceVar  (const std::string &name);
  void traceVar  (const std::string &name, CTclTraceProc *proc);
  void untraceVar(const std::string &name);
  void untraceVar(const std::string &name, CTclTraceProc *proc);

  void handleTrace(const char *name, int flags);

  static char *traceProc(ClientData data, Tcl_Interp *,
                         const char *name1, const char *, int flags);
  static char *genTraceProc(ClientData data, Tcl_Interp *,
                            const char *name1, const char *, int flags);

  virtual void handleRead(const char *name);
  virtual void handleWrite(const char *name);
#if 0
  virtual void handleUnset(const char *name);
#endif

  //---

  bool evalFile(const std::string &filename) const;

  bool eval(const char *format, ...) const;

  bool eval(const std::string &str) const;

 protected:
  explicit CTclApp(int argc, const char **argv);

  virtual ~CTclApp() { }

  void setAppName(const std::string &appName);

  bool tclInit();

  void tkInit();

  void init();

  void initInterp();

  virtual void addCommands() = 0;

  virtual void startup() { }

  virtual std::string getTclStr() = 0;

 private:
  static int tkInit_(Tcl_Interp *interp);

  void initCommands();
  void initInteractive();

  std::string errorInfo_(int rc) const;

 private:
  using TraceNames     = std::set<std::string>;
  using TraceProcs     = std::set<CTclTraceProc *>;
  using NameTraceProcs = std::map<std::string, TraceProcs>;

  static CTclApp *app_;

  int          argc_        { 0 };
  const char** argv_        { nullptr };
  Tcl_Interp*  interp_      { nullptr };
  std::string  appName_;
  bool         interactive_ { true };
  bool         isTk_        { false };

  bool resultSet_ { false };

  TraceNames     traces_;
  NameTraceProcs traceProcs_;
};

#endif
