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
#include <util/arith_tools.h>
#include <util/std_expr.h>
#include <util/std_code.h>
#include <util/replace_symbol.h>
#include <ansi-c/c_types.h>
#include <ansi-c/ansi_c_typecheck.h>
#include <langapi/language_ui.h>
#include <goto-programs/goto_convert_functions.h>
#include <pointer-analysis/add_failed_symbols.h>
#include <goto-programs/remove_function_pointers.h>
#include <goto-programs/goto_inline.h>
#include <goto-programs/loop_ids.h>
#include <analyses/goto_check.h>
#include <linking/entry_point.h>

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

#include <fshell2/command/grammar.hpp>

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

Command_Processing::Command_Processing(::optionst & opts, ::goto_functionst & gf) :
	m_opts(opts), m_gf(gf),
	m_finalized(::config.main.empty() &&
				m_opts.get_int_option("max-argc") == 0),
	m_remove_zero_init(false) {
	if (::config.main.empty()) ::config.main = "main";
	m_opts.set_option(F2_O_LIMIT, 0);
	m_opts.set_option(F2_O_MULTIPLE_COVERAGE, 1);
}

::std::ostream & Command_Processing::help(::std::ostream & os) {
	os << "Control commands:" << ::std::endl << COMMAND_HELP
		<< "Comments start with `//' and end at the end of the line" << ::std::endl;
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
				m_finalized = false;
			}
			return DONE;
		case CMD_SET_LIMIT_COUNT:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, numeric_arg >= 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, numeric_arg > 0,
					"Limit must be greater than 0");
			m_opts.set_option(F2_O_LIMIT, numeric_arg);
			return DONE;
		case CMD_SET_NO_ZERO_INIT:
			m_remove_zero_init = true;	
			m_finalized = false;
			return DONE;
		case CMD_SET_ABSTRACT:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, arg != 0);
			{
				::symbol_tablet::symbolst::iterator sym_entry(
						manager.symbol_table.symbols.find(::std::string("c::") + arg));
				FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
						sym_entry != manager.symbol_table.symbols.end(),
						::std::string("Function ") + arg + " not found");
				sym_entry->second.value.make_nil();
				m_finalized = false;
			}
			return DONE;
		case CMD_SET_MULTIPLE_COVERAGE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, numeric_arg >= 0);
			FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error, numeric_arg > 1,
					"Multiplicity must be greater than 1");
			m_opts.set_option(F2_O_MULTIPLE_COVERAGE, numeric_arg);
			return DONE;
	}
			
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
	return NO_CONTROL_COMMAND;
}

void Command_Processing::remove_zero_init(::language_uit & manager) const {
	::symbol_tablet::symbolst::iterator init_iter(
	  manager.symbol_table.symbols.find("c::__CPROVER_initialize"));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						 init_iter != manager.symbol_table.symbols.end());

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

void Command_Processing::model_argv(::language_uit & manager) const {
	::symbol_tablet &symbol_table=manager.symbol_table;
	::namespacet const ns(symbol_table);

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, symbol_table.has_symbol("c::main"));
	locationt const& main_loc=symbol_table.symbols.find("c::main")->second.location;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, main_loc.is_not_nil());

	/*
	unsigned next=0;
	unsigned i=0;
   	__CPROVER_assume(argc'<=max_argc);
   	while(i<argc' && i<max_argc)
   	{
    	unsigned len;
     	__CPROVER_assume(len<4096);
     	__CPROVER_assume(next+len<4096);
     	argv'[i]=argv''+next;
     	argv''[next+len]=0;
     	next=next+len+1;
		i=i+1;
   	}
	*/
	int max_argc=m_opts.get_int_option("max-argc");
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, max_argc > 0);

	::symbol_tablet::symbolst::iterator main_iter(
	  symbol_table.symbols.find("main"));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						 main_iter != symbol_table.symbols.end());

	// turn argv' into a bounded array
	{
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, symbol_table.has_symbol("c::argv'"));
		symbol_table.symbols.find("c::argv'")->second.type.set(ID_size, from_integer(max_argc+1, index_type()));

		replace_symbolt replace_argvp;

		::symbolt const& argvp_symbol(ns.lookup("c::argv'"));
		symbol_exprt argvp(argvp_symbol.name, argvp_symbol.type);
		replace_argvp.insert(argvp_symbol.name, argvp);

		bool replaced=replace_argvp.replace(main_iter->second.value);
		assert(!replaced);
	}

	// add argv'' to the symbol table
	if(!symbol_table.has_symbol("c::main::1::argv''"))
	{
		// POSIX guarantees that at least 4096 characters can be passed as
		// arguments
		::array_typet argv_type(char_type(), from_integer(4096, index_type()));

		::symbolt argv_symbol;
		argv_symbol.pretty_name="argv''";
		argv_symbol.base_name="main::1::argv''";
		argv_symbol.name="c::main::1::argv''";
		argv_symbol.type=argv_type;
		argv_symbol.is_file_local=true;
		argv_symbol.is_type=false;
		argv_symbol.is_thread_local=false;
		argv_symbol.is_static_lifetime=false;
		argv_symbol.value.make_nil();
		argv_symbol.location=main_loc;

		symbol_table.add(argv_symbol);
	}

	// add counters/size
	::irep_idt locals[] = { "next", "i", "len'" };
	for(size_t i=0; i<sizeof(locals)/sizeof(::irep_idt); ++i)
	{
		::irep_idt name="c::main::1::"+id2string(locals[i]);
		if(symbol_table.has_symbol(name))
			continue;

		symbolt next_symbol;
		next_symbol.pretty_name=id2string(locals[i]);
		next_symbol.base_name="main::1::"+id2string(locals[i]);
		next_symbol.name=name;
		next_symbol.type=unsigned_short_int_type();
		next_symbol.is_file_local=true;
		next_symbol.is_type=false;
		next_symbol.is_thread_local=false;
		next_symbol.is_static_lifetime=false;
		next_symbol.is_lvalue=true;
		next_symbol.value.make_nil();
		next_symbol.location=main_loc;

		symbol_table.add(next_symbol);
	}

	/*
	char argv''[4096];
	*/
	::symbolt const& argvpp_symbol(ns.lookup("c::main::1::argv''"));
	symbol_exprt argvpp(argvpp_symbol.base_name, argvpp_symbol.type);
	code_declt argvpp_decl(argvpp);
	argvpp_decl.location()=main_loc;

	code_blockt code;

	/*
	unsigned next=0;
	unsigned i=0;
	*/
	for(int i=0; i<2; ++i)
	{
		::symbolt const& symbol(ns.lookup("c::main::1::"+id2string(locals[i])));
		::symbol_exprt symbol_expr(symbol.base_name, symbol.type);

		code_declt decl(symbol_expr);
		decl.location()=main_loc;
		code.move_to_operands(decl);

		code_assignt assign(symbol_expr, from_integer(0, unsigned_short_int_type()));
		assign.location()=main_loc;
		code.move_to_operands(assign);
	}

	/*
   	__CPROVER_assume(argc'<=max_argc);
	*/
	::symbolt const& argcp_symbol(ns.lookup("c::argc'"));
	symbol_exprt argcp("argc'", argcp_symbol.type); /* for some reason the base_name of c::argc' was set to argc */
	{
		binary_relation_exprt le(
		  argcp,
		  ID_le,
		  from_integer(max_argc, argcp_symbol.type));
		code_assumet assume(le);
		assume.location()=main_loc;
		code.move_to_operands(assume);
	}

	/*
   	while(i<argc' && i<max_argc)
   	{
    	unsigned len;
     	__CPROVER_assume(len<4096);
     	__CPROVER_assume(next+len<4096);
     	argv'[i]=argv''+next;
     	argv''[next+len]=0;
     	next=next+len+1;
		i=i+1;
   	}
	*/
	{
		::symbolt const& next_symbol(ns.lookup("c::main::1::next"));
		::symbol_exprt next(next_symbol.base_name, next_symbol.type);
		::symbolt const& len_symbol(ns.lookup("c::main::1::len'"));
		::symbol_exprt len(len_symbol.base_name, len_symbol.type);
		::symbolt const& i_symbol(ns.lookup("c::main::1::i"));
		symbol_exprt i(i_symbol.base_name, i_symbol.type);

		::symbolt const& argvp_symbol(ns.lookup("c::argv'"));
		symbol_exprt argvp(argvp_symbol.base_name, argvp_symbol.type);

		code_whilet while_loop;
		while_loop.location()=main_loc;

		while_loop.cond()=and_exprt(
		  binary_relation_exprt(i, ID_lt, argcp),
		  binary_relation_exprt(i, ID_lt, from_integer(max_argc, i_symbol.type)));

		while_loop.body()=code_blockt();

		code_declt decl(len);
		decl.location()=main_loc;
		while_loop.body().move_to_operands(decl);

		exprt e4096=from_integer(4096, len_symbol.type);
		binary_relation_exprt lt1(len, ID_lt, e4096);
		code_assumet assume1(lt1);
		assume1.location()=main_loc;
		while_loop.body().move_to_operands(assume1);
		binary_relation_exprt lt2(plus_exprt(next, len), ID_lt, e4096);
		code_assumet assume2(lt2);
		assume2.location()=main_loc;
		while_loop.body().move_to_operands(assume2);

		code_assignt assign1(
		  index_exprt(argvp, i),
		  plus_exprt(argvpp, next));
		assign1.location()=main_loc;
		while_loop.body().move_to_operands(assign1);

		code_assignt assign2(
		  index_exprt(argvpp, plus_exprt(next, len)),
		  from_integer(0, char_type()));
		assign2.location()=main_loc;
		while_loop.body().move_to_operands(assign2);

		code_assignt assign3(next, plus_exprt(plus_exprt(next, len), from_integer(1, unsigned_short_int_type())));
		assign3.location()=main_loc;
		while_loop.body().move_to_operands(assign3);

		code_assignt assign4(i, plus_exprt(i, from_integer(1, unsigned_short_int_type())));
		assign4.location()=main_loc;
		while_loop.body().move_to_operands(assign4);

		code.move_to_operands(while_loop);
	}

	ansi_c_parse_treet ansi_c_parse_tree;
	ansi_c_typecheckt ansi_c_typecheck(
	  ansi_c_parse_tree, symbol_table, "", manager.get_message_handler());
	try
	{
		side_effect_exprt expr(ID_statement_expression, empty_typet());
		expr.move_to_operands(code);

		ansi_c_typecheck.typecheck_expr(expr);
		code.swap(expr.op0());

		code_blockt dummyb;
		dummyb.move_to_operands(argvpp_decl);
		expr.op0()=dummyb;
		ansi_c_typecheck.typecheck_expr(expr);
		argvpp_decl.swap(to_code_block(to_code(expr.op0())).operands().front());
	}

	catch(int)
	{
		ansi_c_typecheck.error();
	}

	catch(const char *e)
	{
		ansi_c_typecheck.error(e);
	}

	catch(const std::string &e)
	{
		ansi_c_typecheck.error(e);
	}
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						 !ansi_c_typecheck.get_error_found());

	// find main() call and insert code before
	for (::exprt::operandst::iterator iter(main_iter->second.value.operands().begin());
		 iter != main_iter->second.value.operands().end();
		 ++iter)
	{
		if (iter->get(ID_statement) == ID_function_call &&
			to_symbol_expr(to_code_function_call(to_code(*iter)).function()).get_identifier() == "c::main")
		{
			::exprt::operandst items;

			items.push_back(argvpp_decl);
			items.push_back(code);

			main_iter->second.value.operands().insert(iter, items.begin(), items.end());

			code.make_nil();
			break;
		}
	}
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, code.is_nil());
}

bool Command_Processing::finalize(::language_uit & manager) {
	FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
			!manager.symbol_table.symbols.empty(),
			"No source files loaded!");
				
	::std::string entry("c::");
	entry += ::config.main;
	FSHELL2_PROD_CHECK1(::fshell2::Command_Processing_Error,
			manager.symbol_table.has_symbol(entry),
			::std::string("Could not find entry function " + ::config.main));
	
	if (m_finalized) return false;

	manager.symbol_table.remove(ID_main);
	
	m_finalized = ! ::entry_point(manager.symbol_table,
								  "c::main",
								  manager.ui_message_handler);
	// this must never fail, given all the previous sanity checks
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_finalized);

	// remove 0-initialization of global variables, if requested by user
	if (m_remove_zero_init) remove_zero_init(manager);

	// create init code for argv, if max-argc was set
	if (m_opts.get_int_option("max-argc") > 0 &&
		entry=="c::main" &&
		manager.symbol_table.has_symbol("c::argc'"))
		model_argv(manager);

	// convert all symbols; iterators are unstable, copy symbol names first
	::std::vector< ::irep_idt > symbols;
	for(::symbol_tablet::symbolst::iterator iter(manager.symbol_table.symbols.begin());
			iter != manager.symbol_table.symbols.end(); ++iter)
		if(iter->second.type.id() == ID_code)
			symbols.push_back(iter->first);
	
	::goto_convert_functionst converter(manager.symbol_table, m_gf,
			manager.ui_message_handler);
	for(::std::vector< ::irep_idt >::const_iterator iter(symbols.begin());
			iter != symbols.end(); ++iter) {
		::goto_functionst::function_mapt::iterator fct(m_gf.function_map.find(*iter));
		if (m_gf.function_map.end() != fct)
		{
			if(*iter!=ID_main && // main was possibly modified
			   fct->second.body_available && // ADD SOURCECODE may have added sth
			   manager.symbol_table.symbols.find(*iter)->second.value.is_not_nil()) //not ABSTRACT
				continue;
			m_gf.function_map.erase(fct);
		}
		converter.convert_function(*iter);
	}

	finalize_goto_program(manager);

	return true;
}

void Command_Processing::finalize_goto_program(::language_uit & manager) {

	manager.status("Function Pointer Removal");
	::remove_function_pointers(manager.symbol_table, m_gf, false);

	manager.status("Partial Inlining");
	// do partial inlining
	::namespacet ns(manager.symbol_table);
	::goto_partial_inline(m_gf, ns, manager.ui_message_handler);

	// add generic checks
	manager.status("Generic Property Instrumentation");
	::goto_check(ns, m_opts, m_gf);

	// add failed symbols
	::add_failed_symbols(manager.symbol_table);

	// recalculate numbers, etc.
	m_gf.update();

	// add loop ids
	m_gf.compute_loop_numbers();

	// show loops, if requested
	if (m_opts.get_bool_option("show-loops"))
		::show_loop_ids(manager.get_ui(), m_gf);
}

FSHELL2_COMMAND_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

