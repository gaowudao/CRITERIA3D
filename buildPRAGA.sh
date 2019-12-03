#!/bin/bash
cd bin/Makeall_PRAGA
qmake Makeall_PRAGA.pro -spec linux-g++-64 CONFIG+=debug CONFIG+=qml_debug CONFIG+=c++11
make -f Makefile qmake_all
make
