/**
    \file commandline.hh
    Command-line parsing
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


#ifndef __COMMANDLINE_HH
#define __COMMANDLINE_HH

/// A command-line option
///
/// This class represents a command-line option such as --help
///
/// As of now short keys are not supported.
///
/// @todo: create a derived class
class CommandLineOption {
public:

  /// The handler for a given option
  typedef void (* CommandHandler)(const QStringList & args);

  /// The long key, including --
  QString longKey;

  /// The number of arguments needed. If negative, it means at least
  /// -numberNeeded but it slurps up all anyway 
  int numberNeeded;

  /// Help text
  QString helpText;


  /// The function to run to process the option
  CommandHandler handler;

  /// Whether the option is a command or just an option. In the former
  /// case, the default, the GUI is not displayed after command-line
  /// processing.
  bool isCommand;

  CommandLineOption(const QString & k, CommandHandler h,
		    int nb, const QString & help = "",
                    bool cmd = true) : 
    longKey(k), numberNeeded(nb), helpText(help),
    handler(h), isCommand(cmd)
  {
  };

  /// Handles the given arguments. It modifies the string list to
  /// leave only what it hasn't touched yet.
  ///
  /// It expects args[0] to be the key.
  ///
  /// @todo can be made virtual one day to support 'call-to-member'
  /// handlers and other funny things.
  void handle(QStringList & args);

};

/// The actual parser for the command line
///
/// @todo add a fall-through handler
///
/// @todo it would be nice to separate between options and commands,
/// ie, things that will only modify the behaviour of the program
/// versus those that trigger another completely unrelated thing (for
/// testing, for instance). A simple flag in CommandLineOption would
/// be neat.
class CommandLineParser {

  /// The --k -indexed options.
  QHash<QString, CommandLineOption *> options;
public:

  /// Whether the CommandLineParser has processed commands.
  bool processedCommands;

  CommandLineParser() : processedCommands(false) { 
    ;
  };

  /// Adds an option to the parser.
  void addOption(CommandLineOption * option);

  CommandLineParser & operator<<(CommandLineOption * option) {
    addOption(option);
    return *this;
  };

  /// Parses the given command-line (only arguments should be given)
  void parseCommandLine(const QStringList & args);

  /// Parsers the application-wide command-line.
  void parseCommandLine();

  /// Sends help text to the given stream
  void showHelpText(QTextStream & s);

 
};

/// This function parses the command-line and returns true if anything
/// was done, in which case the GUI should not be displayed.
/// Wow, that's what I call not object-oriented. But that
/// should be mostly allright.
///
/// @todo maybe this function and the related code (handlers) should
/// go to its own namespace/file ?
bool parseCommandLine();

#endif
