#include <CTclApp.h>
#include <CTclAppCanvas.h>
#include <CTclCanvas2D.h>
#include <test_tcl_image.k>

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
  new CTclCanvas2DCmd(this);
}

std::string
testApp::
getTclStr()
{
  return test_tcl_image_TK;
}
