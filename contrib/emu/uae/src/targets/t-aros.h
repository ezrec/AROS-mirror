 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Target specific stuff, AmigaOS version
  *
  * Copyright 1997 Bernd Schmidt
  */

#define PICASSO96_SUPPORTED

#define TARGET_NAME "aros"

#define UNSUPPORTED_OPTION_l

#define OPTIONSFILENAME ".uaerc"
#define OPTIONS_IN_HOME

#define TARGET_SPECIAL_OPTIONS 
#define COLOR_MODE_HELP_STRING \
    "\nValid color modes (see -H) are:\n" \
    "     0 => 256 cols max on customscreen;\n" \
    "     1 => OpenWindow on default public screen;\n" \
    "     2 => Ask the user to select a screen mode with ASL requester;\n" 

#define DEFSERNAME "ser:"
#define DEFPRTNAME "par:"

#define write_log write_log_standard
