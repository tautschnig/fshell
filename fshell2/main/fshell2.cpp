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

/*! \file fshell2/main/fshell2.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Apr 15 11:58:46 CEST 2009 
*/

#include <fshell2/main/fshell2.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>

#include <fshell2/exception/command_processing_error.hpp>
#include <fshell2/exception/macro_processing_error.hpp>
#include <fshell2/exception/query_processing_error.hpp>
#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/fql/normalize/normalization_visitor.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>
#include <fshell2/fql/evaluation/automaton_inserter.hpp>
#include <fshell2/fql/evaluation/compute_test_goals.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/exception/fshell2_error.hpp>
#include <fshell2/tc_generation/constraint_strengthening.hpp>
#include <fshell2/tc_generation/test_suite_minimization.hpp>
#include <fshell2/tc_generation/test_suite_output.hpp>

#include <memory>
#include <cstdlib>
#include <cerrno>

#include <readline/readline.h>
#include <readline/history.h>

#include <cbmc/src/cbmc/bmc.h>

namespace std {
template<>
auto_ptr<char>::~auto_ptr()
{
	::free(this->release());
}
} 

FSHELL2_NAMESPACE_BEGIN;

FSHELL2_FQL_NAMESPACE_BEGIN;

class Query;
class Abstraction;

FSHELL2_FQL_NAMESPACE_END;

FShell2::FShell2(::optionst const& opts, ::goto_functionst & gf) :
	m_opts(opts), m_gf(gf), m_cmd(opts, gf), m_first_run(true) {
	// try to read history from file, ignore errors
	::read_history(".fshell2_history"); errno = 0;
}

FShell2::~FShell2() {
	// write the history and truncate it to 200 lines
	::write_history(".fshell2_history"); 
	// not yet available on OS X, thus left out for now
	// ::history_truncate_lines(".fshell2_history", 200);
}

class Query_Cleanup {
	public:
		Query_Cleanup(::fshell2::fql::Query * q);
		void set_object(::fshell2::fql::Query * q);
		~Query_Cleanup();
	private:
		::fshell2::fql::Query * m_q;
};

Query_Cleanup::Query_Cleanup(::fshell2::fql::Query * q) :
	m_q(q) {
}
		
void Query_Cleanup::set_object(::fshell2::fql::Query * q) {
	m_q = q;
}

Query_Cleanup::~Query_Cleanup() {
	m_q->destroy();
	m_q = 0;
}

class Context_Backup {
	public:
		Context_Backup(::language_uit & manager);
		~Context_Backup();
	private:
		::language_uit & m_manager;
		::contextt m_context;
};

Context_Backup::Context_Backup(::language_uit & manager) :
	m_manager(manager), m_context(m_manager.context) {
}

Context_Backup::~Context_Backup() {
	m_manager.context.swap(m_context);
}

class Smart_Printer {
	public:
		Smart_Printer(::language_uit & manager);
		~Smart_Printer();
		void print_now();
		::std::ostream & get_ostream();
	private:
		::language_uit & m_manager;
		::std::ostringstream m_oss;
};

Smart_Printer::Smart_Printer(::language_uit & manager) :
	m_manager(manager) {
}

Smart_Printer::~Smart_Printer() {
	if (!m_oss.str().empty()) m_manager.print(m_oss.str());
}

void Smart_Printer::print_now() {
	if (!m_oss.str().empty()) m_manager.print(m_oss.str());
	m_oss.str("");
}

::std::ostream & Smart_Printer::get_ostream() {
	return m_oss;
}

void FShell2::try_query(::language_uit & manager, char const * line) {
	::std::string query(m_macro.expand(line));
	if (query.empty()) return;

	// there is some query string left, try to parse it
	::fshell2::fql::Query * query_ast(0);
	{
		Smart_Printer smp(manager);
		m_fql_parser.parse(smp.get_ostream(), query.c_str(), &query_ast);
	}
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, query_ast != 0);
	Query_Cleanup cleanup(query_ast);
	
	// parse succeeded, make sure the CFG is prepared
	bool mod(m_cmd.finalize(manager));
	
	if (mod || m_first_run) {
		// code may have changed, check for failing assertions
		// we could also disable (using CBMC cmdline) assertions, but that must
		// include unwinding assertions et al.
		::bmct bmc(manager.context, manager.ui_message_handler);
		bmc.options = m_opts;
		bmc.set_verbosity(manager.get_verbosity());
		bmc.set_ui(manager.ui_message_handler.get_ui());
		FSHELL2_PROD_CHECK1(::fshell2::FShell2_Error, !bmc.run(m_gf),
				"Program has failing assertions, cannot proceed.");
		m_first_run = false;
	}

	// normalize the input query
	::fshell2::fql::Normalization_Visitor norm;
	norm.normalize(&query_ast);
	cleanup.set_object(query_ast);
	
	// copy goto program, it will be modified
	::goto_functionst gf_copy;
	gf_copy.copy_from(m_gf);
	// build a CFG to have forward and backward edges
	::fshell2::instrumentation::CFG cfg;
	cfg.compute_edges(gf_copy);

	// prepare filter evaluation
	::fshell2::fql::Evaluate_Filter filter_eval(gf_copy, cfg);
	// evaluate all filters before modifying the CFG
	query_ast->accept(&filter_eval);

	// build automata from path monitor expressions
	::fshell2::fql::Evaluate_Path_Monitor pm_eval(filter_eval);
	query_ast->accept(&pm_eval);
	
	// do automaton instrumentation
	Context_Backup context_backup(manager);
	::fshell2::fql::Automaton_Inserter aut(pm_eval, filter_eval, gf_copy, cfg, manager.context);
	aut.insert(*query_ast);

	/*
	// build CFGs with abstraction
	::fshell2::instrumentation::Abstract_CFG_Builder abst(instrumented_cfg, *ast);
	::std::map< ::fshell2::fql::Abstraction const*, ::goto_functionst const > abst_map;
	abst.build(abst_map);
	*/

	if (m_opts.get_bool_option("show-goto-functions")) {
		::namespacet const ns(manager.context);
		Smart_Printer smp(manager);
		gf_copy.output(ns, smp.get_ostream());
	}
	
	// compute test goals
	::fshell2::fql::Compute_Test_Goals goals(manager, m_opts, gf_copy, filter_eval, pm_eval, aut);

	// do the enumeration
	::fshell2::Constraint_Strengthening cs(goals);
	::fshell2::Constraint_Strengthening::test_cases_t test_suite;
	cs.generate(*query_ast, test_suite);

	// post-minimization
	::fshell2::Test_Suite_Minimization ts_min;
	ts_min.minimize(test_suite);

	// output
	::fshell2::Test_Suite_Output out(goals);
	out.print_ts(test_suite, ::std::cout, manager.ui_message_handler.get_ui());
}

bool FShell2::process_line(::language_uit & manager, char const * line) {
	using ::fshell2::command::Command_Processing;

	Smart_Printer smp(manager);
	switch (m_cmd.process(manager, smp.get_ostream(), line)) {
		case Command_Processing::QUIT:
			return true;
		case Command_Processing::HELP:
			Command_Processing::help(smp.get_ostream());
			smp.get_ostream() << ::std::endl;
			::fshell2::macro::Macro_Processing::help(smp.get_ostream());
			smp.get_ostream() << ::std::endl;
			::fshell2::fql::Query_Parser::help(smp.get_ostream());
			return false;
		case Command_Processing::DONE:
			return false;
		case Command_Processing::NO_CONTROL_COMMAND:
			smp.print_now();
			try_query(manager, line);
			return false;
	}

	return true;
}

void FShell2::interactive(::language_uit & manager) {
	while (true) {
		// obtain the input from readline
		::std::auto_ptr<char> input(::readline("FShell2> "));
		::add_history(input.get());
		try {
			// process the input; returning true signals "quit"
			if (process_line(manager, input.get())) {
				manager.status("Bye.");
				return;
			}
		} catch (::fshell2::Command_Processing_Error & e) {
			manager.error(e.what());
		} catch (::fshell2::Macro_Processing_Error & e) {
			manager.error(e.what());
		} catch (::fshell2::Query_Processing_Error & e) {
			manager.error(e.what());
		} catch (::fshell2::FShell2_Error & e) {
			manager.error(e.what());
		}
	}
}

FSHELL2_NAMESPACE_END;

