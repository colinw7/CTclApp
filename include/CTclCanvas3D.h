#ifndef CTclCanvas3D_H
#define CTclCanvas3D_H

#include <CTclAppCanvas.h>

class CTclCanvas3D;
class CCanvas3D;

class CTclCanvas3DCmd : public CTclAppCanvasCmd {
 public:
  CTclCanvas3DCmd(CTclApp *app);

  virtual ~CTclCanvas3DCmd() { }

  virtual std::string getCommandName() const { return "CTclCanvas3D"; }
  virtual std::string getCommandClass() const { return "CTclCanvas3D"; }

  CTclAppCanvas *createInstance(CTclApp *app);

  CCanvas3D *getCanvas3D();

  virtual void initCanvas() { }

 private:
  CTclCanvas3D *canvas_ { nullptr };
};

#endif
