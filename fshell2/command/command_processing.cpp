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

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/invalid_argument.hpp>
#endif

#include <util/config.h>
#include <langapi/language_ui.h>
#include <goto-programs/goto_convert_functions.h>
#include <pointer-analysis/add_failed_symbols.h>
#include <goto-programs/remove_function_pointers.h>
#include <goto-programs/goto_inline.h>
#include <goto-programs/loop_numbers.h>

#include <cerrno>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>

/* parser */
#define yyFlexLexer CMDFlexLexer
#include <FlexLexer.h>

extern int CMDparse(CMDFlexLexer *,
		::fshell2::command::Command_Processing::parsed_command_t &, char **,
		int *, ::std::list< ::std::pair< char*, char* > > &);
// extern int yydebug;
/* end parser */

#define COMMAND_HELP \
"<Statement> ::= `QUIT' | `EXIT' | `HELP'" << ::std::endl << \
"              | `ADD' `SOURCECODE' <Defines> <File name>" << ::std::endl << \
"              | `SHOW' `FILENAMES'" << ::std::endl << \
"              | `SHOW' `SOURCECODE' <File name>" << ::std::endl << \
"              | `SHOW' `SOURCECODE' `ALL'" << ::std::endl << \
"              | `SET' <Options>" << ::std::endl << \
"<Options> ::= `ENTRY' <Identifier>" << ::std::endl << \
"            | `LIMIT' `COUNT' <Number>" << ::std::endl << \
"            | `NO_ZERO_INIT'" << ::std::endl << \
"            | `ABSTRACT' <Identifier>" << ::std::endl << \
"            | `MULTIPLE_COVERAGE' <Number>" << ::std::endl << \
"<File name> ::= <Singly Quoted String>" << ::std::endl << \
"<Defines> ::=" << ::std::endl << \
"            | `-D' <Identifier> <Defines>" << ::std::endl << \
"            | `-D' <Identifier> `=' <Singly Quoted String> <Defines>" << ::std::endl << \
"Comments start with `//' and end at the end of the line"

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_COMMAND_NAMESPACE_BEGIN;

::std::ostream & operator<<(::std::ostream & os, Command_Processing::status_t const& s) {
	switch (s) {
		case Command_Processing::NO_CONTROL_COMMAND:
			os << "NO_CONTROL_COMMAND";
			break;
		case Command_Processing::HELP:
			os << "HELP";
			break;
		case Command_Processing::QUIT:
			os << "QUIT";
			break;
		case Command_Processing::DONE:
			os << "DONE";
			break;
	}

	return os;
}

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

Command_Processing::Command_Processing(::optionst const& opts, ::goto_functionst & gf) :
	m_opts(opts), m_gf(gf), m_finalized(true),
	m_remove_zero_init(false), m_limit(0), m_multiple_coverage(1) {
	if (::config.main.empty()) ::config.main = "main";
}

::std::ostream & Command_Processing::help(::std::ostream & os) {
	os << "Control commands:" << ::std::endl << COMMAND_HELP << ::std::endl;
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
				::std::string("Failed to stat() file ") + name);
		FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, info.st_mtime <= entry->second,
				::std::string("File ") + name + " changed on disk");
	}
	// read the file using an ifstream
	::std::ifstream fs(name);
	// make sure the file could be opened
	FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, fs.is_open(),
			::std::string("Failed to open file ") + name);
	int lc(0);
	::std::string line;
	while(!::std::getline(fs, line).eof())
	{
		os << ++lc << "\t " << line << ::std::endl;
	}
	return os;
}
	
void Command_Processing::add_sourcecode(::language_uit & manager, char const * file,
		::std::list< ::std::pair< char*, char* > > const& defines) {
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
					::std::string(iter->first) + "=" + iter->second);
	// keep a private copy of previously loaded files
	::language_filest::filemapt prev_files;
	prev_files.swap(manager.language_files.filemap);
	// store the parse time to warn the user in case a modified file
	// is to be printed
	m_parse_time[file] = ::std::time(0);
	// attempt to parse the file
	// we don't even try to catch any exception thrown by CBMC here,
	// these would be fatal anyway
	bool err(manager.parse(file));
	// reset defines
	if (::config.ansi_c.defines.end() != last_global) {
		++last_global;
		::config.ansi_c.defines.erase(last_global, ::config.ansi_c.defines.end());
	}
	if (err) {
		manager.language_files.filemap.swap(prev_files);
		FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, false,
				::std::string("Failed to parse ") + file);
	}
	if (manager.typecheck()) {
		manager.language_files.filemap.swap(prev_files);
		FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, false,
				::std::string("Failed to typecheck ") + file);
	}
	// build the full list of loaded files
	manager.language_files.filemap.insert(prev_files.begin(), prev_files.end());
	// reset entry routine
	if (m_finalized) manager.context.remove(ID_main);
	m_finalized = false;
}

Command_Processing::status_t Command_Processing::process(::language_uit & manager,
		::std::ostream & os, char const * cmd) {
	while (0 != *cmd && ::std::isspace(*cmd)) ++cmd;
	if (0 == *cmd || ('/' == *cmd && '/' == *(cmd + 1))) return DONE;
		
	// new lexer
	CMDFlexLexer lexer;
	// put the input into a stream
	::std::istringstream is(cmd);
	// set the stream as the input to the parser
	lexer.switch_streams(&is, &os);
	// reset errno, readline for some reason sets this to EINVAL
	errno = 0;
	// information returned by parser
	parsed_command_t parsed_cmd(FAIL);
	char * arg(0);
	int numeric_arg(-1);
	::std::list< ::std::pair< char*, char* > > defines;
	Cleanup cleanup(&arg, defines);
	// yyparse returns 0 iff there was no error
	if (0 != CMDparse(&lexer, parsed_cmd, &arg, &numeric_arg, defines)) return NO_CONTROL_COMMAND;

	// parsing succeeded, what has to be done?
	switch (parsed_cmd) {
		case FAIL:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
			return NO_CONTROL_COMMAND;
		case CMD_HELP:
			return HELP;
		case CMD_QUIT:
			return QUIT;
		case CMD_ADD_SOURCECODE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			add_sourcecode(manager, arg, defines);
			return DONE;
		case CMD_SHOW_FILENAMES:
			for(::language_filest::filemapt::const_iterator iter(manager.language_files.filemap.begin());
					iter != manager.language_files.filemap.end(); ++iter)
				os << iter->first << ::std::endl;
			return DONE;
		case CMD_SHOW_SOURCECODE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			print_file_contents(os, arg);
			return DONE;
		case CMD_SHOW_SOURCECODE_ALL:
			for(::language_filest::filemapt::const_iterator iter(manager.language_files.filemap.begin());
					iter != manager.language_files.filemap.end(); ++iter)
				print_file_contents(os, iter->first.c_str());
			return DONE;
		case CMD_SET_ENTRY:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			{
				::config.main = arg;
				if (m_finalized) manager.context.remove(ID_main);
				m_finalized = false;
			}
			return DONE;
		case CMD_SET_LIMIT_COUNT:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, numeric_arg >= 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, numeric_arg > 0,
					"Limit must be greater than 0");
			m_limit = numeric_arg;
			return DONE;
		case CMD_SET_NO_ZERO_INIT:
			m_remove_zero_init = true;	
			m_finalized = false;
			return DONE;
		case CMD_SET_ABSTRACT:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			{
				::contextt::symbolst::iterator sym_entry(manager.context.symbols.find(
							::std::string("c::") + arg));
				FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
						sym_entry != manager.context.symbols.end(),
						::std::string("Function ") + arg + " not found");
				sym_entry->second.value.make_nil();
				m_finalized = false;
			}
			return DONE;
		case CMD_SET_MULTIPLE_COVERAGE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, numeric_arg >= 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, numeric_arg > 1,
					"Multiplicity must be greater than 1");
			m_multiple_coverage = numeric_arg;
			return DONE;
	}
			
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
	return NO_CONTROL_COMMAND;
}

bool Command_Processing::finalize(::language_uit & manager) {
	FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
			!manager.context.symbols.empty(),
			"No source files loaded!");
				
	::std::string entry("c::");
	entry += ::config.main;
	FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
			manager.context.has_symbol(entry),
			::std::string("Could not find entry function " + ::config.main));
	
	if (m_finalized) return false;
	
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			!manager.language_files.filemap.empty());
	m_finalized = ! manager.final();
	// this must never fail, given all the previous sanity checks
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_finalized);
    ::contextt::symbolst::iterator init_iter(manager.context.symbols.find("c::__CPROVER_initialize"));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			init_iter != manager.context.symbols.end());

	// remove 0-initialization of global variables, if requested by user
	if (m_remove_zero_init) {
		// check all operands
		for (::exprt::operandst::iterator iter(init_iter->second.value.operands().begin());
				iter != init_iter->second.value.operands().end();)
		{
			if (iter->get(ID_statement) == ID_assign &&
					iter->location().get_file() != "<built-in>") {
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 2 == iter->operands().size());
				if (iter->op1().get_bool(ID_C_zero_initializer)) {
					iter = init_iter->second.value.operands().erase(iter);
				} else {
					++iter;
				}
			} else {
				++iter;
			}
		}
	}
    

	// convert all symbols; iterators are unstable, copy symbol names first
	::std::vector< ::irep_idt > symbols;
	for(::contextt::symbolst::iterator iter(manager.context.symbols.begin()); iter !=
			manager.context.symbols.end(); ++iter)
		if(iter->second.type.id() == ID_code)
			symbols.push_back(iter->first);
	
	::goto_convert_functionst converter(manager.context, m_gf,
			manager.ui_message_handler);
	for(::std::vector< ::irep_idt >::const_iterator iter(symbols.begin());
			iter != symbols.end(); ++iter) {
		::goto_functionst::function_mapt::iterator fct(m_gf.function_map.find(*iter));
		if (m_gf.function_map.end() != fct) m_gf.function_map.erase(fct);
		converter.convert_function(*iter);
	}

	finalize_goto_program(manager);

	return true;
}

void Command_Processing::finalize_goto_program(::language_uit & manager) {
	::namespacet ns(manager.context);

	manager.status("Function Pointer Removal");
	::remove_function_pointers(ns, m_gf, false);

	manager.status("Partial Inlining");
	// do partial inlining
	::goto_partial_inline(m_gf, ns, manager.ui_message_handler);

	// add failed symbols
	::add_failed_symbols(manager.context);

	// recalculate numbers, etc.
	m_gf.update();

	// add loop ids
	m_gf.compute_loop_numbers();

	// show loops, if requested
	if (m_opts.get_bool_option("show-loops"))
		::show_loop_numbers(manager.get_ui(), m_gf);
}

FSHELL2_COMMAND_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

