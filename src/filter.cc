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
  ac->addAttribute("attribute",
		   new SerializationItemScalar<int>((int*)&transactionAttribute, true));
  ac->addAttribute("regexp",
		   new SerializationItemScalar<bool>(&regexp, true));
  ac->addAttribute("match",
		   new SerializationItemScalar<QString>(&match));
  return ac;

}

bool FilterElement::matches(Transaction * t)
{
  QString targetString;
  switch(transactionAttribute) {
  case Name: targetString = t->name;break;
  case Memo: targetString = t->memo;break;
  }

  if(regexp)
    return targetString.contains(QRegExp(match));
  else
    return targetString.contains(match);
}

Filter::Filter() : active(true), matchAny(true)
{
}

SerializationAccessor * Filter::serializationAccessor()
{
  SerializationAccessor * ac = new SerializationAccessor(this);
  ac->addAttribute("name",
		   new SerializationItemScalar<QString>(&name, true));
  ac->addAttribute("category",
		   new SerializationItemScalar<QString>(&category, true));
  ac->addAttribute("matchany",
		   new SerializationItemScalar<bool>(&matchAny, true));
  ac->addAttribute("active",
		   new SerializationItemScalar<bool>(&active, true));
  ac->addAttribute("elements",
		   new SerializationQList<FilterElement>(&elements));
  return ac;

}

bool Filter::matches(Transaction * t)
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

void Filter::processList(TransactionList * l)
{
  for(int i = 0; i < l->size(); i++) {
    Transaction * t = &l->operator[](i);
    if(!t->category && matches(t)) // Does not set when category
				   // exists, please override.
      t->setCategoryFromName(category);
  }
}
