#!/bin/bash

# build mapGraphics
cd mapGraphics
qmake MapGraphics.pro -spec linux-g++-64 CONFIG+=debug CONFIG+=qml_debug CONFIG+=c++11 CONFIG+=qtquickcompiler
make

cd -

# build PRAGA
cd bin/Makeall_PRAGA
qmake Makeall_PRAGA.pro -spec linux-g++-64 CONFIG+=debug CONFIG+=qml_debug CONFIG+=c++11 CONFIG+=qtquickcompiler PREFIX=/usr
make -f Makefile qmake_all 
# remove QPalette::PlaceholderText because it is not supported, has been added with Qt 5.12
# grep -v 'QPalette::PlaceholderText' ../PRAGA/ui_mainWindow.h > ../PRAGA/ui.h; mv -f ../PRAGA/ui.h ../PRAGA/ui_mainWindow.h
make 

cd -

# download linuxdeployqt
wget -c -nv -O linuxqtdeploy "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod +x linuxqtdeploy

# build appimage
ls bin/Makeall_PRAGA

cp bin/Makeall_PRAGA/PRAGA deploy/PRAGA/appimage/usr/bin/PRAGA
./linuxdeployqt deploy/PRAGA/appimage/usr/share/applications/PRAGA.desktop -qmake=$QMAKE -qmlimport=/opt/qt512/qml -appimage -always-overwrite
