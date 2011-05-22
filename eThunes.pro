# -*- mode: conf; -*-
# QMake configuration file for QCam

TEMPLATE = app
CONFIG += precompile_header debug warn_on thread
INCLUDEPATH += . src

DEPENDPATH += src

VERSION = 0.0

QT += xml network

PRECOMPILED_HEADER = src/headers.hh

# Use a build/ directory for building
MOC_DIR = build
OBJECTS_DIR = build

# Really, this should be the default, since it means segfault in the
# best case
QMAKE_CXXFLAGS += -Werror=return-type 

# Handling of Ruby sources, but rather bad for now
# Bad for now
INCLUDEPATH += /usr/lib/ruby/1.8/x86_64-linux
LIBS += -lruby1.8 -lpoppler-qt4

# Input files
SOURCES += src/qmain.cc src/account.cc src/mainwin.cc src/actions.cc \
	src/ofximport.cc src/accountmodel.cc \
	src/transaction.cc src/wallet.cc src/dashboardwidget.cc \
	src/walletdw.cc src/navigationwidget.cc \
	src/navigationpage.cc src/accountpage.cc \
	src/serializable.cc src/category.cc \
	src/filter.cc src/filterdialog.cc \
	src/categorypage.cc src/transactionlists.cc \
	src/categorymodel.cc \
	src/document.cc src/collection.cc \
	src/rubymodulecode.cc src/transactionlistdialog.cc \
	src/attributehash.cc src/ruby-utils.cc \
	src/collectioncode.cc src/cabinet.cc \
	src/cabinetpage.cc src/collectionsdw.cc \
	src/collectionpage.cc src/documentsmodel.cc \
	src/documentspage.cc src/managedfile.cc \
	src/linkshandler.cc src/linkable.cc \
	src/link.cc src/transactionlistwidget.cc \
	src/fetcher.cc src/result.cc \
	src/cookiejar.cc src/log.cc \
	src/logstream.cc src/filterpage.cc \
	src/categorycombo.cc src/filterelements.cc \
	src/documentpage.cc src/logstorage.cc \
	src/logviewer.cc src/accountgroup.cc \
	src/commandline.cc src/tag.cc \
        src/serialization.cc src/statistics.cc src/watchable.cc

HEADERS += src/account.hh src/mainwin.hh src/actions.hh \
	src/ofximport.hh src/accountmodel.hh \
	src/transaction.hh src/wallet.hh src/dashboardwidget.hh	\
	src/walletdw.hh src/navigationwidget.hh	\
	src/navigationpage.hh src/accountpage.hh \
	src/serializable.hh src/category.hh \
	src/filter.hh src/filterdialog.hh \
	src/categorypage.hh src/transactionlists.hh \
	src/categorymodel.hh \
	src/document.hh src/collection.hh \
	src/rubymodulecode.hh src/transactionlistdialog.hh \
	src/attributehash.hh src/ruby-utils.hh \
	src/collectioncode.hh src/cabinet.hh \
	src/cabinetpage.hh src/collectionsdw.hh \
	src/collectionpage.hh src/documentsmodel.hh \
	src/documentspage.hh src/managedfile.hh \
	src/linkshandler.hh src/linkable.hh \
	src/link.hh src/transactionlistwidget.hh \
	src/fetcher.hh src/result.hh \
	src/cookiejar.hh src/log.hh \
	src/logstream.hh src/filterpage.hh \
	src/categorycombo.hh src/filterelements.hh \
	src/documentpage.hh src/logstorage.hh \
	src/logviewer.hh src/accountgroup.hh \
	src/commandline.hh src/tag.hh \
        src/serialization.hh src/statistics.hh src/watchable.hh
