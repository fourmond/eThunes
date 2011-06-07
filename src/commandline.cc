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

#include <ruby-utils.hh>

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

void CommandLineParser::showHelpText(QTextStream & s)
{
  QStringList lst = options.keys();
  lst.sort();
  for(int i = 0; i < lst.size(); i++) {
    CommandLineOption * opt = options[lst[i]];
    s << "\t" << opt->longKey << "\t" << opt->helpText << endl;
  }
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

static void testDocumentLoading(const QStringList & a)
{
  QTextStream o(stdout);
  QStringList args = a;
  QString name = args.takeFirst();
  Ruby::ensureInitRuby();
  CollectionDefinition * def =
    CollectionDefinition::namedDefinition(name);
  if(! def) {
    o << "Unable to find the collection" << name << endl;
    return;
  }
  name = args.takeFirst();
  for(int i = 0; i < args.size(); i++) {
    QString file = args[i];
    AttributeHash contents = CollectionCode::readPDF(file);
    AttributeHash outAttrs = def->code.
      parseDocumentMetaData(name, contents);
    o << endl << "Parsed attributes for file " 
      << file << ": " << endl;
    outAttrs.dumpContents();
  }
}

static void testDocumentParsing(const QStringList & a)
{
  QTextStream o(stdout);
  QStringList args = a;
  Ruby::ensureInitRuby();
  for(int i = 0; i < args.size(); i++) {
    QString file = args[i];
    AttributeHash contents = CollectionCode::readPDF(file);
    o << "File " << file << "'s raw attributes: " << endl;
    contents.dumpContents();
  }
}

static void testDownload(const QStringList & a)
{
  QTextStream o(stdout);
  QStringList args = a;
  QString name = args.takeFirst();
  Ruby::ensureInitRuby();
  CollectionDefinition * def =
    CollectionDefinition::namedDefinition(name);
  if(! def) {
    o << "Unable to find the collection" << name << endl;
    return;
  }
  // Then, we build the collection:
  Collection * col = new Collection();
  col->definition = def;
  AttributeHash credentials;
  for(int i = 0; i < args.size(); i++) {
    QStringList b = args[i].split("=");
    if(b.size() > 1)
      credentials[b[0]] = b[1];
  }

  o << "Using credentials: " << endl;
  credentials.dumpContents();
  col->fetchNewDocumentsForUser(credentials);
  QCoreApplication::instance()->exec();
}


static void testXML(const QStringList & )
{
  /// @todo I will have to write a convenience function that can slurp
  /// the whole raw data (or almost) of an element.
  QTextStream o(stdout);
  QString str = "<element>truc<biniou><bidule name='machin'>bidule</biniou>machin</element><machin bidule='truc'/>";
  QXmlStreamReader xmlIn(str);
  xmlIn.readNext();             // Start document 
  QXmlStreamReader::TokenType t = xmlIn.readNext();
  o << "Token type just read: " << t << " : " 
    << xmlIn.name().toString() << endl; 
  QString s = xmlIn.readElementText(QXmlStreamReader::IncludeChildElements);
  o << "readElementText() gives: " << s << endl;
  t = xmlIn.readNext();
  o << "Token type just read: " << t << " : " 
    << xmlIn.name().toString() << endl; 
  
}


static CommandLineParser * parser = NULL;

static void showHelp(const QStringList & )
{
  QTextStream o(stdout);
  o << "Available options:" << endl;
  parser->showHelpText(o);
}

static CommandLineParser * myParser()
{
  CommandLineParser * parser = new CommandLineParser();
  *parser 
    << new CommandLineOption("--list-collections", listCollections,
			     0, "List collections")
    << new CommandLineOption("--help", showHelp,
			     0, "Shows this help")
    << new CommandLineOption("--test-document-load", testDocumentLoading,
			     -2, "Tries to load the given document")
    << new CommandLineOption("--test-document-parse", testDocumentParsing,
			     -1, "Parses the PDF, do not load as document")
    << new CommandLineOption("--test-download", testDownload,
			     -1, "Attempts to download new documents")
    << new CommandLineOption("--test-xml", testXML,
			     0, "Various internal tests for XML")
    << new CommandLineOption("--show-collection", showCollection,
			     1, "Shows the given collection in more details");
  return parser;
}


bool parseCommandLine()
{
  QStringList a = QCoreApplication::arguments();
  a.takeFirst();		// That's the program name
  if(a.size() > 0) {
    // Do processing. (we use the static version
    /// @todo use real object-oriented interface one day ?
    parser = myParser();
    parser->parseCommandLine(a);

    delete parser;
    parser = NULL;
    return true;
  }

  return false;
}
