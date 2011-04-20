/*
    commandline.cc: implementation of command-line parsing
    Copyright 2011 by Vincent Fourmond

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
#include <commandline.hh>

// Now, stuff for handlers
#include <collection.hh>


void CommandLineOption::handle(QStringList & args)
{
  args.takeFirst();		// Remove the first argument, which
				// should be --longKey
  int nbreq = abs(numberNeeded);
  if(nbreq > args.size()) {
    throw "up";			// Hilarious, isn't it 
  }

  if(numberNeeded < 0) {
    handler(args);
    args.clear();
  }
  else {
    QStringList arglist;
    for(int i = 0; i < numberNeeded; i++)
      arglist.append(args.takeFirst());
    handler(arglist);
  }
}

void CommandLineParser::addOption(CommandLineOption * option)
{
  QString key;
  if(option->longKey.startsWith("--"))
    key = option->longKey;
  else
    key = QString("--") + option->longKey;
  options[key] = option;
}

void CommandLineParser::parseCommandLine(const QStringList & args)
{
  QStringList a = args;
  while(a.size() > 0) {
    CommandLineOption * opt = options.value(a.first(), NULL);
    if(! opt) {
      throw QString("Unkown command-line option: ") + a.first();
    }
    opt->handle(a);
  }
}

void CommandLineParser::parseCommandLine()
{
  QStringList a = QCoreApplication::arguments();
  a.takeFirst();		// That's the program name
  parseCommandLine(a);
}

////////////////////////////////////////////////////////////
// Now, we switch to the various actual command-line parsing.


// First, the handlers
static void listCollections(const QStringList &)
{
  QStringList defs = CollectionDefinition::availableDefinitions();
  QTextStream o(stdout);
  o << "Listing available collection definitions\n";
  for(int i = 0; i < defs.size(); i++) {
    o << defs[i] << endl;
  }
}

// First, the handlers
static void showCollection(const QStringList & s)
{
  QTextStream o(stdout);
  CollectionDefinition * def = 
    CollectionDefinition::namedDefinition(s.first());
  if(! def) {
    o << "Unable to find the collection" << s.first() << endl;
    return;
  }
  o << "Dumping collection " << def->name << ":" << endl
    << "\tpublic name: " << def->publicName  << endl
    << "\tdescription: " << def->description << endl;

  QStringList docs = def->documentTypes.keys();
  docs.sort();

  o << endl << "Available document types: " << endl;
  for(int i = 0; i < docs.size(); i++)
    o << " - " << docs[i] << endl;
}


static CommandLineParser * myParser()
{
  CommandLineParser * parser = new CommandLineParser();
  *parser 
    << new CommandLineOption("--list-collections", listCollections,
			     0, "List collections")
    << new CommandLineOption("--show-collection", showCollection,
			     1, "Shows the given collection in more details");
  return parser;
}

bool parseCommandLine()
{
  QStringList a = QCoreApplication::arguments();
  a.takeFirst();		// That's the program name
  if(a.size() > 0) {
    // Do processing.
    CommandLineParser * parser = myParser();
    parser->parseCommandLine(a);

    delete parser;
    return true;
  }

  return false;
}
