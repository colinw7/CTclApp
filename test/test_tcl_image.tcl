proc createGUI { filename } {
  frame .menubar -relief raised -borderwidth 2

  menubutton .menubar.file -text "File" -menu .menubar.file.menu
  menubutton .menubar.help -text "Help" -menu .menubar.help.menu

  pack .menubar.file -side left
  pack .menubar.help -side right

  menu .menubar.file.menu
  menu .menubar.help.menu

  .menubar.file.menu add command -label "Load" -command { loadImage }
  .menubar.file.menu add command -label "Save" -command { saveImage }
  .menubar.file.menu add command -label "Exit" -command { exit }

  .menubar.help.menu add command -label "Help" -command { help }

  pack .menubar -side top -fill x

  #----

  frame .frame

  #----

  global canvas

  set canvas .frame.canvas

  CTclCanvas2D $canvas -width 300 -height 300

  if {$filename != ""} {
    $canvas loadImage $filename
  }

  pack .frame.canvas -side top -anchor w -fill both -expand true

  #----

  pack .frame -side left -fill both -expand true
}

proc loadImage { } {
  set types {
    {{BMP Files} {.bmp} }
    {{GIF Files} {.gif} }
    {{IFF Files} {.iff} }
    {{JPG Files} {.jpg} }
    {{PCX Files} {.pcx} }
    {{PNG Files} {.png} }
    {{SGI Files} {.sgi} }
    {{TIF Files} {.tif} }
    {{XBM Files} {.xbm} }
    {{XPM Files} {.xpm} }
    {{XWD Files} {.xwd} }
    {{All Files} *      }
  }

  set filename [tk_getOpenFile -filetypes $types]

  if {$filename != ""} {
    global canvas

    $canvas loadImage $filename
  }
}

proc saveImage { } {
  set types {
    {{BMP Files} {.bmp} }
    {{EPS Files} {.eps} }
    {{GIF Files} {.gif} }
    {{JPG Files} {.jpg} }
    {{PNG Files} {.png} }
    {{PS  Files} {.ps} }
    {{TIF Files} {.tif} }
    {{XBM Files} {.xbm} }
    {{XPM Files} {.xpm} }
    {{XWD Files} {.xwd} }
    {{All Files} *      }
  }

  set filename [tk_getSaveFile -filetypes $types]

  if {$filename != ""} {
    global canvas

    $canvas saveImage $filename
  }
}

createGUI $argv0
