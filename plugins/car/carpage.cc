/*
  carpage.cc: Implementation of CarPage
  Copyright 2020 by Vincent Fourmond

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


#include <htlabel.hh>
#include <httarget-templates.hh>

#include "car.hh"
#include "carpage.hh"

/// The model class for displaying/editing the CarEvent
class CarEventModel : public QAbstractTableModel {
  QList<CarEvent> * events;

public:

  /// The column
  class Column  {
  public:
    QString name;

    typedef std::function<QVariant (const CarEvent* event,
                                    int role)> DataFcn;
    DataFcn data;
    
    Column(const QString & n, const DataFcn & d) :
      name(n), data(d) {;};
  };

  QList<Column> columns;


public:


  QVariant data(const CarEvent * event, int col, int role) const {
    if(col < 0 || col >= columns.size())
      return QVariant();
    return columns[col].data(event, role);
  };

  CarEvent * eventForIndex(const QModelIndex & idx) const {
    int row = idx.row();
    if(row < 0 || row >= events->size())
      return NULL;
    return &((*events)[row]);
  };
  
  /// @name Reimplemented interface
  ///
  /// @{
  virtual int rowCount(const QModelIndex & parent = QModelIndex()) const {
    return events->size();
  }

  virtual int columnCount(const QModelIndex & parent = QModelIndex()) const {
    return columns.size();
  };

  virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const {
    int col = index.column();
    const CarEvent * ev = eventForIndex(index);
    if(! ev)
      return QVariant();
    return data(ev, col, role);
  };

  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role) const {
    if(orientation == Qt::Horizontal) {
      if(role == Qt::DisplayRole) {
        return columns[section].name;
      }
      return QVariant();
    }
    return QVariant();
  }
  /// @}

  
};







//////////////////////////////////////////////////////////////////////

QHash<CarPlugin *, CarPage *> CarPage::carPages;

CarPage * CarPage::getCarPage(CarPlugin * plugin)
{
  if(! carPages.contains(plugin))
    carPages[plugin] = new CarPage(plugin);
  return carPages[plugin];
}


CarPage::CarPage(CarPlugin * p) : plugin(p)
{
  QVBoxLayout * layout = new QVBoxLayout(this);
  summary = new HTLabel();
  layout->addWidget(summary);


  layout->addStretch();
  updatePage();
}

QString CarPage::pageTitle() 
{
  return tr("Car Plugin: %1").arg(plugin->getName());
}



void CarPage::updatePage()
{
  // Here, we do various things, basically to update the main summary.
  QString str;
  str += tr("<h1>Car: %1</h1>\n").
    arg(plugin->getName());
  summary->setText(str);
}
