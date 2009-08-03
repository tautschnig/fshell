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
#include <fshell2/fql/parser/query_processing.hpp>
#include <fshell2/exception/query_processing_error.hpp>
#include <fshell2/fql/normalize/normalization_visitor.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/compute_test_goals.hpp>
#include <fshell2/instrumentation/goto_transformation.hpp>
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

#include <cbmc/src/util/std_code.h>
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
		~Query_Cleanup();
	private:
		::fshell2::fql::Query * m_q;
};

Query_Cleanup::Query_Cleanup(::fshell2::fql::Query * q) :
	m_q(q) {
}

Query_Cleanup::~Query_Cleanup() {
	m_q->destroy();
	m_q = 0;
}

void FShell2::try_query(::language_uit & manager, ::std::ostream & os, char const * line) {
	::std::string query(m_macro.expand(line));
	if (query.empty()) return;

	// there is some query string left, try to parse it
	::fshell2::fql::Query * query_ast(0);
	::fshell2::fql::Query_Processing::get_instance().parse(os, query.c_str(), &query_ast);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, query_ast != 0);
	Query_Cleanup cleanup(query_ast);
	
	// parse succeeded, make sure the CFG is prepared
	bool mod(m_cmd.finalize(manager, os));

	if (mod || m_first_run) {
		// code may have changed, check for failing assertions
		// we could also disable (using CBMC cmdline) assertions, but that must
		// include unwinding assertions et al.
		::bmct bmc(manager.context, manager.ui_message_handler);
		bmc.options = m_opts;
		bmc.set_verbosity(manager.get_verbosity());
		FSHELL2_PROD_CHECK1(::fshell2::FShell2_Error, !bmc.run(m_gf),
				"Program has failing assertions, cannot proceed.");
	}

	// normalize the input query
	::fshell2::fql::Normalization_Visitor norm;
	norm.normalize(&query_ast);

	// prepare filter evaluation
	::fshell2::fql::Evaluate_Filter eval(m_gf);
	// evaluate all filters before modifying the CFG
	query_ast->accept(&eval);

	// do automaton instrumentation
	// for now, only insert a final assert(0)
	if (mod || m_first_run) {
		::goto_programt tmp;
		::goto_programt::targett as(tmp.add_instruction(ASSERT));
		as->code = ::code_assertt();
		::exprt zero(::exprt("constant", ::typet("bool")));
		zero.set("value", "false");
		as->guard = zero;

		::fshell2::instrumentation::GOTO_Transformation inserter(m_gf);
		inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::END_FUNCTION, tmp);

		m_first_run = false;
	}

	/*
	// do automaton instrumentation
	::fshell2::instrumentation::Automaton_Inserter aut(prg_cfg, *query_ast);
	::goto_functionst instrumented_cfg;
	aut.instrument(instrumented_cfg);

	// build CFGs with abstraction
	::fshell2::instrumentation::Abstract_CFG_Builder abst(instrumented_cfg, *ast);
	::std::map< ::fshell2::fql::Abstraction const*, ::goto_functionst const > abst_map;
	abst.build(abst_map);

	// find proper strategy
	::fshell2::fql::Strategy_Selection_Visitor strat;
	::fshell2::fql::Strategy_Selection_Visitor::strategy_t s(strat.select(*ast));
	*/

	if (manager.get_verbosity() > 9) {
		::namespacet const ns(manager.context);
		m_gf.output(ns, os);
	}
	
	// compute test goals
	::fshell2::fql::Compute_Test_Goals goals(manager, m_opts, eval);

	// do the enumeration
	::fshell2::Constraint_Strengthening cs(goals);
	::fshell2::Constraint_Strengthening::test_cases_t test_suite;
	cs.generate(*query_ast, test_suite);

	// post-minimization
	::fshell2::Test_Suite_Minimization ts_min;
	ts_min.minimize(test_suite);

	// output
	::fshell2::Test_Suite_Output out(goals);
	out.print_ts(test_suite, os);
}

bool FShell2::process_line(::language_uit & manager, ::std::ostream & os, char const * line) {
	using ::fshell2::command::Command_Processing;

	switch (m_cmd.process(manager, os, line)) {
		case Command_Processing::QUIT:
			return true;
		case Command_Processing::HELP:
			Command_Processing::help(os);
			os << ::std::endl;
			::fshell2::macro::Macro_Processing::help(os);
			os << ::std::endl;
			::fshell2::fql::Query_Processing::help(os);
			return false;
		case Command_Processing::DONE:
			return false;
		case Command_Processing::NO_CONTROL_COMMAND:
			try_query(manager, os, line);
			return false;
	}

	return true;
}

void FShell2::interactive(::language_uit & manager, ::std::ostream & os) {
	while (true) {
		// obtain the input from readline
		::std::auto_ptr<char> input(::readline("FShell2> "));
		::add_history(input.get());
		try {
			// process the input; returning true signals "quit"
			if (process_line(manager, os, input.get())) {
				os << "Bye." << ::std::endl;
				return;
			}
		} catch (::fshell2::Command_Processing_Error & e) {
			os << e.what() << ::std::endl;
		} catch (::fshell2::Macro_Processing_Error & e) {
			os << e.what() << ::std::endl;
		} catch (::fshell2::Query_Processing_Error & e) {
			os << e.what() << ::std::endl;
		} catch (::fshell2::FShell2_Error & e) {
			os << e.what() << ::std::endl;
		}
	}
}

FSHELL2_NAMESPACE_END;

