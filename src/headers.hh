/**
    \file headers.hh precompiled headers for eThunes
    Copyright 2008 by Vincent Fourmond

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __HEADERS_HH
#define __HEADERS_HH


#include <QApplication>
#include <QSettings>

// Main GUI Classse
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QTabBar>
#include <QTabWidget>
#include <QScrollArea>

#include <QListWidget>
#include <QComboBox>

#include <QSplitter>
#include <QSignalMapper>

#include <QCheckBox>
#include <QDateTimeEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QPainter>

#include <QScrollBar>
#include <QAbstractSlider>
#include <QAbstractScrollArea>

// Deeply digging into Qt4...
#include <QStyledItemDelegate>

// Sorting and filtering
#include <QSortFilterProxyModel>

// Dialogs and the like
#include <QFileDialog>
#include <QDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QMessageBox>
#include <QDialogButtonBox>

#include <QSpinBox>

#include <QFileSystemModel>
#include <QMimeType>
#include <QMimeDatabase>


// A rich text viewer (and editor?).
// (not used)
#include <QTextEdit>
#include <QTextBrowser>

// Desktop stuff
#include <QUrl>

// Needed for post data
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#   include <QUrlQuery>
#   include <QStandardPaths>
#endif

#include <QDesktopServices>

// Non-GUI objects
#include <QDate>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QPointer>
#include <QTemporaryFile>

// Network
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkCookie>
#include <QNetworkCookieJar>
#include <QNetworkProxy>

// Templates
#include <QHash>
#include <QCache>
#include <QMultiHash>
#include <QList>

#include <QMutex>
#include <QMutexLocker>


// QML-related classes
#include <QQmlEngine>
#include <QQmlComponent>

#include <QMetaProperty>


// Item-model stuff
#include <QAbstractItemModel>
#include <QTreeView>

// XML-related classes
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

// Translations 
#include <QTranslator>
#include <QLibraryInfo>

// Poppler classes
#include <poppler/qt5/poppler-qt5.h>

#include <functional>

#endif
