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
  RootItem(bool rec = true) {
    addChild(new TextModelItem("biniou"));
    addChild(new TextModelItem("bidule"));
    addChild(new TextModelItem("truc"));
    addChild(new TextModelItem(QStringList() << "machin" << "stuff"));
    if(rec)
      addChild(new RootItem(false));
  };

  virtual QVariant data(int column, int role) const {
    if(role == Qt::DisplayRole && column == 0)
      return "Root";
    return QVariant();
  };

  virtual int columnCount() const {
    return 1;
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
}

void OOTest::test()
{
  WidgetWrapperDialog * dlg = 
    new WidgetWrapperDialog(new OOTest, 
                            tr("test !"));
  dlg->show();
}

OOTest::~OOTest()
{
  delete model;
}
