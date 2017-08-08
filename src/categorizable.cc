/*
    categorizable.cc: implementation of the Categorizable superclass
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
#include <categorizable.hh>
#include <cabinet.hh>
#include <wallet.hh>
#include <serializable-templates.hh>

#include <transactionlistdialog.hh>

Categorizable::Categorizable() :
  category(NULL)
{
  watchChild(&tags, "tags");
}

void Categorizable::addCategoriesSerialization(SerializationAccessor * ac)
{
  ac->addAccessorsAttribute("category",this, 
                            &Categorizable::setCategoryFromNamePrivate, 
                            &Categorizable::categoryName);
  ac->addAccessorsAttribute("tags",this, 
                            &Categorizable::setTagListPrivate, 
                            &Categorizable::tagString);
}

QString Categorizable::categoryName() const
{
  if(category)
    return category->fullName();
  return QString();
}

void Categorizable::setCategoryFromName(const QString & str, Wallet * w)
{
  if(! w)
    w = &Cabinet::globalCabinet()->wallet;
  setCategory(w->categories.namedSubCategory(str, true));
}

void Categorizable::setTagFromName(const QString & str, Wallet * w)
{
  if(! w)
    w = &Cabinet::globalCabinet()->wallet;
  Tag * t = w->tags.namedTag(str, true);
  setTag(t);
}


void Categorizable::setTagListPrivate(const QString & str) 
{
  setTagList(str, Wallet::walletCurrentlyRead);
}

void Categorizable::setCategoryFromNamePrivate(const QString & str) 
{
  setCategoryFromName(str, Wallet::walletCurrentlyRead);
}

void Categorizable::setTagList(const QString & str, Wallet * w)
{
  if(! w)
    w = &Cabinet::globalCabinet()->wallet;
  tags.fromString(str, w);
}

QVariant Categorizable::categoryData(int role)
{
  switch(role) {
  case Qt::DisplayRole:
  case Qt::EditRole:
    return categoryName();
  default:
    break;
  }
  return QVariant();
}

QVariant Categorizable::tagsData(int role)
{
  switch(role) {
  case Qt::DisplayRole:
  case Qt::EditRole:
    return tagString();
  default:
    break;
  }
  return QVariant();
}

static void fillOneCategory(QMenu * menu,
                     QList<Categorizable *> targets,
                     Category * category);

static void fillCategoryHash(QMenu * menu,
                             QList<Categorizable *> targets,
                             CategoryHash * ch)
{
  QStringList subCategories = ch->keys();
  subCategories.sort(); // Will be much easier to read !
  for(int i = 0; i < subCategories.count(); i++)
    fillOneCategory(menu, targets, &(*ch)[subCategories[i]]);
}

static void fillOneCategory(QMenu * menu,
                            QList<Categorizable *> targets,
                            Category * category)
{
  QAction * a = new QAction();
  QObject::connect(a, &QAction::triggered, [targets, category](bool) {
      for(auto t : targets)
        t->setCategory(category);
    }
    );

  if(category->subCategories.size()) {
    // Complex case:
    QMenu * subMenu = new QMenu(category->name);
    a->setText(QObject::tr("This category"));
    subMenu->addAction(a);
    subMenu->addSeparator();
    fillCategoryHash(subMenu, targets, &category->subCategories);
    menu->addMenu(subMenu);
  }
  else {
    a->setText(category->name);
    menu->addAction(a);
  }
}

static void fillTags(QMenu * menu, 
                     QList<Categorizable *> targets,
                     TagHash * tags, 
                     std::function< void(Tag * tag, Categorizable * c)> action)
{
  QStringList t = tags->tagNames();
  t.sort();
  for(int i = 0; i < t.count(); i++) {
    QAction * a = new QAction(t[i]);
    Tag * tag = &(*tags)[t[i]];
    QObject::connect(a, &QAction::triggered, [targets, action, tag](bool) {
        for(auto t : targets)
          action(tag, t);
      }
      );
    menu->addAction(a);
  }
  
}

void Categorizable::fillMenuWithCategorizableActions(QMenu * menu,
                                                     QList<Categorizable *> targets)
{
  QMenu * subMenu = new QMenu(QObject::tr("Set category"));

  Wallet * wallet = &(Cabinet::globalCabinet()->wallet);

  QAction * action = new QAction(QObject::tr("Clear"));
  QObject::connect(action, &QAction::triggered, [targets](bool) {
      for(auto t : targets)
        t->setCategory(NULL);
    }
    );
  subMenu->addAction(action);
  subMenu->addSeparator();
  fillCategoryHash(subMenu, targets, &wallet->categories);
  menu->addMenu(subMenu);

  action = new QAction(QObject::tr("Show category transactions"));
  QObject::connect(action, &QAction::triggered, [targets, wallet](bool) {
      if(targets.size() > 0) {
        Category * cat = targets.first()->getCategory();
        if(cat)
          TransactionListDialog::showCategory(cat, wallet);
      }
    });
  menu->addAction(action);


      
  if(wallet) {
    subMenu = new QMenu(QObject::tr("Clear tag"));
    fillTags(subMenu, targets, &wallet->tags, [](Tag * tag, Categorizable * c) {
        c->clearTag(tag);
      }
      );
    menu->addMenu(subMenu);

    subMenu = new QMenu(QObject::tr("Add tag"));
    fillTags(subMenu, targets, &wallet->tags, [](Tag * tag, Categorizable * c) {
        c->setTag(tag);
      }
      );
    menu->addMenu(subMenu);
  }

}
