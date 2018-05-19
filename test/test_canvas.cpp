#include <tclApp.h>
#include <tclAppCanvas.h>

class testApp;

class testCanvas : public tclAppCanvas {
 public:
  testCanvas(testApp *app) :
   tclAppCanvas((tclApp *) app), app_(app), width_(300), height_(300) {
    init();
  }

  std::string getCommandName() const { return "testCanvas"; }
  std::string getCommandClass() const { return "TestCanvas"; }

  int getWidth() const { return width_; }
  int getHeight() const { return height_; }

  void setWidth(int width) { width_ = width; }
  void setHeight(int height) { height_ = height; }

  void redraw() { cerr << "Redraw" << endl; }

 private:
  testApp* app_    { nullptr };
  int      width_  { 0 };
  int      height_ { 0 };
};

//---

class testApp : public tclApp {
 public:
  testApp(int argc, char **argv) :
   tclApp(argc, argv) {
    init();
  }

 protected:
  void addCommands();

 private:
  testCanvas *canvas_ { nullptr };
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
  canvas_ = new testCanvas(this);
}
