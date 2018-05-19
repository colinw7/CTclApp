proc CTclAppDumpTclErrorsCB { name1 name2 op } {
  global errorInfo

  if {$errorInfo == ""} {
    return
  }

  if {[regexp "cannot call method .* for context .*" $errorInfo]} {
    return
  }

  if {[regexp "text doesn't contain any characters tagged with .*" $errorInfo]} {
    return
  }

  if {[regexp "selection isn't in entry.*" $errorInfo]} {
    return
  }

  puts "$errorInfo"
}

proc CTclAppDumpTclErrors { } {
  global dumpTclErrors

  set dumpTclErrors 1

  CTclAppInitTclErrors
}

proc CTclAppInitTclErrors { } {
  global dumpTclErrors

  if {[info exists dumpTclErrors]} {
    if {$dumpTclErrors == "1"} {
      global errorInfo

      trace variable errorInfo w CTclAppDumpTclErrorsCB
    }
  }
}

CTclAppDumpTclErrors

proc CTclAppAddReturnTextBinding { text proc } {
  bind $text <Return> [list $proc]
}

proc CTclAppMenuAdd { parent label underline } {
  global ctcl_app_menu
  global ctcl_app_menu_item_num

  if {$label == "Help"} {
    set ctcl_app_menu [menu $parent.help]
  } \
  else {
    set ctcl_app_menu [menu $parent.m$label]
  }

  set ctcl_app_menu_item_num 0

  $parent add cascade \
    -label $label -menu $ctcl_app_menu -underline $underline

  return $ctcl_app_menu
}

proc CTclAppMenuAddCmd { label command underline } {
  global ctcl_app_menu
  global ctcl_app_menu_item_num

  $ctcl_app_menu add command \
    -label $label -command $command -underline $underline

  incr ctcl_app_menu_item_num

  return $ctcl_app_menu_item_num
}

proc CTclAppMenuAddSep { } {
  global ctcl_app_menu
  global ctcl_app_menu_item_num

  $ctcl_app_menu add separator

  incr ctcl_app_menu_item_num
}

proc CTclAppMenuAddChk { label command variable underline } {
  global ctcl_app_menu
  global ctcl_app_menu_item_num

  $ctcl_app_menu add checkbutton \
    -label $label -command $command -variable $variable -underline $underline

  incr ctcl_app_menu_item_num
}

proc CTclAppMenuAddRad { label command variable underline } {
  global ctcl_app_menu
  global ctcl_app_menu_item_num

  $ctcl_app_menu add radio \
    -label $label -command $command -variable $variable -underline $underline

  incr ctcl_app_menu_item_num
}

proc CTclHSeparator { path {popts {-padx 2}} } {
  frame $path -height 2 -borderwidth 2 -relief sunken

  eval {pack $path -side top -anchor w -fill x} $popts
}

proc CTclVSeparator { path } {
  frame $path -width 2 -borderwidth 2 -relief sunken

  pack $path -side left -anchor w -fill y -padx 2
}

proc gset { var val } {
  upvar #0 $var var1

  set var1 $val
}
