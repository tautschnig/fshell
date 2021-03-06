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
#include <langapi/language_ui.h>
#include <goto-programs/goto_convert_functions.h>
#include <util/std_code.h>
#include <util/std_expr.h>
#include <util/arith_tools.h>
#include <util/expr_util.h>
#include <langapi/mode.h>
#include <ansi-c/ansi_c_language.h>
#include <cbmc/bmc.h>
#include <cbmc/bv_cbmc.h>

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
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::goto_functionst gf;
	GOTO_Transformation t(l, gf);
	::goto_programt prg;

	TEST_THROWING_BLOCK_ENTER;
	t.insert(ID__start, GOTO_Transformation::BEFORE, ::END_FUNCTION, prg);
	TEST_THROWING_BLOCK_EXIT(::fshell2::Instrumentation_Error);
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of GOTO_Transformation
 *
 */
void test_use_case( Test_Data & data )
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
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::goto_functionst cfg;

	TEST_CHECK(!l.parse(tempname_str));
	::unlink(tempname_str.c_str());
	TEST_CHECK(!l.typecheck());
	TEST_CHECK(!l.final());
    
	::goto_convert(l.symbol_table, cfg, l.ui_message_handler);
	
	::goto_programt tmp;
	::goto_programt::targett as(tmp.add_instruction(ASSERT));
	as->code = ::code_assertt();
	as->guard = ::gen_zero(::bool_typet());

	::fshell2::instrumentation::GOTO_Transformation inserter(l, cfg);
	TEST_ASSERT_RELATION(1, ==, inserter.insert(ID__start, ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::END_FUNCTION, tmp).size());
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
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
  
	::languaget *languagep=get_language_from_filename("bla.c");
	TEST_CHECK(languagep);
	language_filet language_file;

  	std::pair<language_filest::filemapt::iterator, bool> res=l.language_files.filemap.insert(
           ::std::make_pair("bla.c", language_file));

  	language_filet &lf=res.first->second;
  	lf.filename="bla.c";
  	lf.language=languagep;

	::goto_functionst cfg;
	::fshell2::instrumentation::GOTO_Transformation inserter(l, cfg);
    ::goto_convert_functionst converter(l.symbol_table, cfg, l.ui_message_handler);
	
	{
		// add dummy symbol to make CBMC happy
		cfg.function_map["__CPROVER_initialize"].type.return_type() = ::empty_typet(); 
		::symbol_exprt func_expr("__CPROVER_initialize", ::code_typet());
		::symbolt func_symb;
		func_symb.from_irep(func_expr);
		func_symb.value = ::code_blockt();
		func_symb.mode = ID_C;
		func_symb.name = "__CPROVER_initialize";
		func_symb.base_name = "__CPROVER_initialize";
		l.symbol_table.move(func_symb);
		::symbol_tablet::symbolst::iterator init_iter(l.symbol_table.symbols.find("__CPROVER_initialize"));
		TEST_CHECK(init_iter != l.symbol_table.symbols.end());
		converter.convert_function(init_iter->first);
	}
	
	{
		// add dummy symbol to make CBMC happy
		::symbol_exprt rm_expr("__CPROVER_rounding_mode", ::signedbv_typet(sizeof(int)*8));
		::symbolt rm_symb;
		rm_symb.from_irep(rm_expr);
		rm_symb.mode = ID_C;
		rm_symb.name = "__CPROVER_rounding_mode";
		rm_symb.base_name = "__CPROVER_rounding_mode";
		l.symbol_table.move(rm_symb);
	}
	
	cfg.function_map["tmp_func"].type.return_type() = ::empty_typet(); 
	::config.main = "tmp_func";
	::symbol_exprt func_expr("tmp_func", ::code_typet());
	::symbolt func_symb;
	func_symb.from_irep(func_expr);
	func_symb.value = ::code_blockt();
	func_symb.mode = ID_C;
	func_symb.name = "tmp_func";
	func_symb.base_name = "tmp_func";
	l.symbol_table.move(func_symb);
	TEST_CHECK(!l.final());
	::symbol_tablet::symbolst::iterator main_iter(l.symbol_table.symbols.find(ID__start));
	TEST_CHECK(main_iter != l.symbol_table.symbols.end());
    converter.convert_function(main_iter->first);
		
	::fshell2::instrumentation::GOTO_Transformation::inserted_t & targets(
			inserter.make_nondet_choice(cfg.function_map["tmp_func"].body, 3));
	TEST_ASSERT_RELATION(3, ==, targets.size());
	TEST_ASSERT_RELATION(12, ==, cfg.function_map["tmp_func"].body.instructions.size());
	cfg.function_map["tmp_func"].body.add_instruction(END_FUNCTION);

	{
		::false_exprt f;
		targets.front().second->make_assertion(f);
		(++(targets.begin()))->second->make_skip();
		(++(++(targets.begin())))->second->make_skip();
	
		const namespacet ns(l.symbol_table);
		satcheckt satcheck;
		bv_cbmct bv_cbmc(ns, satcheck);
		::bmct bmc(options, l.symbol_table, l.ui_message_handler, bv_cbmc);
		TEST_ASSERT(bmc.run(cfg));
	}

	{
		::false_exprt f;
		targets.front().second->make_skip();
		(++(targets.begin()))->second->make_assertion(f);
		(++(++(targets.begin())))->second->make_skip();

		const namespacet ns(l.symbol_table);
		satcheckt satcheck;
		bv_cbmct bv_cbmc(ns, satcheck);
		::bmct bmc(options, l.symbol_table, l.ui_message_handler, bv_cbmc);
		TEST_ASSERT(bmc.run(cfg));
	}

	{
		::false_exprt f;
		targets.front().second->make_skip();
		(++(targets.begin()))->second->make_skip();
		(++(++(targets.begin())))->second->make_assertion(f);

		const namespacet ns(l.symbol_table);
		satcheckt satcheck;
		bv_cbmct bv_cbmc(ns, satcheck);
		::bmct bmc(options, l.symbol_table, l.ui_message_handler, bv_cbmc);
		TEST_ASSERT(bmc.run(cfg));
	}
		
	cfg.function_map["tmp_func"].body.instructions.front().function = "tmp_func";
	::false_exprt f;
	::fshell2::instrumentation::GOTO_Transformation::goto_node_t ndchoice3(*(++(++(targets.begin()))));

	{
		// invalidates targets
		inserter.insert_predicate_at(ndchoice3, &f);
		::goto_programt::targett loc(ndchoice3.second);
		--(--loc);
		TEST_ASSERT(loc->type == LOCATION);
		TEST_ASSERT_RELATION(16, ==, cfg.function_map["tmp_func"].body.instructions.size());
		ndchoice3.second->make_skip();
		loc->make_assertion(f);

		const namespacet ns(l.symbol_table);
		satcheckt satcheck;
		bv_cbmct bv_cbmc(ns, satcheck);
		::bmct bmc(options, l.symbol_table, l.ui_message_handler, bv_cbmc);
		TEST_ASSERT(!bmc.run(cfg));
	}

	{
		::symbol_exprt x("x");
		x.set("base_name", "x");
		::binary_relation_exprt inv(x, "<", ::from_integer(1, ::integer_typet()));
		// invalidates targets
		inserter.insert_predicate_at(ndchoice3, &inv);
		::goto_programt::targett loc(ndchoice3.second);
		--(--loc);
		TEST_ASSERT(loc->type == LOCATION);
		/*::namespacet const ns(l.symbol_table);
		cfg.output(ns, ::std::cerr);*/
		TEST_ASSERT_RELATION(20, ==, cfg.function_map["tmp_func"].body.instructions.size());
		::false_exprt f;
		loc->make_assertion(f);

		const namespacet ns(l.symbol_table);
		satcheckt satcheck;
		bv_cbmct bv_cbmc(ns, satcheck);
		::bmct bmc(options, l.symbol_table, l.ui_message_handler, bv_cbmc);
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
