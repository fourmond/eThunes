/*
    cabinetpage.cc: Implementation of CabinetPage
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
#include <cabinetpage.hh>
#include <plugin.hh>

#include <statistics.hh>
#include <linkshandler.hh>
#include <navigationwidget.hh>

#include <httarget-templates.hh>
#include <htlabel.hh>

CabinetPage::CabinetPage(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new QLabel();
  layout->addWidget(summary);

  QHBoxLayout * hb = new QHBoxLayout();
  walletDW = new WalletDW(&cabinet->wallet);
  hb->addWidget(walletDW);

  collectionsDW = new CollectionsDW(cabinet);
  hb->addWidget(collectionsDW);

  plugins = new QLabel;
  hb->addWidget(plugins);

  connect(plugins, SIGNAL(linkActivated(const QString &)),
          SLOT(handlePluginLink(const QString &)));

  HTLabel * l = new HTLabel();
  hb->addWidget(l);
  QString str = "Bidule: ";
  str += HTTarget::linkToMember("load ?", 
                                this, &CabinetPage::load);
  l->setText(str);
  

  layout->addLayout(hb);


  stats = new QLabel();
  statsArea = new QScrollArea;
  statsArea->setWidget(stats);

  layout->addWidget(statsArea);
  LinksHandler::handleObject(stats);
  stats->show();
  

  updateContents();
  connect(cabinet, SIGNAL(filenameChanged(const QString&)),
	  SLOT(updateContents()));

  connect(cabinet, SIGNAL(filenameChanged(const QString&)),
	  SIGNAL(filenameChanged(const QString&)));
  stats->show();

  
}


CabinetPage::~CabinetPage()
{
}

QString CabinetPage::pageTitle()
{
  return tr("Dashboard");
}

void CabinetPage::updateContents()
{
  summary->setText(tr("<b>Cabinet : </b>%1<p>").
		   arg(cabinet->fileName()));
  // We list the plugins
  {
    QString str;
    str = "<h2>Plugins</h2><br><a href='add-plugin'>(add new plugin)</a><p>";
    for(int i = 0; i < cabinet->plugins.size(); i++)
      str += QString("<a href='plugin:%1'>%2</a><br>\n").
        arg(i).arg(cabinet->plugins[i]->getName());

    plugins->setText(str);
  }

  if(cabinet->wallet.accounts.size() > 0) {
    // We pick the one with the most transactions:
    Account * account = NULL;
    for(int i = 0; i < cabinet->wallet.accounts.size(); i++) {
      if( ( ! account) ||
          account->transactions.size() < 
          cabinet->wallet.accounts[i].transactions.size())
        account = &cabinet->wallet.accounts[i];
      Statistics s(account->transactions.toPtrList());
      stats->setText(s.htmlStatistics(-1));
      stats->resize(stats->sizeHint());
    }
  } 
  else
    stats->setText("Stuff !");
}

void CabinetPage::save()
{
  if(currentFileName().isEmpty())
    saveAs();
  else
    cabinet->saveToFile(currentFileName());
}

void CabinetPage::saveAs()
{
  QString str =
    QFileDialog::getSaveFileName(this,
				 tr("Save cabinet as"),
				 QString(),
				 tr("XML cabinet files (*.xml)"));
  if(str.isEmpty())
    return;
  cabinet->saveToFile(str);
}

void CabinetPage::load()
{
  QString file =
    QFileDialog::getOpenFileName(this,
				 tr("Select cabinet to load"),
				 QString(),
				 tr("XML cabinet files (*.xml)"));
  if(file.isEmpty())
    return;
  load(file);
}

void CabinetPage::load(const QString & file)
{
  cabinet->loadFromFile(file);
}

void CabinetPage::handlePluginLink(const QString & link)
{
  if(link == "add-plugin") {
    QList<const PluginDefinition *> defs = Plugin::availablePlugins();
    QStringList lst;
    for(int i = 0; i < defs.size(); i++)
      lst << defs[i]->publicName;
    QString str = 
      QInputDialog::getItem(this, tr("Choose the new plugin's type"), 
                            tr("Plugin type"), 
                            lst, 0, false);
    if(str.isEmpty())
      return;
    int idx = lst.indexOf(str);
    if(idx < 0)
      return;
    str = QInputDialog::getText(this, tr("Your name for the plugin"),
                                tr("Give a name"));
    if(str.isEmpty())
      return;
    Plugin * plugin = defs[idx]->createPlugin();
    plugin->setName(str);
    cabinet->plugins.append(plugin);
  }
  else if(link.startsWith("plugin:")) {
    int i = link.split(":")[1].toInt();
    NavigationPage * page = cabinet->plugins[i]->pageForPlugin();
    if(page)
      NavigationWidget::gotoPage(page);
    else {
      QTextStream o(stdout);
      o << "No page for plugin " << cabinet->plugins[i] << endl;
        
    }
  }
}
