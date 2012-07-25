/*
  modelitems.cc: model for transaction lists
  Copyright 2010, 2011 by Vincent Fourmond

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
#include <account.hh>
#include <modelitems.hh>

int ModelItem::myRow() const 
{
  ModelItem * p = parent();
  if(! p)
    return 0;
  return p->childIndex(this);
}

bool ModelItem::setData(int , const QVariant & , int)
{
  return false;
}

Qt::ItemFlags ModelItem::flags(int column) const
{
  return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

void ModelItem::trackChild(ModelItem * child)
{
  child->_parent = this;
  connect(child, SIGNAL(itemChanged(ModelItem *, int, int)),
          SIGNAL(itemChanged(ModelItem *, int, int)));

  // To avoid awkward problems, we force a direct connection, although
  // it may not be needed ?
  connect(child, SIGNAL(rowsChanged(ModelItem *)),
          SIGNAL(rowsChanged(ModelItem *)), Qt::DirectConnection);
  connect(child, SIGNAL(rowsWillChange(ModelItem *, int, int)),
          SIGNAL(rowsWillChange(ModelItem *, int, int)), 
          Qt::DirectConnection);
}

QVariant ModelItem::headerData(int , Qt::Orientation, int) const
{
  return QVariant();            // No header data by default.
}

bool ModelItem::popupContextMenu(const QPoint & /*pos*/)
{
  return false;
}

//////////////////////////////////////////////////////////////////////

FixedChildrenModelItem::FixedChildrenModelItem() :
  maxCols(0), maxColsIndex(0)
{
}

int FixedChildrenModelItem::childIndex(const ModelItem * child) const
{
  // The const_cast stuff really doesn't sound so great, but I guess
  // there isn't choice...
  return children.indexOf(const_cast<ModelItem*>(child));
}

ModelItem * FixedChildrenModelItem::childAt(int line)
{
  return children.value(line, NULL);
}

FixedChildrenModelItem::~FixedChildrenModelItem()
{
  for(int i = 0; i < children.size(); i++)
    delete children[i];
}

void FixedChildrenModelItem::insertChild(int index, ModelItem * child)
{
  if(index < 0)
    index = 0;
  if(index > children.size())
    index = children.size();

  emit(rowsWillChange(this, index, 1));
  children.insert(index, child);

  int cc = child->columnCount();
  if(maxCols < cc) {
    maxCols = cc;
    maxColsIndex = index;
  }
  else if(index <= maxColsIndex)
    ++maxColsIndex;
  trackChild(child);
  emit(rowsChanged(this));
}

void FixedChildrenModelItem::appendChild(ModelItem * child)
{
  insertChild(children.size(), child);
}

void FixedChildrenModelItem::removeChild(int id, int nb)
{
  /// @todo !!
}

int FixedChildrenModelItem::rowCount() const 
{
  return children.size();
}

int FixedChildrenModelItem::columnCount() const
{
  return maxCols;
}


//////////////////////////////////////////////////////////////////////

int LeafModelItem::childIndex(const ModelItem * ) const
{
  return -1;
}

ModelItem * LeafModelItem::childAt(int )
{
  return NULL;
}

int LeafModelItem::rowCount() const 
{
  return 0;
}


//////////////////////////////////////////////////////////////////////

TextModelItem::TextModelItem(const QStringList & c) : 
  columns(c), canEdit(false)
{
  
}

TextModelItem::TextModelItem(const QString & c, bool ce) :
  canEdit(ce)
{
  columns = QStringList() << c;
}

int TextModelItem::columnCount() const
{
  return columns.size();
}

QVariant TextModelItem::data(int column, int role) const
{
  if(role == Qt::DisplayRole || role == Qt::EditRole)
    return columns.value(column, "");
  return QVariant();
}


void TextModelItem::setText(const QString & str, int col)
{
  if(columns.size() <= col || col < 0)
    return;
  columns[col] = str;
  emit(itemChanged(this, col, col));
}

Qt::ItemFlags TextModelItem::flags(int column) const
{
  return Qt::ItemIsSelectable|Qt::ItemIsEnabled| 
    (canEdit ? Qt::ItemIsEditable : Qt::NoItemFlags);
}

bool TextModelItem::setData(int column, const QVariant & value,
                            int role)
{
  QTextStream o(stdout);
  o << "Editing item: " << this << " -- col: " << column 
    << " -- new value " << value.toString() << endl;
  if(role != Qt::EditRole)
    return false;
  if(! canEdit)
    return false;
  if(column >= columns.size() || column < 0)
    return false;
  setText(value.toString(), column);
  o << " -> success" << endl;
  return true;
}
