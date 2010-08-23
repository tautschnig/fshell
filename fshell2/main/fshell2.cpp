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
#include <fshell2/exception/fshell2_error.hpp>
#include <fshell2/util/smart_printer.hpp>
#include <fshell2/util/statistics.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/normalize/normalization_visitor.hpp>
#include <fshell2/fql/evaluation/compute_test_goals.hpp>
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

FShell2::FShell2(::optionst const& opts, ::goto_functionst & gf) :
	m_opts(opts), m_gf(gf), m_cmd(opts, gf), m_first_run(true) {
	// try to read history from file, ignore errors
	::read_history(".fshell2_history"); errno = 0;
}

FShell2::~FShell2() {
	// write the history and truncate it to 200 lines
	::write_history(".fshell2_history"); 
	::history_truncate_file(".fshell2_history", 200);
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
	
	// query parse succeeded, make sure the CFA is prepared
	bool const mod(m_cmd.finalize(manager));
	
	// if code changed, check for failing assertions; in the future, this should
	// be skipped and all extra checks be disabled using CBMC cmdline, but that
	// must include unwinding assertions et al.
	if (mod || m_first_run) {
		::bmct bmc(manager.context, manager.ui_message_handler);
		bmc.options = m_opts;
		bmc.set_verbosity(manager.get_verbosity());
		bmc.set_ui(manager.ui_message_handler.get_ui());
		FSHELL2_PROD_CHECK1(::fshell2::FShell2_Error, !bmc.run(m_gf),
				"Program has failing assertions, cannot proceed.");
		m_first_run = false;
	}

	::std::string const& output(m_opts.get_option("outfile"));
	::std::ofstream of;
	if (!output.empty() && output != "-") {
		of.open(output.c_str(), ::std::ios::out | ::std::ios::app);
		FSHELL2_PROD_CHECK1(::fshell2::FShell2_Error, of.is_open(),
				"Failed to open output file " + output);
	}
	::std::ostream & os((output.empty() || output == "-") ? ::std::cout : of); 

	// collect per-query statistics
	::fshell2::statistics::Statistics stats;
	NEW_STAT(stats, CPU_Timer, timer, "Query CPU time");
	timer.start_timer();

	// normalize the input query
	::fshell2::fql::Normalization_Visitor norm;
	norm.normalize(&query_ast);
	cleanup.set_object(query_ast);
	
	// compute test goals
	Context_Backup context_backup(manager);
	::std::auto_ptr< ::fshell2::fql::Compute_Test_Goals > goals(0);
	if (m_opts.get_bool_option("use-instrumentation"))
		goals.reset(new ::fshell2::fql::Compute_Test_Goals_From_Instrumentation(manager, m_opts));
	else
		goals.reset(new ::fshell2::fql::Compute_Test_Goals_Boolean(manager, m_opts, stats));
	// copy goto program, it will be modified
	::goto_functionst gf_copy;
	gf_copy.copy_from(m_gf);
	::fshell2::fql::CNF_Conversion & equation(goals->do_query(gf_copy, *query_ast));

	// do the enumeration
	::fshell2::Constraint_Strengthening cs(equation, stats, m_opts);
	::fshell2::Constraint_Strengthening::test_cases_t test_suite;
	cs.generate(test_suite);

	// post-minimization
	::fshell2::Test_Suite_Minimization ts_min;
	ts_min.minimize(test_suite);

	// output
	::fshell2::Test_Suite_Output out(equation);
	out.print_ts(test_suite, os, manager.ui_message_handler.get_ui());
	of.close();

	timer.stop_timer();
	if (m_opts.get_bool_option("statistics"))
		stats.print(manager);
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

