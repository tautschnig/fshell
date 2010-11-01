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

/*! \file fshell2/tc_generation/constraint_strengthening.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun May  3 21:42:26 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/tc_generation/constraint_strengthening.hpp>

#include <fshell2/fql/evaluation/compute_test_goals.hpp>

#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/repeat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>

#include <fshell2/util/statistics.hpp>

#include <fstream>

#include <cbmc/src/util/config.h>
#include <cbmc/src/util/tempfile.h>
#include <cbmc/src/langapi/language_ui.h>
#include <cbmc/src/goto-programs/goto_convert_functions.h>
#include <cbmc/src/langapi/mode.h>
#include <cbmc/src/ansi-c/ansi_c_language.h>

#define TEST_COMPONENT_NAME Constraint_Strengthening
#define TEST_COMPONENT_NAMESPACE fshell2

FSHELL2_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Constraint_Strengthening
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
		<< "return x;" << ::std::endl
		<< "}" << ::std::endl;
	of.close();

	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l("FShell2", cmdline);
	::optionst options;
	::goto_functionst gf;

	TEST_CHECK(!l.parse(tempname_str));
	::unlink(tempname_str.c_str());
	TEST_CHECK(!l.typecheck());
	TEST_CHECK(!l.final());
    
	::goto_convert(l.context, options, gf, l.ui_message_handler);
		
	fql::Path_Pattern_Expr * id_kleene(FQL_CREATE3(Repeat, FQL_CREATE1(Edgecov,
					FQL_CREATE_FF0(F_IDENTITY)), 0, -1));
	fql::Coverage_Pattern_Expr * id_kleene_q(FQL_CREATE1(Quote, id_kleene));
	fql::Filter_Expr * bb(FQL_CREATE_FF0(F_BASICBLOCKENTRY));
	fql::Edgecov * e(FQL_CREATE1(Edgecov, bb));
	fql::Coverage_Pattern_Expr * c(FQL_CREATE2(CP_Concat, id_kleene_q,
				FQL_CREATE2(CP_Concat, e, id_kleene_q)));
	fql::Query * q(FQL_CREATE3(Query, 0, c, id_kleene));
	
	fql::Compute_Test_Goals_From_Instrumentation goals(l, options);
	fql::CNF_Conversion & eq(goals.do_query(gf, *q));
	TEST_CHECK_RELATION(6, ==, eq.get_test_goal_id_map().size());

	statistics::Statistics stats;
	Constraint_Strengthening cs(eq, options);
	Constraint_Strengthening::test_cases_t test_suite, atc;
	options.set_option("sat-coverage-check", true);
	cs.generate(goals, test_suite, 0, stats, atc);
	TEST_ASSERT_RELATION(test_suite.size(), >=, 2);
	TEST_ASSERT_RELATION(test_suite.size(), <=, 3);
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
