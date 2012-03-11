/*
  ootest.cc: test widget for OOModel
  Copyright 2012 by Vincent Fourmond

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
#include <ootest.hh>
#include <oomodel.hh>
#include <modelitems.hh>

#include <widgetwrapperdialog.hh>

class RootItem : public FixedChildrenModelItem {
  
public:
  RootItem(int rem = 1) {
    appendChild(new TextModelItem("biniou"));
    appendChild(new TextModelItem("bidule"));
    appendChild(new TextModelItem(QString("truc %1").arg(rem)));
    appendChild(new TextModelItem(QStringList() << "machin" << "stuff"));
    if(rem > 0)
      appendChild(new RootItem(rem - 1));
  };

  virtual QVariant data(int column, int role) const {
    if(role == Qt::DisplayRole && column == 0)
      return "Root";
    return QVariant();
  };

  void makeNew(const QString &s) {
    insertChild(1, new TextModelItem(s));
  };

  void modifyFirst() {
    static int nb = 0;
    TextModelItem * it = static_cast<TextModelItem*>(children.first());
    it->setText(QString("Modification #%1").arg(++nb));
  };

  virtual QVariant headerData(int column, Qt::Orientation /*orientation*/, 
                              int role) const {
    if(role != Qt::DisplayRole)
      return QVariant();
    return QString("column %1").arg(column);
  };

};


OOTest::OOTest()
{
  model = new OOModel(new RootItem);
  QVBoxLayout * layout = new QVBoxLayout(this);
  
  view = new QTreeView();
  view->setModel(model);
  layout->addWidget(view);
  view->setRootIndex(model->rootIndex());

  QPushButton * bt = new QPushButton("Change root !");
  connect(bt, SIGNAL(clicked()), SLOT(changeRoot()));
  layout->addWidget(bt);

  bt = new QPushButton("Append !");
  connect(bt, SIGNAL(clicked()), SLOT(appendChild()));
  layout->addWidget(bt);

  bt = new QPushButton("Modify !");
  connect(bt, SIGNAL(clicked()), SLOT(modifyChild()));
  layout->addWidget(bt);
}

void OOTest::test()
{
  WidgetWrapperDialog * dlg = 
    new WidgetWrapperDialog(new OOTest, 
                            tr("test !"));
  dlg->show();
}

void OOTest::changeRoot()
{
  model->setRoot(new RootItem(3));
  view->setRootIndex(model->rootIndex());
}

void OOTest::appendChild()
{
  static int idx = 0;           // Arg !
  RootItem * it = static_cast<RootItem *>(model->currentRoot());
  QString str = QString("new item %1").arg(++idx);
  it->makeNew(str);
  QTextStream o(stdout);
  o << "Trying to add item " << str << endl;
}

void OOTest::modifyChild()
{
  RootItem * it = static_cast<RootItem *>(model->currentRoot());
  it->modifyFirst();
}

OOTest::~OOTest()
{
  delete model;
}
