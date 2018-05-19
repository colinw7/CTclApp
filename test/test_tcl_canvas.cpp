#include <CTclApp.h>
#include <CTclAppCanvas.h>
#include <CTclCanvas3D.h>
#include <test_tcl_canvas.k>

class testApp;

class testApp : public CTclApp {
 public:
  testApp(int argc, char **argv) :
   CTclApp(argc, argv) {
    init();
  }

 protected:
  void addCommands();

  std::string getTclStr();
};

//---

int
main(int argc, char **argv)
{
  testApp app(argc, argv);
}

void
testApp::
addCommands()
{
  new CTclCanvas3DCmd(this);
}

std::string
testApp::
getTclStr()
{
  return test_tcl_canvas_TK;
}
