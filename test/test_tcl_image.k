static char test_tcl_image_TK[] = "\n\
proc createGUI { filename } {\n\
  frame .menubar -relief raised -borderwidth 2\n\
  menubutton .menubar.file -text \"File\" -menu .menubar.file.menu\n\
  menubutton .menubar.help -text \"Help\" -menu .menubar.help.menu\n\
  pack .menubar.file -side left\n\
  pack .menubar.help -side right\n\
  menu .menubar.file.menu\n\
  menu .menubar.help.menu\n\
  .menubar.file.menu add command -label \"Load\" -command { loadImage }\n\
  .menubar.file.menu add command -label \"Save\" -command { saveImage }\n\
  .menubar.file.menu add command -label \"Exit\" -command { exit }\n\
  .menubar.help.menu add command -label \"Help\" -command { help }\n\
  pack .menubar -side top -fill x\n\
  frame .frame\n\
  global canvas\n\
  set canvas .frame.canvas\n\
  CTclCanvas2D $canvas -width 300 -height 300\n\
  if {$filename != \"\"} {\n\
    $canvas loadImage $filename\n\
  }\n\
  pack .frame.canvas -side top -anchor w -fill both -expand true\n\
  pack .frame -side left -fill both -expand true\n\
}\n\
proc loadImage { } {\n\
  set types {\n\
    {{BMP Files} {.bmp} }\n\
    {{GIF Files} {.gif} }\n\
    {{IFF Files} {.iff} }\n\
    {{JPG Files} {.jpg} }\n\
    {{PCX Files} {.pcx} }\n\
    {{PNG Files} {.png} }\n\
    {{SGI Files} {.sgi} }\n\
    {{TIF Files} {.tif} }\n\
    {{XBM Files} {.xbm} }\n\
    {{XPM Files} {.xpm} }\n\
    {{XWD Files} {.xwd} }\n\
    {{All Files} *      }\n\
  }\n\
  set filename [tk_getOpenFile -filetypes $types]\n\
  if {$filename != \"\"} {\n\
    global canvas\n\
    $canvas loadImage $filename\n\
  }\n\
}\n\
proc saveImage { } {\n\
  set types {\n\
    {{BMP Files} {.bmp} }\n\
    {{EPS Files} {.eps} }\n\
    {{GIF Files} {.gif} }\n\
    {{JPG Files} {.jpg} }\n\
    {{PNG Files} {.png} }\n\
    {{PS  Files} {.ps} }\n\
    {{TIF Files} {.tif} }\n\
    {{XBM Files} {.xbm} }\n\
    {{XPM Files} {.xpm} }\n\
    {{XWD Files} {.xwd} }\n\
    {{All Files} *      }\n\
  }\n\
  set filename [tk_getSaveFile -filetypes $types]\n\
  if {$filename != \"\"} {\n\
    global canvas\n\
    $canvas saveImage $filename\n\
  }\n\
}\n\
createGUI $argv0\n\
";

