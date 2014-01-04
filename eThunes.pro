# -*- mode: conf; -*-
# QMake configuration file for QCam

TEMPLATE = app
CONFIG += exception precompile_header debug warn_on thread
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

# We use poppler
LIBS += -lpoppler-qt4

# You can specify the full path of ruby on the command-line using:
# qmake RUBY=/usr/local/ruby1.8/bin/ruby
isEmpty(RUBY):RUBY = ruby

# Ruby detection/installation
RUBY_LIB_ARG = $$system($$RUBY ./get-ruby-config.rb libarg)
RUBY_INCLUDE_DIRS = $$system($$RUBY ./get-ruby-config.rb includedir)
RUBY_LIB_DIR = $$system($$RUBY ./get-ruby-config.rb libdir)

isEmpty(RUBY_LIB_ARG) {
  error("Could not find ruby, make sure $$RUBY is in the PATH !")
}

message("Ruby: using $$RUBY, found library: $$RUBY_LIB_ARG and includes at $$RUBY_INCLUDE_DIRS")
INCLUDEPATH += $$RUBY_INCLUDE_DIRS
LIBS += $$RUBY_LIB_ARG


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
	src/document.cc src/collection.cc \
	src/transactionlistdialog.cc \
	src/attributehash.cc src/ruby-utils.cc \
	src/collectioncode.cc src/cabinet.cc \
	src/cabinetpage.cc src/collectionsdw.cc \
	src/collectionpage.cc src/documentsmodel.cc \
	src/documentspage.cc src/managedfile.cc \
	src/linkable.cc \
        src/serializable-templates.cc \
	src/link.cc src/transactionlistwidget.cc \
	src/fetcher.cc src/result.cc \
	src/cookiejar.cc src/log.cc \
	src/logstream.cc src/filterpage.cc \
	src/categorycombo.cc src/filterelements.cc \
	src/documentpage.cc src/logstorage.cc \
	src/logviewer.cc src/accountgroup.cc \
	src/commandline.cc src/tag.cc \
        src/serialization.cc src/statistics.cc src/watchable.cc \
        src/flowinggridlayout.cc src/documentlistwidget.cc \
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
        src/identitiessafe.cc \
        src/collectiondefinition.cc \
        src/documentdefinition.cc

#        src/testserializepointers.cc \

HEADERS += src/account.hh src/mainwin.hh src/actions.hh \
	src/ofximport.hh src/accountmodel.hh \
	src/transaction.hh src/wallet.hh src/dashboardwidget.hh	\
	src/walletdw.hh src/navigationwidget.hh	\
	src/navigationpage.hh src/accountpage.hh \
	src/serializable.hh src/category.hh \
	src/filter.hh src/filterdialog.hh \
	src/categorypage.hh src/transactionlists.hh \
	src/statisticsmodel.hh \
	src/document.hh src/collection.hh \
	src/transactionlistdialog.hh \
	src/attributehash.hh src/ruby-utils.hh \
	src/collectioncode.hh src/cabinet.hh \
	src/cabinetpage.hh src/collectionsdw.hh \
	src/collectionpage.hh src/documentsmodel.hh \
	src/documentspage.hh src/managedfile.hh \
	src/linkable.hh \
        src/serializable-templates.hh \
	src/link.hh src/transactionlistwidget.hh \
	src/fetcher.hh src/result.hh \
	src/cookiejar.hh src/log.hh \
	src/logstream.hh src/filterpage.hh \
	src/categorycombo.hh src/filterelements.hh \
	src/documentpage.hh src/logstorage.hh \
	src/logviewer.hh src/accountgroup.hh \
	src/commandline.hh src/tag.hh \
        src/serialization.hh src/statistics.hh src/watchable.hh \
        src/watchablecontainers.hh  src/flowinggridlayout.hh \
        src/documentlistwidget.hh src/plugin.hh \
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
        src/ruby-templates.hh \
        src/identitiessafe.hh \
        src/collectiondefinition.hh \
        src/documentdefinition.hh


# Plugins:

        


        

# Loan tracker
HEADERS += plugins/loans/loan.hh \
        plugins/loans/loanpage.hh


SOURCES += plugins/loans/loan.cc \
        plugins/loans/loanpage.cc

# Translations

TRANSLATIONS = ts/eThunes_fr.ts