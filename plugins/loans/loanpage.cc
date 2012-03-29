/*
  loanpage.cc: Implementation of LoanPage
  Copyright 2011 by Vincent Fourmond

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
#include <widgetwrapperdialog.hh>
#include <utils.hh>
#include <transaction.hh>

#include <latexoutput.hh>

#include <htlabel.hh>
#include <httarget-templates.hh>

#include "loanpage.hh"

QHash<LoanPlugin *, LoanPage *> LoanPage::loanPages;



LoanPage * LoanPage::getLoanPage(LoanPlugin * plugin)
{
  if(! loanPages.contains(plugin))
    loanPages[plugin] = new LoanPage(plugin);
  return loanPages[plugin];
}


LoanPage::LoanPage(LoanPlugin * p) : plugin(p)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new HTLabel();
  layout->addWidget(summary);

  updatePage();
}

QString LoanPage::pageTitle() 
{
  return tr("Loan Plugin: %1").arg(plugin->getName());
}


void LoanPage::addLoan()
{
  plugin->loans.append(Loan());
  plugin->loans.last().targetPlugin = plugin;
  updatePage();
}

void LoanPage::updatePage()
{
  // Here, we do various things, basically to update the main summary.
  QString str;
  str += tr("<h1>Loans: %1</h1>\n").
    arg(plugin->getName());

  int totalDebt = 0;
  int totalPaid = 0;

  for(int i = 0; i < plugin->loans.size(); i++) {
    str += plugin->loans[i].html();
    totalDebt += plugin->loans[i].amountLeft;
    totalPaid += plugin->loans[i].totalPaid;
  }

  str += tr("<p><b>Total debt:</b> %1<br>").
    arg(Transaction::formatAmount(totalDebt));
  str += tr("<b>Total paid:</b> %1<br>").
    arg(Transaction::formatAmount(totalPaid));
  
  str += HTTarget::linkToMember("(add loan)", this, &LoanPage::addLoan);

  summary->setText(str);
}
