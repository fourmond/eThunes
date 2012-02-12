/*
  loan.cc: implementation of the Loan plugin
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
#include <utils.hh>
#include <transaction.hh>

#include <latexoutput.hh>
#include <htlabel.hh>
#include <latextable.hh>

#include <navigationwidget.hh>

#include <wallet.hh>
#include <cabinet.hh>

#include "loan.hh"
#include "loanpage.hh"

#include <httarget-templates.hh>

#include <QtCore/qmath.h>


static Plugin * loanCreator(const QString &)
{
  return new LoanPlugin();
}

static PluginDefinition loan(loanCreator, 
                             "Loan helper",
                             "");


//////////////////////////////////////////////////////////////////////


Loan::Loan() : targetPlugin(NULL)
{
  amount = 0;
}

SerializationAccessor * Loan::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  addLinkAttributes(ac);
  ac->addScalarAttribute("name", &name);
  ac->addScalarAttribute("date-contracted", &dateContracted);
  ac->addScalarAttribute("amount", &amount);
  ac->addScalarAttribute("yearly-rate", &yearlyRate);
  ac->addScalarAttribute("matcher", &matcher);
  return ac;
}

QString Loan::typeName() const
{
  return "loan";
}

QString Loan::publicTypeName() const
{
  return "Loan: " + name;
}

void Loan::followLink() 
{
  NavigationWidget::gotoPage(targetPlugin->pageForPlugin());
}

QString Loan::html() 
{
  QString str = QObject::tr("<h2>Loan: %1 %2</h2>").
    arg(name).arg(HTTarget::linkToMember("(change name)", this, 
                                         &Loan::promptForName));

  str += QObject::tr("Amount: %1 %2, started: %3 %4, yearly interest rate: "
                     "%5 % %6").
    arg(Transaction::formatAmount(amount)).
    arg(HTTarget::linkToMember("(change)", this, 
                               &Loan::promptForAmount)). 
    arg(dateContracted.toString(Qt::DefaultLocaleLongDate)).
    arg(HTTarget::linkToMember("(change)", this, 
                               &Loan::promptForDate)).
    arg(100 * yearlyRate).
    arg(HTTarget::linkToMember("(change)", this, 
                               &Loan::promptForRate));

  // Now the more interesting stuff:

  computeDebt();
  str += QObject::tr("<ul><li>total paid: %1</li>\n"
                     "<li>amount left: %2</li>\n" 
                     "<li>months so far: %3 (%5 payments)</li>\n" 
                     "<li>cumulated interests: %4</li></ul>").
    arg(Transaction::formatAmount(totalPaid)).
    arg(Transaction::formatAmount(amountLeft)).
    arg(monthsRunning).
    arg(Transaction::formatAmount(paidInterests)).
    arg(matchingTransactions.size());

  str += QObject::tr("Payments match: %1 %2 %3 <br/>").
    arg(matcher).
    arg(HTTarget::linkToMember("(change)", this, 
                               &Loan::promptForMatcher)).
    arg(HTTarget::linkToMember("(find matching transactions)", this, 
                               &Loan::findMatchingTransactions));


  str += QObject::tr("Payments: ");
  QStringList trs;
  for(int i = 0; i < matchingTransactions.size(); i++) {
    Transaction * t = matchingTransactions[i];
    trs << QString("%1: %2").
      arg(HTTarget::linkTo(t->getDate().toString("dd/MM/yyyy"),
                           t)).
      arg(Transaction::formatAmount(-t->getAmount()));
  }
  str += trs.join(", ");

  return str;
}

void Loan::promptForName()
{
  QString nn = QInputDialog::getText(NULL, QObject::tr("Name for the loan"),
                                     QObject::tr("Enter new name for the loan"),
                                     QLineEdit::Normal, name);
  if(nn.isEmpty())
    return;
  name = nn;
  updatePage();
}

void Loan::promptForMatcher()
{
  QString nn = QInputDialog::getText(NULL, QObject::tr("Transactions matching"),
                                     QObject::tr("Transactions matching: "),
                                     QLineEdit::Normal, matcher);
  if(nn.isEmpty())
    return;
  matcher = nn;
  updatePage();
}

void Loan::promptForAmount()
{
  int na = QInputDialog::getInt(NULL, QObject::tr("Amount of the loan: %1").
                                arg(name),
                                QObject::tr("Enter the amount of loan %1 "
                                            "(in cents)").
                                arg(name), amount);
  amount = na;
  updatePage();
}

void Loan::promptForRate()
{
  double na = 
    QInputDialog::getDouble(NULL, QObject::tr("Interest rate for loan: %1").
                            arg(name),
                            QObject::tr("Enter the interest rate of loan %1 "
                                        "(in percents)").
                            arg(name), yearlyRate);
  yearlyRate = na * 0.01;
  updatePage();
}

void Loan::promptForDate()
{
  QDate nd = 
    Utils::promptForDate(NULL, 
                         QObject::tr("Initial date for loan %1").
                         arg(name),
                         QObject::tr("Edit the initial date for %1").
                         arg(name),
                         dateContracted);
  if(! nd.isValid())
    return;
  dateContracted = nd;
  updatePage();
}

void Loan::findMatchingTransactions()
{
  QDate end = QDate::currentDate();
  TransactionPtrList transactions = 
    targetPlugin->cabinet->wallet.
    transactionsWithinRange(dateContracted.addMonths(1), end);

  for(int j = 0; j < transactions.size(); j++) {
    Transaction * t = transactions[j];
    if(t->hasNamedLinks("loan-payment"))
      continue;

    // Very simple ?
    if(t->getMemo().contains(matcher))
      addLink(t, "loan-payment");
  }
  updatePage();
}

void Loan::updatePage()
{
  LoanPage * page = (LoanPage *) targetPlugin->pageForPlugin();
  page->updatePage();
}

void Loan::computeDebt()
{
  double monthlyRate = qPow(1+yearlyRate, 1/12.); // Doesn't start
  // well...

  // First, we convert all links named "loan-payment" into a real
  // transaction list
  QList<Link *> tr = links.namedLinks("loan-payment");

  matchingTransactions.clear();

  for(int i = 0; i < tr.size(); i++) {
    Transaction * t = dynamic_cast<Transaction *>(tr[i]->linkTarget());
    if(t)
      matchingTransactions << t;
  }
  matchingTransactions.sortByDate();

  amountLeft = amount;
  totalPaid = 0;
  paidInterests = 0;
  monthsRunning = 0;

  if(! dateContracted.isValid())
    return ;

  int curMID = Transaction::monthID(QDate::currentDate());
  int trid = 0;

  

  // We use a month-based way to see the things
  for(int mid = Transaction::monthID(dateContracted) + 1; 
      mid < curMID; mid++) {
    amountLeft *= monthlyRate;
    while(trid < matchingTransactions.size() && 
          matchingTransactions[trid]->monthID() < mid)
      trid++;
    Transaction * cur = matchingTransactions.value(trid, NULL);
    if(cur && cur->monthID() == mid) {
      totalPaid -= cur->getAmount(); // Don't forget the amount is NEGATIVE !
      amountLeft += cur->getAmount();
      trid++;
    }
    monthsRunning += 1;
  }
  paidInterests = totalPaid + amountLeft - amount;
}


//////////////////////////////////////////////////////////////////////


LoanPlugin::LoanPlugin() 
{
}


NavigationPage * LoanPlugin::pageForPlugin()
{
  return LoanPage::getLoanPage(this);
}

SerializationAccessor * LoanPlugin::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("name", &name); // MUST NOT BE FORGOTTEN !
  ac->addListAttribute("loans", &loans);
  return ac;
}


void LoanPlugin::finishedSerializationRead()
{
  // Backreferences...
  for(int i = 0; i < loans.size(); i++)
    loans[i].targetPlugin = this;
}

