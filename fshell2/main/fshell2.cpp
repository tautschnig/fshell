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

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#endif

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

#include <fshell2/config/features.hpp>

#ifdef FSHELL2_HAVE_LIBREADLINE
#  include <readline/readline.h>
#  include <readline/history.h>
#else
extern "C" {
#  include <linenoise/linenoise.h>
}
#define readline(prompt) linenoise(prompt)
#define read_history(f) linenoiseHistoryLoad(f)
#define add_history(x) linenoiseHistoryAdd(x)
#endif

#include <cbmc/bmc.h>
#include <minisat/utils/System.h>

namespace std {
template<>
auto_ptr<char>::~auto_ptr() throw()
{
	::free(this->release());
}
} 

FSHELL2_NAMESPACE_BEGIN;

FShell2::FShell2(::optionst & opts, ::goto_functionst & gf) :
	m_opts(opts), m_gf(gf), m_cmd(opts, gf), m_first_run(true) {
	// try to read history from file, ignore errors
	::read_history(".fshell2_history"); errno = 0;
}

FShell2::~FShell2() {
	// write the history and truncate it to 200 lines
#ifdef FSHELL2_HAVE_LIBREADLINE
	::write_history(".fshell2_history"); 
	::history_truncate_file(".fshell2_history", 200);
#else
	::linenoiseHistorySetMaxLen(200);
	::linenoiseHistorySave(".fshell2_history");
#endif
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
		::symbol_tablet m_context;
};

Context_Backup::Context_Backup(::language_uit & manager) :
	m_manager(manager), m_context(m_manager.symbol_table) {
}

Context_Backup::~Context_Backup() {
	m_manager.symbol_table.swap(m_context);
}

void FShell2::try_query(::language_uit & manager, char const * line,
		::fshell2::statistics::Statistics & stats)
{
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
	        satcheckt satcheck;
	        const namespacet ns(manager.symbol_table);
	        bv_cbmct bv_cbmc(ns, satcheck);
		::bmct bmc(m_opts, manager.symbol_table, manager.ui_message_handler, bv_cbmc);
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
	::fshell2::Constraint_Strengthening cs(equation, m_opts);
	::fshell2::Constraint_Strengthening::test_cases_t all_test_cases;
	::fshell2::statistics::Statistics * ts_stats(&stats);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_opts.get_unsigned_int_option(F2_O_MULTIPLE_COVERAGE) > 0);
	unsigned const mult(m_opts.get_unsigned_int_option(F2_O_MULTIPLE_COVERAGE));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_opts.get_unsigned_int_option(F2_O_LIMIT) >= 0);
	unsigned const limit(m_opts.get_unsigned_int_option(F2_O_LIMIT));

	::fshell2::statistics::Statistics aggregated_cpu_time;
	::fshell2::statistics::CPU_Timer * aggr_cpu_time(0);
	if (mult > 1) aggr_cpu_time = &(aggregated_cpu_time.new_stat<
			::fshell2::statistics::CPU_Timer >("Accumulated CPU time for multiple coverage"));

	for (unsigned idx(0); idx<mult; ++idx) {
		if (mult > 1) ts_stats = new ::fshell2::statistics::Statistics();
		if (aggr_cpu_time) aggr_cpu_time->start_timer();
		::fshell2::Constraint_Strengthening::test_cases_t test_suite;
		cs.generate(*goals, test_suite, limit, *ts_stats, all_test_cases);
		::fshell2::Constraint_Strengthening::test_cases_t::size_type const before_min(test_suite.size());

		// post-minimization
#if defined(__linux__) || defined(__APPLE__)
		NEW_STAT((*ts_stats), Counter< double >, min_mem_usage, "Minimization memory overhead");
		double const mem_peak(::Minisat::memUsedPeak());
#endif
		NEW_STAT((*ts_stats), CPU_Timer, min_cpu_timer, "Minimization CPU time overhead");
		min_cpu_timer.start_timer();
		::fshell2::Test_Suite_Minimization ts_min(manager);
		ts_min.minimize(test_suite);
		min_cpu_timer.stop_timer();
#if defined(__linux__) || defined(__APPLE__)
		min_mem_usage.inc(::Minisat::memUsedPeak() - mem_peak);
#endif
		NEW_STAT((*ts_stats), Counter< ::fshell2::Constraint_Strengthening::test_cases_t::size_type >, min_cnt, "Test cases removed by minimization");
		min_cnt.inc(before_min - test_suite.size());
		NEW_STAT((*ts_stats), Counter< ::fshell2::Constraint_Strengthening::test_cases_t::size_type >, tcs_cnt, "Test cases");
		tcs_cnt.inc(test_suite.size());

		// output
		::fshell2::Test_Suite_Output out(equation, m_opts);
		out.print_ts(test_suite, idx, os, manager.ui_message_handler.get_ui());

		all_test_cases.splice(all_test_cases.end(), test_suite);
		if (aggr_cpu_time) aggr_cpu_time->stop_timer();
		if (mult > 1) {
			if (m_opts.get_bool_option("statistics")) {
				ts_stats->print(manager);
				aggregated_cpu_time.print(manager);
			}
			delete ts_stats;
			ts_stats = &stats;
		}
	}

	of.close();
	timer.stop_timer();
}
	
bool FShell2::process_line(::language_uit & manager, char const * line)
{
	::fshell2::statistics::Statistics stats;
	bool const result(process_line(manager, line, stats));
	if (m_opts.get_bool_option("statistics"))
		stats.print(manager);
	return result;
}

bool FShell2::process_line(::language_uit & manager, char const * line,
			::fshell2::statistics::Statistics & stats)
{
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
			try_query(manager, line, stats);
			return false;
	}

	return true;
}

void FShell2::interactive(::language_uit & manager) {
	while (true) {
		// obtain the input from readline
		::std::auto_ptr<char> input(::readline("FShell2> "));
		if (input.get()) ::add_history(input.get());
		try {
			// process the input; returning true signals "quit"
			if (!input.get() || process_line(manager, input.get())) {
				manager.status() << "Bye." << messaget::eom;
				return;
			}
		} catch (::fshell2::Command_Processing_Error & e) {
			manager.error() << e.what() << messaget::eom;
		} catch (::fshell2::Macro_Processing_Error & e) {
			manager.error() << e.what() << messaget::eom;
		} catch (::fshell2::Query_Processing_Error & e) {
			manager.error() << e.what() << messaget::eom;
		} catch (::fshell2::FShell2_Error & e) {
			manager.error() << e.what() << messaget::eom;
		}
	}
}

FSHELL2_NAMESPACE_END;

