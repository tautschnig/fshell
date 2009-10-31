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

/*! \file fshell2/fql/evaluation/compute_test_goals.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri May  1 21:45:44 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/evaluation/compute_test_goals.hpp>

#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>
#include <fshell2/fql/evaluation/automaton_inserter.hpp>

#include <fshell2/fql/ast/edgecov.hpp>
// #include <fshell2/fql/ast/filter_complement.hpp>
// #include <fshell2/fql/ast/filter_compose.hpp>
// #include <fshell2/fql/ast/filter_enclosing_scopes.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
// #include <fshell2/fql/ast/filter_intersection.hpp>
// #include <fshell2/fql/ast/filter_setminus.hpp>
// #include <fshell2/fql/ast/filter_union.hpp>
// #include <fshell2/fql/ast/pathcov.hpp>
// #include <fshell2/fql/ast/pm_alternative.hpp>
// #include <fshell2/fql/ast/pm_concat.hpp>
// #include <fshell2/fql/ast/pm_filter_adapter.hpp>
// #include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
// #include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
// #include <fshell2/fql/ast/tgs_intersection.hpp>
// #include <fshell2/fql/ast/tgs_setminus.hpp>
// #include <fshell2/fql/ast/tgs_union.hpp>

#include <fstream>

#include <cbmc/src/util/config.h>
#include <cbmc/src/langapi/language_ui.h>
#include <cbmc/src/goto-programs/goto_convert_functions.h>
#include <cbmc/src/langapi/mode.h>
#include <cbmc/src/ansi-c/ansi_c_language.h>

#define TEST_COMPONENT_NAME Compute_Test_Goals
#define TEST_COMPONENT_NAMESPACE fshell2::fql

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Compute_Test_Goals
 *
 */
void test( Test_Data & data )
{
	char * tempname(::strdup("/tmp/srcXXXXXX"));
	TEST_CHECK(-1 != ::mkstemp(tempname));
	::std::string tempname_str(tempname);
	tempname_str += ".c";
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
	::unlink(tempname);
	::free(tempname);

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
		
	Evaluate_Filter eval(gf, cfg);
	
	Filter_Expr * bb(Filter_Function::Factory::get_instance().create<F_BASICBLOCKENTRY>());
	Edgecov * e(Edgecov::Factory::get_instance().create(bb,
				static_cast< Predicate::preds_t * >(0)));
	Test_Goal_Sequence::seq_t seq_list;
	seq_list.push_back(::std::make_pair<Path_Monitor_Expr *, Test_Goal_Set *>(0, e));
	Test_Goal_Sequence * s(Test_Goal_Sequence::Factory::get_instance().create(seq_list, 0));
	Query * q(Query::Factory::get_instance().create(0, s, 0));
	q->accept(&eval);
	target_graph_t const& bb_entries(eval.get(*bb));
	TEST_CHECK_RELATION(6, ==, bb_entries.get_edges().size());

	::fshell2::fql::Evaluate_Path_Monitor pm_eval(eval);
	q->accept(&pm_eval);

	::fshell2::fql::Automaton_Inserter aut(pm_eval, eval, gf, cfg, l.context);
	aut.insert(*q);
	
	Compute_Test_Goals goals(l, options, gf, eval, pm_eval, aut);
	Compute_Test_Goals::test_goals_t const& bb_goals(goals.compute(*q));

	TEST_ASSERT_RELATION(6, ==, bb_goals.size());
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
