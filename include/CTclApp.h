#ifndef CTCL_APP_H
#define CTCL_APP_H

#include <string>

extern "C" {
#include <tcl.h>
#include <tk.h>
#include <tix.h>
}

typedef int (*CTclNonConstProc)(ClientData clientData, Tcl_Interp *interp,
                                int argc, char **argv);
typedef int (*CTclConstProc)(ClientData clientData, Tcl_Interp *interp,
                             int argc, const char **argv);

class CTclApp {
 public:
  Tcl_Interp *getInterp() { return interp_; }

  Display *getDisplay();

  void setInteractive(bool interactive) { interactive_ =interactive; }

  virtual void mainLoop();

  virtual void idle() { }

  void tkMainLoop();
  void tkInteractiveMainLoop();
  void tkIdleMainLoop();

  void setIntegerVar(const std::string &var, int value);
  void setRealVar(const std::string &var, double value);
  void setStringVar(const std::string &var, const std::string &value);

  int         getIntegerVar(const std::string &var);
  double      getRealVar(const std::string &var);
  std::string getStringVar(const std::string &var);

  void eval(const char *format, ...);

 protected:
  CTclApp(int argc, char **argv);

  virtual ~CTclApp() { }

  void setAppName(const std::string &appName);

  void init();

  virtual void addCommands() = 0;

  virtual void startup() { }

  virtual std::string getTclStr() = 0;

 private:
  static int init_(Tcl_Interp *interp);

 private:
  static CTclApp *app_;

  int          argc_        { 0 };
  char**       argv_        { nullptr };
  Tcl_Interp*  interp_      { nullptr };
  std::string  appName_;
  bool         interactive_ { false };
};

#endif
