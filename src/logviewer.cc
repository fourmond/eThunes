/*
    filterdialog.cc: dialog boxes for filter
    Copyright 2010 by Vincent Fourmond

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

#include <headers.hh>
#include <logviewer.hh>

LogViewer::LogViewer(LogStorage * s)
{
  /// @todo flag ownership to delete the Storage upon destruction.
  if(s)
    storage = s;
  else
    storage = new LogStorage();

  QVBoxLayout * l1 = new QVBoxLayout(this);
  l1->addWidget(new QLabel(tr("Log messages")));

  viewer = new QTextEdit();
  viewer->setReadOnly(true);
  viewer->setAcceptRichText(true);
  l1->addWidget(viewer);
  populateViewer();
  connect(storage, SIGNAL(receivedMessage(const LogMessage &)),
	  SLOT(addMessage(const LogMessage &)));
}


void LogViewer::populateViewer()
{
  QList<LogMessage> messages = storage->getMessages();
  viewer->clear();
  
  for(int i = 0; i < messages.size(); i++)
    addMessage(messages[i]);
}

void LogViewer::addMessage(const LogMessage & msg)
{
  viewer->append(msg.message);
  QTextStream o(stdout);
}
