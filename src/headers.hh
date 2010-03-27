/** 
    \file headers.hh precompiled headers for QMoney
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

#include <QTabBar>
#include <QTabWidget>


// Dialogs and the like
#include <QFileDialog>

// Non-GUI objects
#include <QDate>
#include <QString>
#include <QFile>
#include <QTextStream>

// Templates
#include <QHash>
#include <QList>

// Item-model stuff
#include <QAbstractItemModel>
#include <QTreeView>

// XML-related classes
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#endif
