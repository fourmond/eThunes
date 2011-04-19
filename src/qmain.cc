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


int main(int argc, char ** argv)
{
  QApplication main(argc, argv);
  QFile log;
  log.open(stdout, QIODevice::WriteOnly);
  Log::logger()->spy = &log;
  if(parseCommandLine())
    return 0;

  MainWin win;

  // The search path for Ruby code (general-purpose modules)
  QDir::addSearchPath("ruby", "/home/vincent/Prog/eThunes/ruby");
  QDir::addSearchPath("icons", "/home/vincent/Prog/eThunes/data");
  main.setApplicationName("eThunes");
  win.show();
  return main.exec();
}

