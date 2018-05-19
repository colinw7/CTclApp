proc createGUI { } {
  frame .left_frame
  frame .right_frame

  #----

  global canvas

  set canvas .left_frame.canvas

  CTclCanvas3D $canvas -width 300 -height 300

  pack .left_frame.canvas -side top -anchor w -fill both -expand true

  #----

  radiobutton .right_frame.box      -text "Box" \
    -variable shape_type -command SelectShape -value box
  radiobutton .right_frame.cone     -text "Cone" \
    -variable shape_type -command SelectShape -value cone
  radiobutton .right_frame.cube     -text "Cube" \
    -variable shape_type -command SelectShape -value cube
  radiobutton .right_frame.cylinder -text "Cylinder" \
    -variable shape_type -command SelectShape -value cylinder
  radiobutton .right_frame.pyramid  -text "Pyramid" \
    -variable shape_type -command SelectShape -value pyramid
  radiobutton .right_frame.sphere   -text "Sphere" \
    -variable shape_type -command SelectShape -value sphere
  radiobutton .right_frame.torus    -text "Torus" \
    -variable shape_type -command SelectShape -value torus

  pack .right_frame.box \
       .right_frame.cone \
       .right_frame.cube \
       .right_frame.cylinder \
       .right_frame.pyramid \
       .right_frame.sphere \
       .right_frame.torus \
       -side top -anchor w

  global shape_type

  set shape_type box

  SelectShape

  #----

  radiobutton .right_frame.solid     -text "Solid" \
    -variable draw_mode -command SelectDrawType -value solid
  radiobutton .right_frame.wireframe -text "Wireframe" \
    -variable draw_mode -command SelectDrawType -value wireframe

  pack .right_frame.solid \
       .right_frame.wireframe \
       -side top -anchor w

  global draw_mode

  set draw_mode solid

  SelectDrawMode

  #----

  radiobutton .right_frame.flat    -text "Flat" \
    -variable draw_type -command SelectDrawType -value flat
  radiobutton .right_frame.gouraud -text "Gouraud" \
    -variable draw_type -command SelectDrawType -value gouraud
  radiobutton .right_frame.phong   -text "Phong" \
    -variable draw_type -command SelectDrawType -value phong

  pack .right_frame.flat \
       .right_frame.gouraud \
       .right_frame.phong \
       -side top -anchor w

  global draw_type

  set draw_type flat

  SelectDrawType

  #----

  button .right_frame.quit -text "Quit" -command QuitCmd

  pack .right_frame.quit

  #----

  pack .left_frame -side left -fill both -expand true

  pack .right_frame -side left -fill y
}

proc SelectShape { } {
  global canvas
  global shape_type

  $canvas delete all

  if     {$shape_type == "box"     } {
    $canvas create box
  } \
  elseif {$shape_type == "cone"    } {
    $canvas create cone
  } \
  elseif {$shape_type == "cube"    } {
    $canvas create cube
  } \
  elseif {$shape_type == "cylinder"} {
    $canvas create cylinder
  } \
  elseif {$shape_type == "pyramid" } {
    $canvas create pyramid
  } \
  elseif {$shape_type == "sphere"  } {
    $canvas create sphere
  } \
  elseif {$shape_type == "torus"   } {
    $canvas create torus
  }
}

proc SelectDrawMode { } {
  global canvas
  global draw_mode

  if     {$draw_mode == "flat"} {
    $canvas configure -drawmode flat
  } \
  elseif {$draw_mode == "wireframe" } {
    $canvas configure -drawmode wireframe
  }
}

proc SelectDrawType { } {
  global canvas
  global draw_type

  if     {$draw_type == "solid"} {
    $canvas configure -fillmode solid
  } \
  elseif {$draw_type == "gouraud"   } {
    $canvas configure -fillmode gouraud
  } \
  elseif {$draw_type == "phong"     } {
    $canvas configure -fillmode phong
  }
}

proc QuitCmd { } {
  exit
}

createGUI
