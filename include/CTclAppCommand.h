#ifndef CTclAppCommand_H
#define CTclAppCommand_H

class CTclApp;

#include <vector>
#include <string>

using ClientData = void *;

struct Tcl_Interp;
struct Tcl_Obj;

class CTclAppCommand {
 protected:
  CTclAppCommand(CTclApp *app, const std::string &name);

  virtual ~CTclAppCommand();

  const std::string &getName() const { return name_; }

  virtual bool proc(int argc, const char **argv) = 0;

  //---

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

  void setIntegerVar(const std::string &var, int value);
  void setRealVar   (const std::string &var, double value);
  void setStringVar (const std::string &var, const std::string &value);
  void setBoolVar   (const std::string &var, bool value);

  int         getIntegerVar(const std::string &var) const;
  double      getRealVar   (const std::string &var) const;
  std::string getStringVar (const std::string &var) const;
  bool        getBoolVar   (const std::string &var) const;

  bool getStringArrayVar(const std::string &var, std::vector<std::string> &strs) const;

 private:
  static int commandProc(ClientData clientData, Tcl_Interp *interp, int argc, const char **argv);

 protected:
  CTclApp*    app_ { nullptr };
  std::string name_;
};

#endif
