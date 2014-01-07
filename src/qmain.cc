/**
   \file qmain.cc
   entry point of eThunes
   Copyright 2008 by Vincent Fourmond

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see
   <http://www.gnu.org/licenses/>.
*/

#include <headers.hh>
#include <mainwin.hh>
#include <log.hh>
#include <commandline.hh>

#include <ruby.hh>

void loadTranslations(const QString & locale, QCoreApplication * app)
{
  QTranslator * translator = new QTranslator;
  QTextStream o(stderr);
  translator->load("qt_" + locale,
                   QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app->installTranslator(translator);

  translator = new QTranslator;

  QStringList pathes = QStringList()
    << 
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    QDesktopServices::storageLocation(QDesktopServices::HomeLocation)
#else
    QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
#endif
    + 
    "/Prog/eThunes/ts"
    << "/home/vincent/Prog/eThunes/ts";
  for(int i = 0; i < pathes.size(); i++) {
    QString path = pathes[i];
    if(translator->load("eThunes_" + locale,
                        path)) {
      o << "Found translations for locale " << locale  
        << " at " << path << endl;
      app->installTranslator(translator);
      return;
    }
  }
  o << "Failed to load translations for locale " << locale << endl;
}


int main(int argc, char ** argv)
{
  QApplication main(argc, argv);
  QFile log;
  log.open(stdout, QIODevice::WriteOnly);
  Log::logger()->spy = &log;

  loadTranslations(QLocale::system().name(), &main);

  // The search path for Ruby code (general-purpose modules)
  QDir::addSearchPath("ruby", "/home/vincent/Prog/eThunes/ruby");
  QDir::addSearchPath("icons", "/home/vincent/Prog/eThunes/data");

  Ruby::ensureInitRuby();

  // Command-line parsing if applicable
  if(parseCommandLine())
    return 0;

  MainWin win;

  main.setApplicationName("eThunes");
  win.show();

  return main.exec();
}

