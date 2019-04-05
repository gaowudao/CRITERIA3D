#=========================================================================
#
# Copyright (C) 2018 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
#
# This file is part of the QDbf - Qt DBF library.
#
# The QDbf is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# The QDbf is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the QDbf.  If not, see <http://www.gnu.org/licenses/>.
#
# https://github.com/IvanPinezhaninov/QDbf
#
#=========================================================================


TARGET = shapeDBF
TEMPLATE = lib
CONFIG += staticlib

DEFINES += SHAPEDBF_LIBRARY
DEFINES += _CRT_SECURE_NO_WARNINGS

SOURCES += qdbffield.cpp  qdbfrecord.cpp  qdbftable.cpp  qdbftablemodel.cpp

HEADERS += qdbf_compat.h  qdbffield.h  qdbf_global.h  qdbfrecord.h  qdbftable.h  qdbftablemodel.h


unix {
    target.path = /usr/lib
    INSTALLS += target
}
