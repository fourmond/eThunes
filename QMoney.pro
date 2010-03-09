# -*- mode: conf; -*-
# QMake configuration file for QCam

TEMPLATE = app
CONFIG += precompile_header debug
INCLUDEPATH += . src

VERSION = 0.0

PRECOMPILED_HEADER = src/headers.hh

# Following lines for profiling
# QMAKE_CXXFLAGS += -pg
# QMAKE_LFLAGS += -pg

# Input files
SOURCES += src/qmain.cc src/account.cc src/mainwin.cc src/actions.cc \
	src/ofximport.cc src/transaction-list-model.cc \
	src/transaction.cc 

HEADERS += src/account.hh src/mainwin.hh src/actions.hh \
	src/ofximport.hh src/transaction-list-model.hh \
	src/transaction.hh