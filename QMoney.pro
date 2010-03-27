# -*- mode: conf; -*-
# QMake configuration file for QCam

TEMPLATE = app
CONFIG += precompile_header debug
INCLUDEPATH += . src

VERSION = 0.0

QT += xml

PRECOMPILED_HEADER = src/headers.hh

# Following lines for profiling
# QMAKE_CXXFLAGS += -pg
# QMAKE_LFLAGS += -pg

# Input files
SOURCES += src/qmain.cc src/account.cc src/mainwin.cc src/actions.cc \
	src/ofximport.cc src/account-model.cc \
	src/transaction.cc src/wallet.cc src/dashboardwidget.cc \
	src/walletdw.cc src/navigationwidget.cc \
	src/navigationpage.cc src/accountpage.cc \
	src/serializable.cc src/category.cc

HEADERS += src/account.hh src/mainwin.hh src/actions.hh \
	src/ofximport.hh src/account-model.hh \
	src/transaction.hh src/wallet.hh src/dashboardwidget.hh	\
	src/walletdw.hh src/navigationwidget.hh	\
	src/navigationpage.hh src/accountpage.hh \
	src/serializable.hh src/category.hh
