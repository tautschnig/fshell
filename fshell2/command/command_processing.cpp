/* -*- Mode: C++; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 noexpandtab: */

/*******************************************************************************
 * FShell 2
 * Copyright 2009 Michael Tautschnig, tautschnig@forsyte.de
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *******************************************************************************/

/*! \file fshell2/command/command_processing.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:47 CEST 2009 
*/

#include <fshell2/command/command_processing.hpp>
#include <fshell2/config/annotations.hpp>
#include <fshell2/exception/command_processing_error.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>

#include <cbmc/src/util/config.h>

#include <cerrno>
#include <fstream>
#include <sys/stat.h>

/* parser */
#define yyFlexLexer CMDFlexLexer
#include <FlexLexer.h>

extern int CMDparse(CMDFlexLexer *,
		::fshell2::command::Command_Processing::status_t &, char **,
		int *, ::std::list< ::std::pair< char*, char* > > &);
// extern int yydebug;
/* end parser */

#define HELP \
"<Statement> ::= `QUIT' | `HELP'" << ::std::endl << \
"              | `ADD' `SOURCECODE' <Defines> <File name>" << ::std::endl << \
"              | `SHOW' `FILENAMES'" << ::std::endl << \
"              | `SHOW' `SOURCECODE' <File name>" << ::std::endl << \
"              | `SHOW' `SOURCECODE' `ALL'" << ::std::endl << \
"              | `SET' <Options>" << ::std::endl << \
"<Options> ::= `ENTRY' <Identifier>" << ::std::endl << \
"            | `LIMIT' `COUNT' <Number>" << ::std::endl << \
"<File name> ::= <Quoted String>" << ::std::endl << \
"<Defines> ::=" << ::std::endl << \
"            | `-D' <Identifier> <Defines>" << ::std::endl << \
"            | `-D' <Identifier> `=' <Quoted String> <Defines>" << ::std::endl << \
"Comments start with `//' and end at the end of the line"

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_COMMAND_NAMESPACE_BEGIN;

// cleanup char* stuff
class Cleanup {
	public:
		Cleanup(char ** a, ::std::list< ::std::pair< char*, char* > > & l);
		~Cleanup();
	private:
		char ** m_a;
		::std::list< ::std::pair< char*, char* > > & m_l;
};

Cleanup::Cleanup(char ** a, ::std::list< ::std::pair< char*, char* > > & l) :
	m_a(a), m_l(l) {
}

Cleanup::~Cleanup() {
	free(*m_a);

	while (!m_l.empty()) {
		free(m_l.front().first);
		m_l.front().first = 0;
		free(m_l.front().second);
		m_l.front().second = 0;
		m_l.erase(m_l.begin());
	}
}

Command_Processing::Command_Processing() {
}

Command_Processing::Command_Processing & Command_Processing::get_instance() {
	static Command_Processing instance;
	return instance;
}
	
::std::ostream & Command_Processing::help(::std::ostream & os) {
	os << "Control commands:" << ::std::endl << HELP << ::std::endl;
	return os;
}
	
::std::ostream & Command_Processing::print_file_contents(::std::ostream & os, char const * name) const {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Invalid_Argument, name != 0);
	// file might not correspond to what has been parsed, check file date
	::std::map< ::std::string, time_t >::const_iterator entry(m_parse_time.find(name));
	if (m_parse_time.end() == entry) {
		os << "WARNING: parse time of " << name
			<< " unknown, internal data may be inconsistent with file on disk!" << ::std::endl;
	} else {
		// try to obtain the file modification date
		struct stat info;
		FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, 0 == ::stat(name, &info),
				::diagnostics::internal::to_string("Failed to stat() file ", name));
		FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, info.st_mtimespec.tv_sec <= entry->second,
				::diagnostics::internal::to_string("File ", name, " changed on disk"));
	}
	// read the file using an ifstream
	::std::ifstream fs(name);
	// make sure the file could be opened
	FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, fs.is_open(),
			::diagnostics::internal::to_string("Failed to open file ", name));
	int lc(0);
	::std::string line;
	while(!::std::getline(fs, line).eof())
	{
		os << ++lc << "\t " << line << ::std::endl;
	}
	return os;
}

Command_Processing::status_t Command_Processing::process(::language_uit & manager,
		::std::ostream & os, char const * cmd) {
	while (0 != *cmd && ::std::isspace(*cmd)) ++cmd;
	if (0 == *cmd || ('/' == *cmd && '/' == *(cmd + 1))) return BLANK;
		
	// new lexer
	CMDFlexLexer lexer;
	// put the input into a stream
	::std::istringstream is(cmd);
	// set the stream as the input to the parser
	lexer.switch_streams(&is, &os);
	// reset errno, readline for some reason sets this to EINVAL
	errno = 0;
	// information returned by parser
	status_t status(NO_CONTROL_COMMAND);
	char * arg(0);
	int numeric_arg(-1);
	::std::list< ::std::pair< char*, char* > > defines;
	Cleanup cleanup(&arg, defines);
	// yyparse returns 0 iff there was no error
	if (0 != CMDparse(&lexer, status, &arg, &numeric_arg, defines)) return NO_CONTROL_COMMAND;

	// parsing succeeded, what has to be done?
	switch (status) {
		case CMD_HELP:
			break;
		case CMD_QUIT:
			break;
		case CMD_ADD_SOURCECODE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			{
				// defines -- keep a marker of global defines
				::std::list< ::std::string >::iterator last_global(::config.ansi_c.defines.end());
				if (!::config.ansi_c.defines.empty()) --last_global;
				// add the specific defines given with the ADD_SOURCE command
				if (!defines.empty())
					for (::std::list< ::std::pair<char*,char*> >::const_iterator iter(defines.begin());
							iter != defines.end(); ++iter)
						// CBMC internally does -D' and the closing ', thus we
						// generate define_ident=definition
						config.ansi_c.defines.push_back(
								::diagnostics::internal::to_string(iter->first, "=", iter->second));
				// store the parse time to warn the user in case a modified file
				// is to be printed
				m_parse_time[arg] = ::std::time(0);
				// attempt to parse the file
				try {
					bool err(manager.parse(arg));
					FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,!err,
							::diagnostics::internal::to_string("Failed to parse ", arg));
				} catch (::std::string & e) {
					FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,false,
							::diagnostics::internal::to_string("Failed to parse ", arg,
								": ", e));
				} catch (char const * e) {
					FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,false,
							::diagnostics::internal::to_string("Failed to parse ", arg,
								": ", e));
				}
				// reset defines
				if (::config.ansi_c.defines.end() != last_global) {
					++last_global;
					::config.ansi_c.defines.erase(last_global, ::config.ansi_c.defines.end());
				}
			}
			status = CMD_PROCESSED;
			break;
		case CMD_SHOW_FILENAMES:
			for(::language_filest::filemapt::const_iterator iter(manager.language_files.filemap.begin());
					iter != manager.language_files.filemap.end(); ++iter)
				os << iter->first << ::std::endl;
			status = CMD_PROCESSED;
			break;
		case CMD_SHOW_SOURCECODE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			print_file_contents(os, arg);
			status = CMD_PROCESSED;
			break;
		case CMD_SHOW_SOURCECODE_ALL:
			for(::language_filest::filemapt::const_iterator iter(manager.language_files.filemap.begin());
					iter != manager.language_files.filemap.end(); ++iter)
				print_file_contents(os, iter->first.c_str());
			status = CMD_PROCESSED;
			break;
		case CMD_SET_ENTRY:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
					manager.context.has_symbol(arg),
					::diagnostics::internal::to_string("Could not find entry function ", arg));
			::config.main = arg;
			status = CMD_PROCESSED;
			break;
		case CMD_SET_LIMIT_COUNT:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, numeric_arg >= 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, numeric_arg > 0,
					"Limit must be greater than 0");
			::config.fshell.max_test_cases = numeric_arg;
			status = CMD_PROCESSED;
			break;
		case CMD_PROCESSED:
		case NO_CONTROL_COMMAND:
		case BLANK:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
	}
	
	return status;
}

FSHELL2_COMMAND_NAMESPACE_END;
FSHELL2_NAMESPACE_END;
