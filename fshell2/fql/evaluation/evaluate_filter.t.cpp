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

/*! \file fshell2/fql/evaluation/evaluate_filter.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr 30 13:31:31 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/evaluation/evaluate_filter.hpp>

#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/query.hpp>

#include <fstream>

#include <cbmc/src/util/config.h>
#include <cbmc/src/util/cmdline.h>
#include <cbmc/src/util/tempfile.h>
#include <cbmc/src/langapi/language_ui.h>
#include <cbmc/src/goto-programs/goto_convert_functions.h>
#include <cbmc/src/langapi/mode.h>
#include <cbmc/src/ansi-c/ansi_c_language.h>

#define TEST_COMPONENT_NAME Evaluate_Filter
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
 * @test A test of Evaluate_Filter
 *
 */
void test( Test_Data & data )
{
	::std::string const tempname_str(::get_temporary_file("tmp.src", ".c"));
	::std::ofstream of(tempname_str.c_str());
	TEST_CHECK(of.is_open());
	of << "int main(int argc, char * argv[])" << ::std::endl
		<< "{" << ::std::endl
		<< "if (argc > 2) return 2;" << ::std::endl
		<< "return 0;" << ::std::endl
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
	::fshell2::instrumentation::CFG cfg;
	cfg.compute_edges(gf);
	// gf.output(namespacet(l.context), ::std::cerr);
	Evaluate_Filter eval(l);
	
	{
		Filter_Expr * bb(FQL_CREATE_FF0(F_BASICBLOCKENTRY));
		Query * q(FQL_CREATE3(Query, 0, FQL_CREATE1(Edgecov, bb),
					FQL_CREATE1(Edgecov, FQL_CREATE_FF0(F_IDENTITY))));
		eval.do_query(gf, cfg, *q);
		target_graph_t const& bb_entries(eval.get(*bb));
		TEST_ASSERT_RELATION(2, ==, bb_entries.get_E().size());
	}

	{
		Filter_Expr * cc(FQL_CREATE_FF0(F_CONDITIONEDGE));
		Query * q(FQL_CREATE3(Query, 0, FQL_CREATE1(Edgecov, cc),
					FQL_CREATE1(Edgecov, FQL_CREATE_FF0(F_IDENTITY))));
		eval.do_query(gf, cfg, *q);
		target_graph_t const& cc_entries(eval.get(*cc));
		TEST_ASSERT_RELATION(2, ==, cc_entries.get_E().size());
	}
	
	{
		Filter_Expr * ff(FQL_CREATE_FF1(F_FILE, tempname_str));
		Query * q(FQL_CREATE3(Query, 0, FQL_CREATE1(Edgecov, ff),
					FQL_CREATE1(Edgecov, FQL_CREATE_FF0(F_IDENTITY))));
		eval.do_query(gf, cfg, *q);
		target_graph_t const& ff_entries(eval.get(*ff));
		TEST_ASSERT_RELATION(4, ==, ff_entries.get_E().size());
	}
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
