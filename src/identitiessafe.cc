/*
    identitiessafe.cc: Implementation of IdentitiesSafe
    Copyright 2013 by Vincent Fourmond

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
#include <identitiessafe.hh>
#include <collection.hh>

IdentitiesSafe::~IdentitiesSafe()
{
  // Nothing to destroy, it seems...
}

class PasswordInput : public QDialog {
  QLineEdit * login;

  QLineEdit * password;
public:

  /// Returns the login
  QString getLogin() const {
    return login->text();
  };


  /// Returns the password
  QString getPassword() const {
    return password->text();
  };

  PasswordInput(const QString & title, const QString & label) {
    setWindowTitle(title);
    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel(label));

    QHBoxLayout * hb;

    hb = new QHBoxLayout;
    hb->addWidget(new QLabel(tr("User name")));
    login = new QLineEdit();
    hb->addWidget(login);
    layout->addLayout(hb);

    hb = new QHBoxLayout;
    hb->addWidget(new QLabel(tr("Password")));
    password = new QLineEdit();
    password->setEchoMode(QLineEdit::Password);
    hb->addWidget(password);
    layout->addLayout(hb);

    QPushButton * bt;
    hb = new QHBoxLayout;

    bt = new QPushButton(tr("OK"));
    connect(bt, SIGNAL(clicked()), SLOT(accept()));
    hb->addWidget(bt);

    bt = new QPushButton(tr("Cancel"));
    connect(bt, SIGNAL(clicked()), SLOT(reject()));
    hb->addWidget(bt);

    layout->addLayout(hb);
  };
};

AttributeHash IdentitiesSafe::getCredentialsForCollection(Collection * collection)
{
  AttributeHash user;
  
  PasswordInput input(QString("Credentials for %1").arg(collection->name),
                      QString("Enter credentials for collection %1").
                      arg(collection->name));
  if(input.exec() != QDialog::Accepted)
    throw "Stuff";

  user["login"] = input.getLogin();
  user["passwd"] = input.getPassword();

  return user;
}
