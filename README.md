# README - Fast Light Tool Kit (FLTK) Version 1.4.0

## WHAT IS FLTK?

    The Fast Light Tool Kit ("FLTK", pronounced "fulltick") is a
    cross-platform C++ GUI toolkit for UNIX®/Linux® (X11 or
    Wayland), Microsoft® Windows®, and macOS®.
    FLTK provides modern GUI functionality without the bloat and
    supports 3D graphics via OpenGL® and its built-in GLUT
    emulation. It was originally developed by Mr. Bill Spitzak
    and is currently maintained by a small group of developers
    across the world with a central repository on GitHub.

    For more information see original README.txt:
    https://github.com/fltk/fltk/blob/master/README.txt

## WHAT IS FLTK-custom

This FLTK-custom version supports some different features like these

- Additional "flat" built-in scheme (theme).
- Some different widget drawing like text editors.
- Supports macOS x86.64 and arm64 universal binary building by simple scripts, check these files,
	- configMinGW.sh
	- configMac.sh

## Dettached repository.

This custom build not forked from FLTK original github since before they are not moved to github.
There's another forked source in name as 'fltk-origin'.

## Branches

- master : final latest stable changes.
- v1.3.5-2-ts : last changes branch for FLTK 1.3.5-2 "ts" version.
- v1.4.0-dev : experimental changes form original FLTK 1.4.0 repository.

## FLTK-origin build state
![Build](https://github.com/fltk/fltk/actions/workflows/build.yml/badge.svg)
