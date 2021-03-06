/*
    filter.cc: The Filter class
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
#include <filter.hh>

SerializationAccessor * FilterElement::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("attribute", (int*)&transactionAttribute);
  ac->addScalarAttribute("regexp", &regexp);
  ac->addScalarAttribute("match", &match);
  return ac;
}

void FilterElement::prepareSerializationRead()
{
  /// @todo this is just a waste of time. Why SerializationAttributes
  /// don't have their own hooks too ?
  regexp = false;
}

bool FilterElement::matches(const Transaction * t) const
{
  QString targetString;
  switch(transactionAttribute) {
  case Name: targetString = t->getName();break;
  case Memo: targetString = t->getMemo();break;
  }

  if(regexp)
    return targetString.contains(QRegExp(match));
  else
    return targetString.contains(match);
}

//////////////////////////////////////////////////////////////////////

FilterAction::FilterAction(const QString & s) :
  actionType(FilterAction::SetCategory), what(s)
{
}

SerializationAccessor * FilterAction::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("action", (int*)&actionType);
  ac->addScalarAttribute("what", &what);
  return ac;
}

void FilterAction::performAction(Transaction * target) const
{
  switch(actionType) {
  case AddTag:
    target->setTagFromName(what);
    break;
  case SetCategory:
    if(!target->getCategory())
      target->setCategoryFromName(what);
    break;
  default:
    ;
  }
}

//////////////////////////////////////////////////////////////////////


Filter::Filter() : active(true), matchAny(true)
{
}

SerializationAccessor * Filter::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addScalarAttribute("name", &name);
  ac->addScalarAttribute("category", &category);
  ac->addScalarAttribute("matchany", &matchAny);
  ac->addScalarAttribute("active", &active);
  ac->addListAttribute("elements", &elements);
  ac->addListAttribute("actions", &actions);
  return ac;

}

void Filter::finishedSerializationRead()
{
  if(! category.isEmpty())
    actions << FilterAction(category);
  category = QString();
}

bool Filter::matches(const Transaction * t) const
{
  for(int i = 0; i < elements.size(); i++) {
    bool ok = elements[i].matches(t);
    if(matchAny && ok)
      return true; // OR logic and one match
    else if(! (matchAny || ok))
      return false; // AND logic and one non-match
  }
  return ! matchAny;
}

void Filter::performActions(Transaction * t) const
{
  for(int i = 0; i < actions.size(); i++)
    actions[i].performAction(t);
}

void Filter::processList(TransactionList * l) const
{
  for(int i = 0; i < l->size(); i++) {
    Transaction * t = &(*l)[i];
    if(matches(t))
      performActions(t);
  }
}

TransactionPtrList Filter::matchingTransactions(TransactionList * l) const
{
  TransactionPtrList ret;
  for(int i = 0; i < l->size(); i++)
    if(matches(&l->operator[](i)))
      ret << &l->operator[](i);
  return ret;
}
