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

/*! \file fshell2/fql/evaluation/automaton_inserter.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:32:07 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/evaluation/automaton_inserter.hpp>

#include <fshell2/instrumentation/cfg.hpp>

#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/repeat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>

#include <fstream>

#ifdef __linux__
#include <unistd.h>
#endif

#ifdef __FreeBSD_kernel__
#include <unistd.h>
#endif

#ifdef __GNU__
#include <unistd.h>
#endif

#ifdef __MACH__
#include <unistd.h>
#endif

#include <util/config.h>
#include <util/cmdline.h>
#include <util/tempfile.h>
#include <util/type_eq.h>
#include <langapi/language_ui.h>
#include <goto-programs/goto_convert_functions.h>
#include <langapi/mode.h>
#include <ansi-c/ansi_c_language.h>

#define TEST_COMPONENT_NAME Automaton_Inserter
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
 * @test A test of Automaton_Inserter
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
	::goto_functionst gf;

	TEST_CHECK(!l.parse(tempname_str));
	::unlink(tempname_str.c_str());
	TEST_CHECK(!l.typecheck());
	TEST_CHECK(!l.final());
    
	::goto_convert(l.context, gf, l.ui_message_handler);
	::fshell2::instrumentation::CFG cfg;
	cfg.compute_edges(gf);
		
	Path_Pattern_Expr * id_kleene(FQL_CREATE3(Repeat, FQL_CREATE1(Edgecov,
					FQL_CREATE_FF0(F_IDENTITY)), 0, -1));
	Coverage_Pattern_Expr * id_kleene_q(FQL_CREATE1(Quote, id_kleene));
		
	Filter_Expr * bb(FQL_CREATE_FF0(F_BASICBLOCKENTRY));
	Edgecov * e(FQL_CREATE1(Edgecov, bb));
	Coverage_Pattern_Expr * c(FQL_CREATE2(CP_Concat, id_kleene_q,
				FQL_CREATE2(CP_Concat, e, id_kleene_q)));

	Query * q(Query::Factory::get_instance().create(0, c, id_kleene));
	
	::fshell2::fql::Automaton_Inserter aut(l);
	aut.do_query(gf, cfg, *q);
	
	trace_automaton_t const& ta(aut.get_tg_aut());
	TEST_CHECK_RELATION(ta.state_count(), ==, 4);
	::namespacet const ns(l.context);
	::symbolt const& sym(ns.lookup("c::$fshell2$state$t_g"));
	TEST_ASSERT(::type_eq(sym.type, ::unsignedbv_typet(3), ns));
	
	for (trace_automaton_t::const_iterator iter(ta.begin()); iter != ta.end(); ++iter) {
		if (!aut.is_test_goal_state(*iter) || ta.delta2_backwards(*iter).empty()) {
#if FSHELL2_DEBUG__LEVEL__ >= 1
			TEST_THROWING_BLOCK_ENTER;
			aut.get_test_goal_instrumentation(*iter);
			TEST_THROWING_BLOCK_EXIT(::diagnostics::Invalid_Protocol);
#endif
			continue;
		}
		TEST_ASSERT(aut.get_test_goal_instrumentation(*iter).size() >= 1);
	}
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
#if FSHELL2_DEBUG__LEVEL__ >= 1
TEST_NORMAL_CASE( &test, LEVEL_DEBUG );
#else
TEST_NORMAL_CASE( &test, LEVEL_PROD );
#endif
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
