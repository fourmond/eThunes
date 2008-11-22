# -*- mode: conf; -*-
# QMake configuration file for QCam

TEMPLATE = app
CONFIG += precompile_header debug
INCLUDEPATH += . src

VERSION = 0.1

QT += sql

PRECOMPILED_HEADER = src/headers.hh

# Following lines for profiling
# QMAKE_CXXFLAGS += -pg
# QMAKE_LFLAGS += -pg

# Input files
SOURCES += src/qmain.cc src/account.cc src/mainwin.cc src/actions.cc \
	src/ofximport.cc

HEADERS += src/account.hh src/mainwin.hh src/actions.hh \
	src/ofximport.hh
