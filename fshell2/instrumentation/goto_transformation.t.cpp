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

/*! \file fshell2/instrumentation/goto_transformation.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr 30 13:50:52 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/exception/instrumentation_error.hpp>

#include <fstream>

#include <cbmc/src/util/config.h>
#include <cbmc/src/langapi/language_ui.h>
#include <cbmc/src/goto-programs/goto_convert_functions.h>
#include <cbmc/src/util/std_code.h>
#include <cbmc/src/util/std_expr.h>
#include <cbmc/src/util/arith_tools.h>
#include <cbmc/src/langapi/mode.h>
#include <cbmc/src/ansi-c/ansi_c_language.h>
#include <cbmc/src/cbmc/bmc.h>

#define TEST_COMPONENT_NAME GOTO_Transformation
#define TEST_COMPONENT_NAMESPACE fshell2::instrumentation

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of GOTO_Transformation
 *
 */
void test_invalid( Test_Data & data )
{
	::goto_functionst gf;
	GOTO_Transformation t(gf);
	::goto_programt prg;

	TEST_THROWING_BLOCK_ENTER;
	t.insert("main", GOTO_Transformation::BEFORE, ::END_FUNCTION, prg);
	TEST_THROWING_BLOCK_EXIT(::fshell2::Instrumentation_Error);
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of GOTO_Transformation
 *
 */
void test_use_case( Test_Data & data )
{
	char * tempname(::strdup("/tmp/srcXXXXXX"));
	TEST_CHECK(-1 != ::mkstemp(tempname));
	::std::string tempname_str(tempname);
	tempname_str += ".c";
	::std::ofstream of(tempname_str.c_str());
	TEST_CHECK(of.is_open());
	of << "int main(int argc, char * argv[])" << ::std::endl
		<< "{" << ::std::endl
		<< "if (argc > 2) return 2;" << ::std::endl
		<< "return 0;" << ::std::endl
		<< "}" << ::std::endl;
	of.close();
	::unlink(tempname);
	::free(tempname);

	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l("FShell2", cmdline);
	::optionst options;
	::goto_functionst cfg;

	TEST_CHECK(!l.parse(tempname_str));
	::unlink(tempname_str.c_str());
	TEST_CHECK(!l.typecheck());
	TEST_CHECK(!l.final());
    
	::goto_convert(l.context, options, cfg, l.ui_message_handler);
	
	::goto_programt tmp;
	::goto_programt::targett as(tmp.add_instruction(ASSERT));
	as->code = ::code_assertt();
	::exprt zero(::exprt("constant", ::typet("bool")));
	zero.set("value", "false");
	as->guard = zero;

	::fshell2::instrumentation::GOTO_Transformation inserter(cfg);
	TEST_ASSERT_RELATION(1, ==, inserter.insert("main", ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::END_FUNCTION, tmp).size());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of GOTO_Transformation
 *
 */
void test_use_case2( Test_Data & data )
{
	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l("FShell2", cmdline);
	::optionst options;
  
	::languaget *languagep=get_language_from_filename("bla.c");
	TEST_CHECK(languagep);
	language_filet language_file;

  	std::pair<language_filest::filemapt::iterator, bool> res=l.language_files.filemap.insert(
           ::std::make_pair<std::string, language_filet>("bla.c", language_file));

  	language_filet &lf=res.first->second;
  	lf.filename="bla.c";
  	lf.language=languagep;

	::goto_functionst cfg;
	::fshell2::instrumentation::GOTO_Transformation inserter(cfg);
	
	cfg.function_map["c::tmp_func"].body_available = true;
	cfg.function_map["c::tmp_func"].type.return_type() = ::empty_typet(); 
	::config.main = "tmp_func";
	::symbol_exprt func_expr("c::tmp_func", ::typet("code"));
	::symbolt func_symb;
	func_symb.from_irep(func_expr);
	func_symb.value = ::code_blockt();
	func_symb.mode = "C";
	func_symb.name = "c::tmp_func";
	func_symb.base_name = "tmp_func";
	l.context.move(func_symb);
	TEST_CHECK(!l.final());
	::symbolst::iterator main_iter(l.context.symbols.find("main"));
	TEST_CHECK(main_iter != l.context.symbols.end());
    ::goto_convert_functionst converter(l.context, options, cfg, l.ui_message_handler);
    converter.convert_function(main_iter->first);
		
	::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
			inserter.make_nondet_choice(cfg.function_map["c::tmp_func"].body, 3, l.context));
	TEST_ASSERT_RELATION(3, ==, targets.size());
	TEST_ASSERT_RELATION(12, ==, cfg.function_map["c::tmp_func"].body.instructions.size());
	cfg.function_map["c::tmp_func"].body.add_instruction(END_FUNCTION);

	{
		::false_exprt f;
		targets.front().second->make_assertion(f);
		(++(targets.begin()))->second->make_skip();
		(++(++(targets.begin())))->second->make_skip();
	
		::bmct bmc(l.context, l.ui_message_handler);
		bmc.options = options;
		bmc.set_verbosity(l.get_verbosity());
		TEST_ASSERT(bmc.run(cfg));
	}

	{
		::false_exprt f;
		targets.front().second->make_skip();
		(++(targets.begin()))->second->make_assertion(f);
		(++(++(targets.begin())))->second->make_skip();

		::bmct bmc(l.context, l.ui_message_handler);
		bmc.options = options;
		bmc.set_verbosity(l.get_verbosity());
		TEST_ASSERT(bmc.run(cfg));
	}

	{
		::false_exprt f;
		targets.front().second->make_skip();
		(++(targets.begin()))->second->make_skip();
		(++(++(targets.begin())))->second->make_assertion(f);

		::bmct bmc(l.context, l.ui_message_handler);
		bmc.options = options;
		bmc.set_verbosity(l.get_verbosity());
		TEST_ASSERT(bmc.run(cfg));
	}
		
	cfg.function_map["c::tmp_func"].body.instructions.front().function = "c::tmp_func";
	::false_exprt f;
	::fshell2::instrumentation::GOTO_Transformation::goto_node_t ndchoice3(*(++(++(targets.begin()))));

	{
		// invalidates targets
		inserter.insert_predicate_at(ndchoice3, &f, l.context);
		::goto_programt::targett loc(ndchoice3.second);
		--(--loc);
		TEST_ASSERT(loc->type == LOCATION);
		TEST_ASSERT_RELATION(16, ==, cfg.function_map["c::tmp_func"].body.instructions.size());
		ndchoice3.second->make_skip();
		loc->make_assertion(f);

		::bmct bmc(l.context, l.ui_message_handler);
		bmc.options = options;
		bmc.set_verbosity(l.get_verbosity());
		TEST_ASSERT(!bmc.run(cfg));
	}

	{
		::symbol_exprt x("x");
		x.set("base_name", "x");
		::binary_relation_exprt inv(x, "<", ::from_integer(1, ::typet("integer")));
		// invalidates targets
		inserter.insert_predicate_at(ndchoice3, &inv, l.context);
		::goto_programt::targett loc(ndchoice3.second);
		--(--loc);
		TEST_ASSERT(loc->type == LOCATION);
		/*::namespacet const ns(l.context);
		cfg.output(ns, ::std::cerr);*/
		TEST_ASSERT_RELATION(20, ==, cfg.function_map["c::tmp_func"].body.instructions.size());
		::false_exprt f;
		loc->make_assertion(f);

		::bmct bmc(l.context, l.ui_message_handler);
		bmc.options = options;
		bmc.set_verbosity(l.get_verbosity());
		TEST_ASSERT(bmc.run(cfg));
	}
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test_invalid, LEVEL_DEBUG );
TEST_NORMAL_CASE( &test_use_case, LEVEL_PROD );
TEST_NORMAL_CASE( &test_use_case2, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
