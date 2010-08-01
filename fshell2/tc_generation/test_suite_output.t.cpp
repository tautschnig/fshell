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

/*! \file fshell2/tc_generation/test_suite_output.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Aug  1 00:42:13 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/tc_generation/test_suite_output.hpp>

#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/predicate_instrumentation.hpp>
#include <fshell2/fql/evaluation/evaluate_path_pattern.hpp>
#include <fshell2/fql/evaluation/evaluate_coverage_pattern.hpp>
#include <fshell2/fql/evaluation/automaton_inserter.hpp>
#include <fshell2/fql/evaluation/compute_test_goals.hpp>
#include <fshell2/tc_generation/constraint_strengthening.hpp>
#include <fshell2/tc_generation/test_suite_minimization.hpp>

#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/repeat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>

#include <fstream>
#include <sys/stat.h>

#include <cbmc/src/util/config.h>
#include <cbmc/src/langapi/language_ui.h>
#include <cbmc/src/goto-programs/goto_convert_functions.h>
#include <cbmc/src/langapi/mode.h>
#include <cbmc/src/ansi-c/ansi_c_language.h>

#define TEST_COMPONENT_NAME Test_Suite_Output
#define TEST_COMPONENT_NAMESPACE fshell2

FSHELL2_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Test_Suite_Output
 *
 */
void test( Test_Data & data )
{
	// we need a fixed filename here for comparable results
	::std::string const tempname_str("__tmp.src.c");
	struct stat info;
	TEST_CHECK_RELATION(0, !=, ::stat(tempname_str.c_str(), &info));
	::std::ofstream of(tempname_str.c_str());
	TEST_CHECK(of.is_open());
	of << "int main(int argc, char * argv[])" << ::std::endl
		<< "{" << ::std::endl
		<< "int x=0;" << ::std::endl
		<< "if (argc > 5) x=2; else x=1;" << ::std::endl
		<< "if (argc > 27) ++x; else --x;" << ::std::endl
		<< "assert(0);" << ::std::endl
		<< "return x;" << ::std::endl
		<< "}" << ::std::endl;
	of.close();

	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l("FShell2", cmdline);
	::optionst options;
	options.set_option("assertions", true);
	::goto_functionst gf;

	TEST_CHECK(!l.parse(tempname_str));
	::unlink(tempname_str.c_str());
	TEST_CHECK(!l.typecheck());
	TEST_CHECK(!l.final());
    
	::goto_convert(l.context, options, gf, l.ui_message_handler);
	::fshell2::instrumentation::CFG cfg;
	cfg.compute_edges(gf);
		
	fql::Evaluate_Filter eval(gf, cfg, l);
	
	fql::Path_Pattern_Expr * id_kleene(FQL_CREATE3(Repeat, FQL_CREATE1(Edgecov,
					FQL_CREATE_FF0(F_IDENTITY)), 0, -1));
	fql::Coverage_Pattern_Expr * id_kleene_q(FQL_CREATE1(Quote, id_kleene));
		
	fql::Filter_Expr * bb(FQL_CREATE_FF0(F_BASICBLOCKENTRY));
	fql::Edgecov * e(FQL_CREATE1(Edgecov, bb));
	fql::Coverage_Pattern_Expr * c(FQL_CREATE2(CP_Concat, id_kleene_q,
				FQL_CREATE2(CP_Concat, e, id_kleene_q)));

	fql::Query * q(FQL_CREATE3(Query, 0, c, id_kleene));
	q->accept(&eval);
	fql::target_graph_t const& bb_entries(eval.get(*bb));
	TEST_CHECK_RELATION(6, ==, bb_entries.get_edges().size());

	::fshell2::fql::Evaluate_Path_Pattern pp_eval(eval, cfg);
	q->accept(&pp_eval);
	::fshell2::fql::Evaluate_Coverage_Pattern cp_eval(eval, pp_eval);
	q->accept(&cp_eval);
	TEST_CHECK_RELATION(1, ==, cp_eval.get_test_goal_states().m_children.back().m_children.front().m_tg_states.size());

	::fshell2::fql::Automaton_Inserter aut(pp_eval, cp_eval, gf, cfg, l);
	aut.insert(*q);

	fql::CNF_Conversion eq(l, options);
	eq.convert(gf);

	fql::Compute_Test_Goals_From_Instrumentation goals(eq, cp_eval, aut);
	q->accept(&goals);

	Constraint_Strengthening cs(eq);
	Constraint_Strengthening::test_cases_t test_suite;
	cs.generate(test_suite);
	Constraint_Strengthening::test_cases_t::size_type const size(test_suite.size());
	TEST_CHECK_RELATION(size, >=, 2);
	TEST_CHECK_RELATION(size, <=, 3);
	
	Test_Suite_Minimization ts_min;
	ts_min.minimize(test_suite);
	TEST_CHECK_RELATION(test_suite.size(), >, 0);
	TEST_CHECK_RELATION(test_suite.size(), <=, size);
	
	::config.main = "main";
	::std::string data_name("test_results_");
	char hname[256];
	TEST_CHECK(0 == ::gethostname(hname, 256));
	data_name += hname;
	Test_Suite_Output out(eq);
	::std::ostringstream os;
	out.print_ts(test_suite, os, ::ui_message_handlert::PLAIN);
	TEST_ASSERT(data.compare(data_name + "_plain_1", os.str()));
	::std::ostringstream os_xml;
	out.print_ts(test_suite, os_xml, ::ui_message_handlert::XML_UI);
	TEST_ASSERT(data.compare(data_name + "_xml_1", os_xml.str()));
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
