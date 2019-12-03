#!/bin/bash
cd bin/Makeall_PRAGA
/opt/qt512/bin/qmake Makeall_PRAGA.pro -spec linux-g++-64 CONFIG+=debug CONFIG+=qml_debug CONFIG+=c++11 CONFIG+=qtquickcompiler
make -f Makefile qmake_all
# remove QPalette::PlaceholderText because it is not supported, has been added with Qt 5.12
# grep -v 'QPalette::PlaceholderText' ../PRAGA/ui_mainWindow.h > ../PRAGA/ui.h; mv -f ../PRAGA/ui.h ../PRAGA/ui_mainWindow.h
make
