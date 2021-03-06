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
// #include <linkshandler.hh>
#include <navigationwidget.hh>
#include <documentspage.hh>

#include <statisticswidget.hh>

#include <budgetdw.hh>

#include <httarget-templates.hh>
#include <htlabel.hh>

CabinetPage::CabinetPage(Cabinet * c) : cabinet(c)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new HTLabel;
  layout->addWidget(summary);

  QHBoxLayout * hb = new QHBoxLayout();
  walletDW = new WalletDW(cabinet);
  hb->addWidget(walletDW);


  budgetDW = new BudgetDW(&cabinet->wallet);
  hb->addWidget(budgetDW);
  
  connect(cabinet, SIGNAL(fileLoaded()),
          budgetDW, SLOT(updateSummary()));


  // collectionsDW = new CollectionsDW(cabinet);
  // hb->addWidget(collectionsDW);

  plugins = new HTLabel;
  hb->addWidget(plugins);
  layout->addLayout(hb);


  stats = new StatisticsWidget(cabinet);
  layout->addWidget(stats);


  updateContents();
  connect(cabinet, SIGNAL(filenameChanged(const QString&)),
	  SLOT(updateContents()));

  connect(cabinet, SIGNAL(filenameChanged(const QString&)),
	  SIGNAL(filenameChanged(const QString&)));
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
  summary->setText(tr("<b>Cabinet : </b>%1<p>%2").
                   arg(cabinet->fileName()).
                   arg(HTTarget::linkToMember("(docs)", 
                                              this, &CabinetPage::showDocumentsPage)));

                 


  // We list the plugins
  {
    QString str;
    str = "<h2>Plugins</h2>";
    str += HTTarget::linkToMember("(add plugin)", 
                                  this, &CabinetPage::promptAddPlugin);
    str += "<p>";
    for(int i = 0; i < cabinet->plugins.size(); i++)
      str += QString("<b>%1:</b> %2<br>\n").
        arg(cabinet->plugins[i]->typeName()).
        arg(HTTarget::linkTo(cabinet->plugins[i]->getName(),
                             cabinet->plugins[i]));
    plugins->setText(str);
  }
  stats->update();
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
  // puzzled why this isn't called, but...
}

void CabinetPage::showDocumentsPage()
{
  NavigationWidget::gotoPage(DocumentsPage::getDocumentsPage(cabinet));
}

void CabinetPage::promptAddPlugin()
{
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
  plugin->cabinet = cabinet;
  updateContents();
}

