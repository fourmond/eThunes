/*
    attributehashwidget.cc: edition of AttributeHash
    Copyright 2017 by Vincent Fourmond

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
#include <attributehashwidget.hh>


AttributeHashElementWidget::
AttributeHashElementWidget(AttributeHash * tg,
                           const QString & element,
                           AttributeHash::HandledType initialType,
                           QWidget * parent) : QWidget(parent), target(tg), valueEditor(NULL)
{
  currentType = AttributeHash::LastType;
  layout = new QHBoxLayout(this);
  nameEditor = new QLineEdit(element);
  lastName = element;
  connect(nameEditor,
          SIGNAL(textEdited(const QString &)),
          SLOT(onNameChanged(const QString &)));
  layout->addWidget(nameEditor);

  typeEditor = new QComboBox;
  typeEditor->setEditable(false);
  for(int i = 0; i < AttributeHash::LastType; i++)
    typeEditor->addItem(AttributeHash::typeNames[i]);
  layout->addWidget(typeEditor);

  setType(initialType);

  connect(typeEditor,
          SIGNAL(currentIndexChanged(const QString &)),
          SLOT(onTypeChanged(const QString &)));
                    
  delButton = new QPushButton("-");
  layout->addWidget(delButton);
  connect(delButton, SIGNAL(clicked(bool)), SIGNAL(deletePressed()));
  
  addButton = new QPushButton("+");
  layout->addWidget(addButton);
  connect(addButton, SIGNAL(clicked(bool)), SIGNAL(addPressed()));
}

void AttributeHashElementWidget::setLocked(bool locked)
{
  bool enabled = !locked;
  nameEditor->setEnabled(enabled);
  typeEditor->setEnabled(enabled);
  delButton->setEnabled(enabled);
}

void AttributeHashElementWidget::setType(AttributeHash::HandledType type)
{
  if(type == currentType)
    return;
  delete valueEditor;
  currentType = type;
  valueEditor = AttributeHash::createEditor(currentType);
  layout->insertWidget(2, valueEditor, 1);
  if(target->contains(nameEditor->text()))
    AttributeHash::setEditorValue(currentType, valueEditor,
                                  (*target)[nameEditor->text()]);
  AttributeHash::connectEditorChanged(currentType, valueEditor,
                                      this, SLOT(onValueChanged()));
}

void AttributeHashElementWidget::onValueChanged()
{
  QString tg = nameEditor->text();
  if(! tg.isEmpty())
    (*target)[tg] = AttributeHash::getEditorValue(currentType, valueEditor);
}

void AttributeHashElementWidget::onNameChanged(const QString & ne)
{
  QVariant v = target->take(lastName);
  (*target)[ne] = v;
  lastName = ne;
}

void AttributeHashElementWidget::onTypeChanged(const QString & ne)
{
  setType(AttributeHash::namedType(ne));
}


//////////////////////////////////////////////////////////////////////

AttributeHashWidget::AttributeHashWidget(QWidget * parent)
{
  deleteMapper = new QSignalMapper;
  layout = new QVBoxLayout(this);
}

AttributeHashWidget::~AttributeHashWidget()
{
  delete deleteMapper;
}

void AttributeHashWidget::clear()
{
  for(AttributeHashElementWidget * ed : editors)
    delete ed;
  editors.clear();
}
                                

void AttributeHashWidget::editHash(AttributeHash * t,
                                   QHash<QString, AttributeHash::HandledType> fa)
{
  target = t;
  clear();
  fixedAttributes = fa;
  QSet<QString> keys = QSet<QString>::fromList(target->keys());
  for(const QString & n : fa.keys()) {
    AttributeHash::HandledType t = fa[n];
    AttributeHashElementWidget * ed =
      new AttributeHashElementWidget(target, n, t);
    ed->setLocked(true);
    deleteMapper->setMapping(ed, editors.size());
    editors << ed;
    keys.remove(n);
    layout->addWidget(ed);
  }

  for(const QString & n : keys) {
    AttributeHash::HandledType t = AttributeHash::variantType((*target)[n]);
    AttributeHashElementWidget * ed =
      new AttributeHashElementWidget(target, n, t);
    deleteMapper->setMapping(ed, editors.size());
    editors << ed;
    layout->addWidget(ed);
  }
}
