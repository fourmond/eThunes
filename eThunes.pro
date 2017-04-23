# -*- mode: conf; -*-
# QMake configuration file for QCam

TEMPLATE = app
CONFIG += exception precompile_header debug warn_on thread
CONFIG += c++11
INCLUDEPATH += . src

DEPENDPATH += src

VERSION = 0.0

QT += xml network

# QT5:
QT +=  widgets

QT += qml

PRECOMPILED_HEADER = src/headers.hh

# Use a build/ directory for building
MOC_DIR = build
OBJECTS_DIR = build

# Really, this should be the default, since it means segfault in the
# best case
QMAKE_CXXFLAGS += -Werror=return-type 

# We use poppler
LIBS += -lpoppler-qt5


# Input files
SOURCES += src/qmain.cc src/account.cc src/mainwin.cc src/actions.cc \
	src/ofximport.cc src/accountmodel.cc \
	src/transaction.cc src/wallet.cc src/dashboardwidget.cc \
	src/walletdw.cc src/navigationwidget.cc \
	src/navigationpage.cc src/accountpage.cc \
	src/serializable.cc src/category.cc \
	src/filter.cc src/filterdialog.cc \
	src/categorypage.cc src/transactionlists.cc \
	src/statisticsmodel.cc \
	src/transactionlistdialog.cc \
	src/attributehash.cc  \
	src/pdftools.cc src/cabinet.cc \
	src/cabinetpage.cc src/managedfile.cc \
	src/linkable.cc \
        src/serializable-templates.cc \
	src/link.cc src/transactionlistwidget.cc \
	src/cookiejar.cc src/log.cc \
	src/logstream.cc src/filterpage.cc \
	src/categorycombo.cc src/filterelements.cc \
	src/logstorage.cc \
	src/logviewer.cc src/accountgroup.cc \
	src/commandline.cc src/tag.cc \
        src/serialization.cc src/statistics.cc src/watchable.cc \
        src/flowinggridlayout.cc  \
        src/plugin.cc src/utils.cc \
        src/widgetwrapperdialog.cc \
        src/httarget.cc src/htlabel.cc src/htlinks.cc \
        src/latexoutput.cc src/evolvingitemwidget.cc \
        src/latextable.cc src/statisticswidget.cc \
        src/modelitems.cc src/oomodel.cc \
        src/ootest.cc src/tagpage.cc \
        src/timebasedcurve.cc src/timebasedwidget.cc \
        src/curvesdisplay.cc \
        src/exceptions.cc \
        src/budget.cc \
        src/budgetpage.cc

HEADERS += src/account.hh src/mainwin.hh src/actions.hh \
           src/ofximport.hh src/accountmodel.hh \
	   src/transaction.hh src/wallet.hh src/dashboardwidget.hh	\
	   src/walletdw.hh src/navigationwidget.hh	\
	   src/navigationpage.hh src/accountpage.hh \
	   src/serializable.hh src/category.hh \
	   src/filter.hh src/filterdialog.hh \
	   src/categorypage.hh src/transactionlists.hh \
	   src/statisticsmodel.hh \
	   src/transactionlistdialog.hh \
	   src/attributehash.hh  \
	   src/pdftools.hh src/cabinet.hh \
	   src/cabinetpage.hh  \
	   src/managedfile.hh \
	   src/linkable.hh \
           src/serializable-templates.hh \
	   src/link.hh src/transactionlistwidget.hh \
	   src/cookiejar.hh src/log.hh \
	   src/logstream.hh src/filterpage.hh \
	   src/categorycombo.hh src/filterelements.hh \
	   src/logstorage.hh \
	   src/logviewer.hh src/accountgroup.hh \
	   src/commandline.hh src/tag.hh \
           src/serialization.hh src/statistics.hh src/watchable.hh \
           src/watchablecontainers.hh  src/flowinggridlayout.hh \
           src/plugin.hh \
           src/serializable-pointers.hh \
           src/testserializepointers.hh \
           src/widgetwrapperdialog.hh \
           src/utils.hh src/evolvingitem.hh \
           src/httarget.hh src/htlabel.hh src/httarget-templates.hh \
           src/latexoutput.hh src/evolvingitemwidget.hh \
           src/latextable.hh src/statisticswidget.hh \
           src/modelitems.hh src/oomodel.hh \
           src/ootest.hh src/tagpage.hh \
           src/timebasedcurve.hh src/timebasedwidget.hh \
           src/curvesdisplay.hh \
           src/pointersafesort.hh \
           src/exceptions.hh \
           src/budget.hh \
           src/budgetpage.hh

# Plugins:

        


        

# Loan tracker
HEADERS += plugins/loans/loan.hh \
        plugins/loans/loanpage.hh


SOURCES += plugins/loans/loan.cc \
        plugins/loans/loanpage.cc

# Translations

TRANSLATIONS = ts/eThunes_fr.ts