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
#include <math.h>

#include <periodic.hh>

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
  plannedMonthlyPayment = -1;
  lastMonthlyPayment = 0;
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

/// @todo It would be nice to add supplementary fees (such as bank
/// fees, insurance and the like...)
QString Loan::html() 
{
  QString str = QObject::tr("<h2>Loan: %1 %2</h2>").
    arg(name).arg(HTTarget::linkToMember("(change name)", this, 
                                         &Loan::promptForName));

  /// @todo make that configurable !
  monthlyRate = qPow(1+yearlyRate, 1/12.);
  monthlyRate = 1 + yearlyRate/12.0;

  str += QObject::tr("Amount: %1 %2, started: %3 %4, yearly interest rate: "
                     "%5 % (monthly: %7) %6").
    arg(Transaction::formatAmount(amount)).
    arg(HTTarget::linkToMember("(change)", this, 
                               &Loan::promptForAmount)). 
    arg(dateContracted.toString(Qt::DefaultLocaleLongDate)).
    arg(HTTarget::linkToMember("(change)", this, 
                               &Loan::promptForDate)).
    arg(100 * yearlyRate).
    arg(HTTarget::linkToMember("(change)", this, 
                               &Loan::promptForRate)).
    arg(100 * (monthlyRate - 1));

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


  str += QObject::tr("<b>Payments</b>: ");
  QStringList trs;
  for(int i = 0; i < matchingTransactions.size(); i++) {
    AtomicTransaction * t = matchingTransactions[i];
    trs << QString("%1: %2").
      arg(HTTarget::linkTo(t->getDate().toString("dd/MM/yyyy"),
                           t)).
      arg(Transaction::formatAmount(-t->getAmount()));
  }
  str += trs.join(", ");

  str += QObject::tr("<p><b>Extrapolation</b>: for a monthly rate of "
                     "%1 %2<br/>").
    arg(Transaction::formatAmount(effectiveMonthlyPayment)).
    arg(HTTarget::linkToMember("(change)", this, 
                               &Loan::promptForMonthlyPayment));

  if(remainingMonths < 0)
    str += QObject::tr(" -> indefinite");
  else
    str += QObject::tr("%1 months left, remaining %2, remaining cost: %3, "
                       "total: %4, total cost: %5").
      arg(remainingMonths).
      arg(Transaction::formatAmount(leftToPay)).
      arg(Transaction::formatAmount(leftToPay-amountLeft)).
      arg(Transaction::formatAmount(leftToPay + totalPaid)).
      arg(Transaction::formatAmount(leftToPay + totalPaid - amount));
    

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

void Loan::promptForMonthlyPayment()
{
  int na = QInputDialog::getInt(NULL, QObject::tr("Planned monthly payment "
                                                  "for load: %1").
                                arg(name),
                                QObject::tr("Enter the planned monthly "
                                            "payment for %1 "
                                            "(in cents)").
                                arg(name), effectiveMonthlyPayment);
  if(na != effectiveMonthlyPayment)
    plannedMonthlyPayment = na;
  updatePage();
}

void Loan::promptForRate()
{
  double na = 
    QInputDialog::getDouble(NULL, QObject::tr("Interest rate for loan: %1").
                            arg(name),
                            QObject::tr("Enter the interest rate of loan %1 "
                                        "(in percents)").
                            arg(name), yearlyRate*100, -100, 100, 3);
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
  Period p;
  p.endDate = QDate::currentDate();
  p.startDate = dateContracted;
  TransactionPtrList transactions = 
    targetPlugin->cabinet->wallet.
    transactionsForPeriod(p);

  for(int j = 0; j < transactions.size(); j++) {
    AtomicTransaction * t = transactions[j];
    if(t->hasNamedLinks("loan-payment"))
      continue;

    // Very simple ?
    if(t->getMemo().contains(matcher) || 
       t->getName().contains(matcher) ||
       t->getComment().contains(matcher)
       )
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
  // First, we convert all links named "loan-payment" into a real
  // transaction list
  QList<Link *> tr = links.namedLinks("loan-payment");

  matchingTransactions.clear();

  for(int i = 0; i < tr.size(); i++) {
    AtomicTransaction * t = dynamic_cast<AtomicTransaction *>(tr[i]->linkTarget());
    if(t)
      matchingTransactions << t;
  }
  matchingTransactions.sortByDate();

  /// @todo This shows that this function is called way way too often.
  // QTextStream o(stdout);
  // o << "Found " << matchingTransactions.size() << " transations" << endl;

  amountLeft = amount;
  totalPaid = 0;
  paidInterests = 0;
  monthsRunning = 0;

  if(! dateContracted.isValid())
    return ;

  int curMID = Transaction::monthID(QDate::currentDate());
  int trid = 0;

  lastMonthlyPayment = 0;

  // We use a month-based way to see the things
  for(int mid = Transaction::monthID(dateContracted) + 1; 
      mid <= curMID; mid++) {
    amountLeft = round(amountLeft * monthlyRate);
    while(trid < matchingTransactions.size() && 
          matchingTransactions[trid]->monthID() < mid)
      trid++;
    AtomicTransaction * cur = matchingTransactions.value(trid, NULL);
    if(cur && cur->monthID() == mid) {
      lastMonthlyPayment = -cur->getAmount();
      totalPaid -= cur->getAmount(); // Don't forget the amount is NEGATIVE !
      amountLeft += cur->getAmount();
      trid++;
    }
    monthsRunning += 1;
  }
  paidInterests = totalPaid + amountLeft - amount;

  remainingMonths = 0;
  leftToPay = 0;
  

  // Now, we enter the planification phase
  effectiveMonthlyPayment = (plannedMonthlyPayment < 0 ? lastMonthlyPayment : 
                             plannedMonthlyPayment);


  if(effectiveMonthlyPayment <= 0) {
    remainingMonths = -1;
    return;                     // Nothing to do here
  }

  int remaining = amountLeft;
  while(remaining > 0 && remaining <= amountLeft) {
    remaining = round(remaining * monthlyRate);
    int ma;
    if(remaining >= effectiveMonthlyPayment)
      ma = effectiveMonthlyPayment;
    else
      ma = remaining;
    remaining -= ma;
    remainingMonths += 1;
    leftToPay += ma;
  }
  if(remaining > 0)
    remainingMonths = -1;


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

bool LoanPlugin::hasBalance() const 
{
  return true;
}

int LoanPlugin::balance() const 
{
  int balance = 0;
  for(int i = 0; i < loans.size(); i++) {
    Loan * ln = const_cast<Loan*>(&loans[i]);
    /// @todo There should be a sane way to do that ;-)...
    /// 
    /// @todo Setup a cache for the computation of the balance (and
    /// the time-based ones ?)
    ln->html();                 // dirty ?
    balance -= ln->amountLeft;
  }
  return balance;
}
