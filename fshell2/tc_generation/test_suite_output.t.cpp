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

#include <fshell2/util/statistics.hpp>

#include <fstream>
#include <sys/stat.h>

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
#include <langapi/language_ui.h>
#include <goto-programs/goto_convert_functions.h>
#include <langapi/mode.h>
#include <ansi-c/ansi_c_language.h>

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
void test_sym_classes( Test_Data & data )
{
	// we need a fixed filename here for comparable results
	::std::string const tempname_str("__tmp.src.c");
	struct stat info;
	TEST_CHECK_RELATION(0, !=, ::stat(tempname_str.c_str(), &info));
	::std::ofstream of(tempname_str.c_str());
	TEST_CHECK(of.is_open());
	of << "int shared;" << std::endl
		<< "int normal_fun(int y)" << std::endl
		<< "{" << std::endl
		<< "if(y) return 1;" << std::endl
		<< "return 0;" << std::endl
		<< "}" << std::endl
		<< "static int local_fun(int x)" << std::endl
		<< "{" << std::endl
		<< "int ls;" << std::endl
		<< "if(ls) ls=1;" << std::endl
		<< "return ls;" << std::endl
		<< "}" << std::endl
		<< "int main(int argc, char* argv[])" << std::endl
		<< "{" << std::endl
		<< "int ln;" << std::endl
		<< "static int stat;" << std::endl
		<< "if(ln) local_fun(ln);" << std::endl
		<< "if(stat) normal_fun(stat);" << std::endl
		<< "return stat;" << std::endl
		<< "}" << std::endl;
	of.close();

	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	options.set_option("stop-on-fail", true);
	::goto_functionst gf;

	TEST_CHECK(!l.parse(tempname_str));
	::unlink(tempname_str.c_str());
	TEST_CHECK(!l.typecheck());
	TEST_CHECK(!l.final());

	::goto_convert(l.symbol_table, gf, l.ui_message_handler);

	fql::Path_Pattern_Expr * id_kleene(FQL_CREATE3(Repeat, FQL_CREATE1(Edgecov,
					FQL_CREATE_FF0(F_IDENTITY)), 0, -1));
	fql::Coverage_Pattern_Expr * id_kleene_q(FQL_CREATE1(Quote, id_kleene));
	fql::Filter_Expr * cc(FQL_CREATE_FF0(F_CONDITIONEDGE));
	fql::Edgecov * e(FQL_CREATE1(Edgecov, cc));
	fql::Coverage_Pattern_Expr * c(FQL_CREATE2(CP_Concat, id_kleene_q,
				FQL_CREATE2(CP_Concat, e, id_kleene_q)));
	fql::Query * q(FQL_CREATE3(Query, 0, c, id_kleene));

	fql::Compute_Test_Goals_From_Instrumentation goals(l, options);
	fql::CNF_Conversion & eq(goals.do_query(gf, *q));
	TEST_CHECK_RELATION(8, ==, eq.get_test_goal_id_map().size());

	statistics::Statistics stats;
	Constraint_Strengthening cs(eq, options);
	Constraint_Strengthening::test_cases_t test_suite, atc;
	options.set_option("sat-coverage-check", true);
	cs.generate(goals, test_suite, 0, stats, atc);
	Constraint_Strengthening::test_cases_t::size_type const size(test_suite.size());
	TEST_CHECK_RELATION(size, >=, 3);
	TEST_CHECK_RELATION(size, <=, 5);
	
	Test_Suite_Minimization ts_min(l);
	ts_min.minimize(test_suite);
	TEST_CHECK_RELATION(test_suite.size(), >=, 3);
	TEST_CHECK_RELATION(test_suite.size(), <=, size);
	
	::config.main = id2string(ID_main);
	::cnf_clause_list_assignmentt & cnf(eq.get_cnf());
	::boolbvt const& bv(eq.get_bv());
	for (::fshell2::Constraint_Strengthening::test_cases_t::const_iterator iter(
				test_suite.begin()); iter != test_suite.end(); ++iter) {
		cnf.copy_assignment_from(iter->second);
		::boolbv_mapt const& map(bv.get_map());
		TEST_CHECK_RELATION(map.mapping.size(), >=, 16);
		for (::boolbv_mapt::mappingt::const_iterator iter2(map.mapping.begin());
				iter2 != map.mapping.end(); ++iter2) {
			::symbol_exprt sym(iter2->first);
			::fshell2::Symbol_Identifier v(sym);
			if (::fshell2::Symbol_Identifier::FSHELL2_INTERNAL == v.m_vt) continue;

			std::string const name(id2string(iter2->first));
			if(std::string::npos != name.find("main::argc") ||
					std::string::npos != name.find("main::argv") ||
					std::string::npos != name.find("::x") ||
					std::string::npos != name.find("::y"))
				TEST_ASSERT_RELATION(v.m_vt, ==, ::fshell2::Symbol_Identifier::PARAMETER);
			else if(std::string::npos != name.find("::ln") ||
					std::string::npos != name.find("::ls"))
				TEST_ASSERT_RELATION(v.m_vt, ==, ::fshell2::Symbol_Identifier::LOCAL);
			else if(std::string::npos != name.find("::stat"))
				TEST_ASSERT_RELATION(v.m_vt, ==, ::fshell2::Symbol_Identifier::LOCAL_STATIC);
			else if(std::string::npos != name.find("shared"))
				TEST_ASSERT_RELATION(v.m_vt, ==, ::fshell2::Symbol_Identifier::GLOBAL);
			else if(std::string::npos != name.find("argc'") ||
					std::string::npos != name.find("argv'") ||
					std::string::npos != name.find("return'") ||
					std::string::npos != name.find("symex::io::") ||
					std::string::npos != name.find("__CPROVER_"))
				TEST_ASSERT_RELATION(v.m_vt, ==, ::fshell2::Symbol_Identifier::CBMC_INTERNAL);
			else
				TEST_CHECK(false);
		}
	}
}

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
	of << "int x;" << ::std::endl
		<< "int main(int argc, char * argv[])" << ::std::endl
		<< "{" << ::std::endl
		<< "if (argc == 2) x=2; else x=1;" << ::std::endl
		<< "if (argc == 3) ++x; else --x;" << ::std::endl
		<< "return x;" << ::std::endl
		<< "}" << ::std::endl;
	of.close();

	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	options.set_option("stop-on-fail", true);
	::goto_functionst gf;

	TEST_CHECK(!l.parse(tempname_str));
	::unlink(tempname_str.c_str());
	TEST_CHECK(!l.typecheck());
	TEST_CHECK(!l.final());
    
	::goto_convert(l.symbol_table, gf, l.ui_message_handler);
	
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
	TEST_CHECK_RELATION(4, ==, eq.get_test_goal_id_map().size());

	statistics::Statistics stats;
	Constraint_Strengthening cs(eq, options);
	Constraint_Strengthening::test_cases_t test_suite, atc;
	options.set_option("sat-coverage-check", true);
	cs.generate(goals, test_suite, 0, stats, atc);
	Constraint_Strengthening::test_cases_t::size_type const size(test_suite.size());
	TEST_CHECK_RELATION(size, >=, 2);
	TEST_CHECK_RELATION(size, <=, 5);
	
	Test_Suite_Minimization ts_min(l);
	ts_min.minimize(test_suite);
	TEST_CHECK_RELATION(test_suite.size(), >, 0);
	TEST_CHECK_RELATION(test_suite.size(), <=, size);
	
	::config.main = id2string(ID_main);
	Test_Suite_Output out(eq, options);
	
	::std::ostringstream os_brief;
	out.print_ts(test_suite, 0, os_brief, ::ui_message_handlert::PLAIN);
	TEST_ASSERT(data.compare("test_results_brief_plain_1", os_brief.str()));
	::std::ostringstream os_xml_brief;
	out.print_ts(test_suite, 0, os_xml_brief, ::ui_message_handlert::XML_UI);
	TEST_ASSERT(data.compare("test_results_brief_xml_1", os_xml_brief.str()));
	
	options.set_option("tco-location", true);
	::std::ostringstream os;
	out.print_ts(test_suite, 0, os, ::ui_message_handlert::PLAIN);
	TEST_ASSERT(data.compare("test_results_plain_1", os.str()));
	::std::ostringstream os_xml;
	out.print_ts(test_suite, 0, os_xml, ::ui_message_handlert::XML_UI);
	TEST_ASSERT(data.compare("test_results_xml_1", os_xml.str()));
	
	options.set_option("tco-called-functions", true);
	options.set_option("tco-assign-globals", true);
	::std::ostringstream os_ext;
	out.print_ts(test_suite, 0, os_ext, ::ui_message_handlert::PLAIN);
	TEST_ASSERT(data.compare("test_results_ext_plain_1", os_ext.str()));
	::std::ostringstream os_xml_ext;
	out.print_ts(test_suite, 0, os_xml_ext, ::ui_message_handlert::XML_UI);
	TEST_ASSERT(data.compare("test_results_ext_xml_1", os_xml_ext.str()));
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test_sym_classes, LEVEL_PROD );
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
