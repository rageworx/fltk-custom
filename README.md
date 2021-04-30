# Fast Light Tool Kit (FLTK) Version 1.4.0.6

## WHAT IS FLTK?

The Fast Light Tool Kit ("FLTK", pronounced "fulltick") is a cross-platform C++ GUI toolkit for UNIX(r)/Linux(r) (X11), Microsoft(r) Windows(r), and MacOS(r) X. FLTK provides modern GUI functionality without the bloat and supports 3D graphics via OpenGL(r) and its built-in GLUT emulation. 
It was originally developed by Mr. Bill Spitzak and is currently maintained by a small group of developers across the world with a central repository in the US.
    
* For more information see README.txt:
    https://github.com/fltk/fltk/blob/master/README.txt

## This is customized version from fltk-1.4.0

* internal version 1.4.0.6
* ABI also same but extension version flag existed as `FL_EXT_VERSION` in Enumerfations.H.
```
/*
  FLTK_EXT_VERSION is for identifying customized version for
  rageworx's github repository.
  --- currently 1.4.0.6
*/
#define FLTK_EXT_VERSION 6
```
### What different ?

* Source codes some scheme presents as "flat" for extra visual.
* fl_ask is working different way to applying user side color changes.
* Fl_Choice affects by theme scheme.
* Fl_Text_Display affects by theme scheme.
* Fl_Spinner affects by theme scheme.
