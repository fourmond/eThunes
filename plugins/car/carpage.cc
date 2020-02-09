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

#include <evolvingitemwidget.hh>

/// The model class for displaying/editing the CarEvent
class CarEventModel : public QAbstractTableModel {
  WatchableList<CarEvent> * events;

public:

  /// The column
  class Column  {
  public:
    QString name;

    typedef std::function<QVariant (const CarEvent* event,
                                    int role)> DataFcn;
    DataFcn data;

    typedef std::function<bool (CarEvent* event,
                                const QVariant & data)> SetDataFcn;
    SetDataFcn setData;

    Column(const QString & n, const DataFcn & d,
           const SetDataFcn & s = 0) :
      name(n), data(d), setData(s) {;};
  };

  QList<Column> columns;


public:

  CarEventModel(WatchableList<CarEvent> * evs) :
    events(evs)
  {
    columns << Column("Date", [](const CarEvent* event,
                                 int role) -> QVariant {
                        if(role == Qt::DisplayRole || role == Qt::EditRole)
                          return event->date();
                        return QVariant();
                      })
            << Column("Amount", [](const CarEvent* event,
                                   int role) -> QVariant {
                        if(role == Qt::DisplayRole || role == Qt::EditRole) 
                          return Transaction::formatAmount(event->amount());
                        if(role == Qt::TextAlignmentRole)
                          return Qt::AlignRight;
                        return QVariant();
                      })
            << Column("Type", [](const CarEvent* event,
                                 int role) -> QVariant {
                        if(role == Qt::DisplayRole || role == Qt::EditRole) {
                          switch(event->type) {
                          case CarEvent::Purchase:
                            return "Purchase";
                          case CarEvent::Refuel:
                            return "Refuel";
                          case CarEvent::Repair:
                            return "Repair";
                          case CarEvent::Toll:
                            return "Toll";
                          case CarEvent::Other:
                            return "Other";
                          }
                        }
                        return QVariant();
                      })
            << Column("Kilometers", [](const CarEvent* event,
                                       int role) -> QVariant {
                        if(role == Qt::DisplayRole || role == Qt::EditRole) {
                          if(event->kilometers >= 0)
                            return event->kilometers;
                          if(event->interpolatedKilometers >=0)
                            return event->interpolatedKilometers;
                          return QVariant("??");
                        }
                        if(role == Qt::ForegroundRole) {
                          if(event->kilometers >= 0)
                            return QColor("black");
                          else
                            return QColor("gray");
                        }
                        return QVariant();
                      },
                      [](CarEvent * event, const QVariant & data) -> bool {
                        QVariant d = data;
                        if(! d.convert(QMetaType::Int))
                          return false;
                        event->kilometers = d.toInt();
                        return true;
                      }
                      )
            << Column("Fuel price", [](const CarEvent* event,
                                       int role) -> QVariant {
                        bool cpt = false;
                        int nb = event->fuelPrice(&cpt);
                        if(nb < 0)
                          return QVariant();
                        if(role == Qt::DisplayRole)
                          return Transaction::formatAmount(nb);
                        if(role == Qt::EditRole)
                          return nb;
                        if(role == Qt::ForegroundRole) {
                          if(cpt)
                            return QColor("gray");
                          else
                            return QColor("black");
                        }
                        return QVariant();
                      },
                      [](CarEvent * event, const QVariant & data) -> bool {
                        QVariant d = data;
                        if(! d.convert(QMetaType::Int))
                          return false;
                        event->pricePerLiter = d.toInt();
                        return true;
                      }
                      )
            << Column("Liters", [](const CarEvent* event,
                                       int role) -> QVariant {
                        bool cpt = false;
                        int nb = event->fuelLiters(&cpt);
                        if(nb < 0)
                          return QVariant();
                        if(role == Qt::DisplayRole)
                          return Transaction::formatAmount(nb);
                        if(role == Qt::EditRole)
                          return nb;
                        if(role == Qt::ForegroundRole) {
                          if(cpt)
                            return QColor("gray");
                          else
                            return QColor("black");
                        }
                        return QVariant();
                      },
                      [](CarEvent * event, const QVariant & data) -> bool {
                        QVariant d = data;
                        if(! d.convert(QMetaType::Int))
                          return false;
                        event->fuel = d.toInt();
                        return true;
                      })
      ;

    connect(*events, SIGNAL(numberChanged(const Watchdog *)),
            SIGNAL(layoutChanged()));
  }


  QVariant data(const CarEvent * event, int col, int role) const {
    if(col < 0 || col >= columns.size())
      return QVariant();
    QVariant v = columns[col].data(event, role);
    return v;
  };

  bool setData(CarEvent * event, int col, const QVariant & v) {
    if(col < 0 || col >= columns.size())
      return false;
    if(columns[col].setData)
      return columns[col].setData(event, v);
    return false;
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
  virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override {
    return events->size();
  }

  virtual int columnCount(const QModelIndex & parent = QModelIndex()) const override {
    return columns.size();
  };

  virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override {
    int col = index.column();
    const CarEvent * ev = eventForIndex(index);
    if(! ev)
      return QVariant();
    return data(ev, col, role);
  };

  virtual bool setData(const QModelIndex & index, const QVariant & v,
                           int role) override {
    int col = index.column();
    CarEvent * ev = eventForIndex(index);
    if(! ev)
      return false;
    return setData(ev, col, v);
  };

  virtual QVariant headerData(int section, Qt::Orientation orientation,
                              int role) const override {
    if(orientation == Qt::Horizontal) {
      if(role == Qt::DisplayRole) {
        return columns[section].name;
      }
      return QVariant();
    }
    return QVariant();
  }

  virtual Qt::ItemFlags flags(const QModelIndex &index) const override {
    int col = index.column();
    if(col < 0 || col >= columns.size())
      return Qt::NoItemFlags;
    if(columns[col].setData)
      return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
    else
      return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
  };
  /// @}

  void update() {
    emit(layoutChanged());
  };


  
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


  QTableView * eventsView = new QTableView();
  // eventsView->setContextMenuPolicy(Qt::CustomContextMenu);
  CarEventModel * model = new CarEventModel(&(plugin->car.events));
  eventsView->setModel(model);
  layout->addWidget(eventsView);

  // layout->addStretch();
  updatePage();
}

QString CarPage::pageTitle() 
{
  return tr("Car Plugin: %1").arg(plugin->getName());
}


static QHash<CarEvent::Type, QString> names =
  {
    {CarEvent::Purchase, "Purchase"},
    {CarEvent::Refuel, "Fuel"},
    {CarEvent::Repair, "Repairs"},
    {CarEvent::Toll, "Tolls"},
    {CarEvent::Other, "Other"}
  };


void CarPage::editDefaultPrice()
{
  EvolvingItem<int> newAmount = plugin->car.defaultPricePerLiter;
  WidgetWrapperDialog dlg(new EvolvingIntWidget(&newAmount),
                          tr("Price per liter"),
                          QDialogButtonBox::Ok|QDialogButtonBox::Cancel);
  dlg.setAttribute(Qt::WA_DeleteOnClose, false);

  if(dlg.exec() == QDialog::Accepted) {
    plugin->car.defaultPricePerLiter = newAmount;
    updatePage();
  }
}

void CarPage::editCO2Emissions()
{
  plugin->car.fuelCO2Emission = QInputDialog::getInt(NULL, "CO2 emissions",
                                                     "Enter the amount of emitted CO2 per liter (in grams)");
  
  updatePage();
}

void CarPage::updatePage()
{
  // Here, we do various things, basically to update the main summary.
  QString str;
  plugin->car.updateCache();
  str += tr("<h1>Car: %1</h1>\n").
    arg(plugin->getName());

  int total = 0;
  int km = plugin->car.kilometers();
  str += tr("Kilometers: %1<br/>").arg(km);

  for(CarEvent::Type t : ::names.keys()) {
    int tot = plugin->car.totals.value(t,0);
    total += tot;
    int perkm = tot*100/km;
    str += QString("<b>%1</b>: %2 total, %3 c per km <br/>").
      arg(names[t]).
      arg(Transaction::formatAmount(tot)).
      arg(Transaction::formatAmount(perkm));
  }

  str += QString("<b>Overall</b>: %1 total, %2 c per km <br/>").
    arg(Transaction::formatAmount(total)).
    arg(Transaction::formatAmount(total*100/km));

  str += "<b>Default price:</b> " +
    plugin->car.defaultPricePerLiter.toString(&Transaction::formatAmount)
    + HTTarget::linkToMember("(change)", this,
                             &CarPage::editDefaultPrice) + "<br/>\n";

  str += "<b>Consumption:</b> " +
    Transaction::formatAmount(plugin->car.liters) +
    " (total), " +
    Transaction::formatAmount(plugin->car.liters * 100/km) +
    " (per 100 km) <br/>";
  str += "<b>CO2 g per liter</b> "
    + QString::number(plugin->car.fuelCO2Emission) + " " 
    + HTTarget::linkToMember("(change)", this,
                             &CarPage::editCO2Emissions)
    + " total: " + QString::number(plugin->car.liters * 1e-8 *
                                   plugin->car.fuelCO2Emission)
    + " (tonnes) " + QString::number(plugin->car.liters * plugin->car.fuelCO2Emission * 0.01 / km) + " (g per km)<br/>";

  
  
  str += HTTarget::linkToMember("(update)", this,
                                &CarPage::updatePage);

  summary->setText(str);
}
