static char test_tcl_canvas_TK[] = "\n\
proc createGUI { } {\n\
  frame .left_frame\n\
  frame .right_frame\n\
  global canvas\n\
  set canvas .left_frame.canvas\n\
  CTclCanvas3D $canvas -width 300 -height 300\n\
  pack .left_frame.canvas -side top -anchor w -fill both -expand true\n\
  radiobutton .right_frame.box      -text \"Box\" \\\n\
    -variable shape_type -command SelectShape -value box\n\
  radiobutton .right_frame.cone     -text \"Cone\" \\\n\
    -variable shape_type -command SelectShape -value cone\n\
  radiobutton .right_frame.cube     -text \"Cube\" \\\n\
    -variable shape_type -command SelectShape -value cube\n\
  radiobutton .right_frame.cylinder -text \"Cylinder\" \\\n\
    -variable shape_type -command SelectShape -value cylinder\n\
  radiobutton .right_frame.pyramid  -text \"Pyramid\" \\\n\
    -variable shape_type -command SelectShape -value pyramid\n\
  radiobutton .right_frame.sphere   -text \"Sphere\" \\\n\
    -variable shape_type -command SelectShape -value sphere\n\
  radiobutton .right_frame.torus    -text \"Torus\" \\\n\
    -variable shape_type -command SelectShape -value torus\n\
  pack .right_frame.box \\\n\
       .right_frame.cone \\\n\
       .right_frame.cube \\\n\
       .right_frame.cylinder \\\n\
       .right_frame.pyramid \\\n\
       .right_frame.sphere \\\n\
       .right_frame.torus \\\n\
       -side top -anchor w\n\
  global shape_type\n\
  set shape_type box\n\
  SelectShape\n\
  radiobutton .right_frame.solid     -text \"Solid\" \\\n\
    -variable draw_mode -command SelectDrawType -value solid\n\
  radiobutton .right_frame.wireframe -text \"Wireframe\" \\\n\
    -variable draw_mode -command SelectDrawType -value wireframe\n\
  pack .right_frame.solid \\\n\
       .right_frame.wireframe \\\n\
       -side top -anchor w\n\
  global draw_mode\n\
  set draw_mode solid\n\
  SelectDrawMode\n\
  radiobutton .right_frame.flat    -text \"Flat\" \\\n\
    -variable draw_type -command SelectDrawType -value flat\n\
  radiobutton .right_frame.gouraud -text \"Gouraud\" \\\n\
    -variable draw_type -command SelectDrawType -value gouraud\n\
  radiobutton .right_frame.phong   -text \"Phong\" \\\n\
    -variable draw_type -command SelectDrawType -value phong\n\
  pack .right_frame.flat \\\n\
       .right_frame.gouraud \\\n\
       .right_frame.phong \\\n\
       -side top -anchor w\n\
  global draw_type\n\
  set draw_type flat\n\
  SelectDrawType\n\
  button .right_frame.quit -text \"Quit\" -command QuitCmd\n\
  pack .right_frame.quit\n\
  pack .left_frame -side left -fill both -expand true\n\
  pack .right_frame -side left -fill y\n\
}\n\
proc SelectShape { } {\n\
  global canvas\n\
  global shape_type\n\
  $canvas delete all\n\
  if     {$shape_type == \"box\"     } {\n\
    $canvas create box\n\
  } \\\n\
  elseif {$shape_type == \"cone\"    } {\n\
    $canvas create cone\n\
  } \\\n\
  elseif {$shape_type == \"cube\"    } {\n\
    $canvas create cube\n\
  } \\\n\
  elseif {$shape_type == \"cylinder\"} {\n\
    $canvas create cylinder\n\
  } \\\n\
  elseif {$shape_type == \"pyramid\" } {\n\
    $canvas create pyramid\n\
  } \\\n\
  elseif {$shape_type == \"sphere\"  } {\n\
    $canvas create sphere\n\
  } \\\n\
  elseif {$shape_type == \"torus\"   } {\n\
    $canvas create torus\n\
  }\n\
}\n\
proc SelectDrawMode { } {\n\
  global canvas\n\
  global draw_mode\n\
  if     {$draw_mode == \"flat\"} {\n\
    $canvas configure -drawmode flat\n\
  } \\\n\
  elseif {$draw_mode == \"wireframe\" } {\n\
    $canvas configure -drawmode wireframe\n\
  }\n\
}\n\
proc SelectDrawType { } {\n\
  global canvas\n\
  global draw_type\n\
  if     {$draw_type == \"solid\"} {\n\
    $canvas configure -fillmode solid\n\
  } \\\n\
  elseif {$draw_type == \"gouraud\"   } {\n\
    $canvas configure -fillmode gouraud\n\
  } \\\n\
  elseif {$draw_type == \"phong\"     } {\n\
    $canvas configure -fillmode phong\n\
  }\n\
}\n\
proc QuitCmd { } {\n\
  exit\n\
}\n\
createGUI\n\
";

