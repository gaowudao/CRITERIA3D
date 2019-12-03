#!/bin/bash
cd bin/Makeall_PRAGA
/usr/lib/x86_64-linux-gnu/qt5/bin/qmake Makeall_PRAGA.pro -spec linux-g++-64 CONFIG+=debug CONFIG+=qml_debug CONFIG+=c++11 CONFIG+=qtquickcompiler
make -f Makefile qmake_all
make
