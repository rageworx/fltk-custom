//
// "Flat" drawing routines for the Fast Light Tool Kit (FLTK).
//
// Copyright 2017 by Raphael Kim (rageworx@gmail.com).
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//
// These box types provide Windows 10 flat design scheme
// for FLTK.
//

#include <FL/Fl.H>
#include <FL/fl_draw.H>

static void up_frame(int x, int y, int w, int h, Fl_Color c) 
{
	Fl_Color co = fl_color();
	fl_color( fl_lighter( c ) );
	fl_rect( x, y, w, h );
	fl_color( co );
}

static void up_box(int x, int y, int w, int h, Fl_Color c) 
{
	Fl_Color co = fl_color();
	fl_color( c );
	fl_rectf( x, y, w, h );
	fl_color( fl_lighter( c ) );
	fl_rect( x, y, w, h );
	fl_color( co );
}

static void thin_up_box(int x, int y, int w, int h, Fl_Color c) 
{
	Fl_Color co = fl_color();
	fl_color( c );
	fl_rectf( x, y, w, h );
	fl_color( fl_lighter( c ) );
	fl_rect( x, y, w, h );
	fl_color( co );
}

static void down_frame(int x, int y, int w, int h, Fl_Color c) 
{
	Fl_Color co = fl_color();
	fl_color( fl_darker ( c ) );
	fl_rect( x, y, w, h );
	fl_color( co );
}

static void down_box(int x, int y, int w, int h, Fl_Color c) 
{
	Fl_Color co = fl_color();
	fl_color( c );
	fl_rectf( x, y, w, h );
	fl_color( fl_darker( c ) );
	fl_rect( x, y, w, h );
	fl_color( co );
}

static void thin_down_box(int x, int y, int w, int h, Fl_Color c) 
{
	Fl_Color co = fl_color();
	fl_color( c );
	fl_rectf( x, y, w, h );
	fl_color( fl_darker( c ) );
	fl_rect( x, y, w, h );
	fl_color( co );
}

extern void fl_internal_boxtype(Fl_Boxtype, Fl_Box_Draw_F*);

Fl_Boxtype fl_define_FL_FLAT_UP_BOX() {
  fl_internal_boxtype(_FL_FLAT_UP_BOX, up_box);
  fl_internal_boxtype(_FL_FLAT_DOWN_BOX, down_box);
  fl_internal_boxtype(_FL_FLAT_UP_FRAME, up_frame);
  fl_internal_boxtype(_FL_FLAT_DOWN_FRAME, down_frame);
  fl_internal_boxtype(_FL_FLAT_THIN_UP_BOX, thin_up_box);
  fl_internal_boxtype(_FL_FLAT_THIN_DOWN_BOX, thin_down_box);
  fl_internal_boxtype(_FL_FLAT_ROUND_UP_BOX, up_box);
  fl_internal_boxtype(_FL_FLAT_ROUND_DOWN_BOX, down_box);
  return _FL_FLAT_UP_BOX;
}
