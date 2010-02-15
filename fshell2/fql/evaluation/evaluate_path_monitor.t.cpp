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

/*! \file fshell2/fql/evaluation/evaluate_path_monitor.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:29:50 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>

#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/predicate_instrumentation.hpp>
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
#include <fshell2/fql/ast/pm_concat.hpp>
#include <fshell2/fql/ast/pm_filter_adapter.hpp>
#include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
// #include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
// #include <fshell2/fql/ast/tgs_intersection.hpp>
// #include <fshell2/fql/ast/tgs_setminus.hpp>
// #include <fshell2/fql/ast/tgs_union.hpp>

#include <fstream>

#include <cbmc/src/util/config.h>
#include <cbmc/src/util/tempfile.h>
#include <cbmc/src/langapi/language_ui.h>
#include <cbmc/src/goto-programs/goto_convert_functions.h>
#include <cbmc/src/langapi/mode.h>
#include <cbmc/src/ansi-c/ansi_c_language.h>

#define TEST_COMPONENT_NAME Evaluate_Path_Monitor
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
 * @test A test of Evaluate_Path_Monitor
 *
 */
void test( Test_Data & data )
{
	::std::string const tempname_str(::get_temporary_file("tmp.src", ".c"));
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
	
	Path_Monitor_Expr * pm_id(PM_Filter_Adapter::Factory::get_instance().create(
				Filter_Function::Factory::get_instance().create<F_IDENTITY>()));
	Path_Monitor_Expr * pm_id_kleene(PM_Repeat::Factory::get_instance().create(pm_id, 0, -1));
		
	Filter_Expr * bb(Filter_Function::Factory::get_instance().create<F_BASICBLOCKENTRY>());
	Edgecov * e(Edgecov::Factory::get_instance().create(bb,
				static_cast< Predicate::preds_t * >(0)));
	Test_Goal_Sequence::seq_t seq_list;
	seq_list.push_back(::std::make_pair<Path_Monitor_Expr *, Test_Goal_Set *>(pm_id_kleene, e));
	Test_Goal_Sequence * s(Test_Goal_Sequence::Factory::get_instance().create(seq_list, pm_id_kleene));

	Path_Monitor_Expr * pm_f(PM_Filter_Adapter::Factory::get_instance().create(bb));
	Path_Monitor_Expr * pm(PM_Concat::Factory::get_instance().create(pm_id_kleene,
					PM_Concat::Factory::get_instance().create(pm_f, pm_id_kleene)));

	Query * q(Query::Factory::get_instance().create(0, s, pm));

	Evaluate_Filter eval(gf, cfg);
	q->accept(&eval);
	Predicate_Instrumentation pred_inst(eval, gf, l);
	q->accept(&pred_inst);
	Evaluate_Path_Monitor pm_eval(eval, pred_inst);
	q->accept(&pm_eval);
	
	TEST_ASSERT_RELATION(4, ==, pm_eval.get(pm).state_count());
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
