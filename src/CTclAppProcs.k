static char CTclAppProcs_TK[] = "\n\
proc CTclAppDumpTclErrorsCB { name1 name2 op } {\n\
  global errorInfo\n\
  if {$errorInfo == \"\"} {\n\
    return\n\
  }\n\
  if {[regexp \"cannot call method .* for context .*\" $errorInfo]} {\n\
    return\n\
  }\n\
  if {[regexp \"text doesn't contain any characters tagged with .*\" $errorInfo]} {\n\
    return\n\
  }\n\
  if {[regexp \"selection isn't in entry.*\" $errorInfo]} {\n\
    return\n\
  }\n\
  puts \"$errorInfo\"\n\
}\n\
proc CTclAppDumpTclErrors { } {\n\
  global dumpTclErrors\n\
  set dumpTclErrors 1\n\
  CTclAppInitTclErrors\n\
}\n\
proc CTclAppInitTclErrors { } {\n\
  global dumpTclErrors\n\
  if {[info exists dumpTclErrors]} {\n\
    if {$dumpTclErrors == \"1\"} {\n\
      global errorInfo\n\
      trace variable errorInfo w CTclAppDumpTclErrorsCB\n\
    }\n\
  }\n\
}\n\
CTclAppDumpTclErrors\n\
proc CTclAppAddReturnTextBinding { text proc } {\n\
  bind $text <Return> [list $proc]\n\
}\n\
proc CTclAppMenuAdd { parent label underline } {\n\
  global ctcl_app_menu\n\
  global ctcl_app_menu_item_num\n\
  if {$label == \"Help\"} {\n\
    set ctcl_app_menu [menu $parent.help]\n\
  } \\\n\
  else {\n\
    set ctcl_app_menu [menu $parent.m$label]\n\
  }\n\
  set ctcl_app_menu_item_num 0\n\
  $parent add cascade \\\n\
    -label $label -menu $ctcl_app_menu -underline $underline\n\
  return $ctcl_app_menu\n\
}\n\
proc CTclAppMenuAddCmd { label command underline } {\n\
  global ctcl_app_menu\n\
  global ctcl_app_menu_item_num\n\
  $ctcl_app_menu add command \\\n\
    -label $label -command $command -underline $underline\n\
  incr ctcl_app_menu_item_num\n\
  return $ctcl_app_menu_item_num\n\
}\n\
proc CTclAppMenuAddSep { } {\n\
  global ctcl_app_menu\n\
  global ctcl_app_menu_item_num\n\
  $ctcl_app_menu add separator\n\
  incr ctcl_app_menu_item_num\n\
}\n\
proc CTclAppMenuAddChk { label command variable underline } {\n\
  global ctcl_app_menu\n\
  global ctcl_app_menu_item_num\n\
  $ctcl_app_menu add checkbutton \\\n\
    -label $label -command $command -variable $variable -underline $underline\n\
  incr ctcl_app_menu_item_num\n\
}\n\
proc CTclAppMenuAddRad { label command variable underline } {\n\
  global ctcl_app_menu\n\
  global ctcl_app_menu_item_num\n\
  $ctcl_app_menu add radio \\\n\
    -label $label -command $command -variable $variable -underline $underline\n\
  incr ctcl_app_menu_item_num\n\
}\n\
proc CTclHSeparator { path {popts {-padx 2}} } {\n\
  frame $path -height 2 -borderwidth 2 -relief sunken\n\
  eval {pack $path -side top -anchor w -fill x} $popts\n\
}\n\
proc CTclVSeparator { path } {\n\
  frame $path -width 2 -borderwidth 2 -relief sunken\n\
  pack $path -side left -anchor w -fill y -padx 2\n\
}\n\
proc gset { var val } {\n\
  upvar #0 $var var1\n\
  set var1 $val\n\
}\n\
";

