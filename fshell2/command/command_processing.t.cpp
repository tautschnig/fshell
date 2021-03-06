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

/*! \file fshell2/command/command_processing.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:39 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

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
#include <langapi/mode.h>
#include <ansi-c/ansi_c_language.h>

#include <fshell2/command/command_processing.hpp>
#include <fshell2/exception/command_processing_error.hpp>

#define TEST_COMPONENT_NAME Command_Processing
#define TEST_COMPONENT_NAMESPACE fshell2::command

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_COMMAND_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Command_Processing
 *
 */
void test_basic( Test_Data & data )
{
	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	::goto_functionst cfg;
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;

	Command_Processing cmd(options, cfg);

	TEST_ASSERT(Command_Processing::NO_CONTROL_COMMAND == cmd.process(l, os, "quit HELP"));
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "   // comment"));
	
	TEST_ASSERT(Command_Processing::QUIT == cmd.process(l, os, "quit"));
	TEST_ASSERT(Command_Processing::QUIT == cmd.process(l, os, "exit"));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Command_Processing
 *
 */
void test_help( Test_Data & data )
{
	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	::goto_functionst cfg;
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;

	Command_Processing cmd(options, cfg);
	
	TEST_ASSERT(Command_Processing::HELP == cmd.process(l, os, "help"));
	TEST_ASSERT(Command_Processing::HELP == cmd.process(l, os, "  HELP  "));
	Command_Processing::help(os);
	TEST_ASSERT(data.compare("command_help", os.str()));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Command_Processing
 *
 */
void test_invalid( Test_Data & data )
{
	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	::goto_functionst cfg;
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;

	Command_Processing cmd(options, cfg);

	TEST_THROWING_BLOCK_ENTER;
	cmd.process(l, os, "add sourcecode 'no_such_file.c'");
	TEST_THROWING_BLOCK_EXIT1(::fshell2::Command_Processing_Error, "Failed to parse no_such_file.c");
	
	TEST_THROWING_BLOCK_ENTER;
	cmd.finalize(l);
	TEST_THROWING_BLOCK_EXIT1(::fshell2::Command_Processing_Error, "No source files loaded!");
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Command_Processing
 *
 */
void test_use_case( Test_Data & data )
{
	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	::goto_functionst cfg;
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;

	Command_Processing cmd(options, cfg);

	::std::string const tempname_str(::get_temporary_file("tmp.src", ".c"));
	::std::ofstream of(tempname_str.c_str());
	TEST_CHECK(of.is_open());
	of << "int main(int argc, char * argv[])" << ::std::endl
		<< "{" << ::std::endl
		<< "return 0;" << ::std::endl
		<< "}" << ::std::endl;
	of.close();

	::std::ostringstream cmd_str;
	cmd_str << "add sourcecode '" << tempname_str << "'";

	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, cmd_str.str().c_str()));
	
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "show filenames"));
	TEST_ASSERT(!os.str().empty());
	
	os.str("");
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "show sourcecode all"));
	TEST_ASSERT(data.compare("tmp_source_show_all", os.str()));
	
	cmd_str.str("");
	cmd_str << "show sourcecode '" << tempname_str << "'";
	os.str("");
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, cmd_str.str().c_str()));
	TEST_ASSERT(data.compare("tmp_source_show", os.str()));
	
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "set entry main"));
	TEST_ASSERT(::config.main == id2string(ID_main));

	TEST_ASSERT(0 == options.get_unsigned_int_option(F2_O_LIMIT));
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "set limit count 27"));
	TEST_ASSERT(27 == options.get_unsigned_int_option(F2_O_LIMIT));

	TEST_ASSERT(1 == options.get_unsigned_int_option(F2_O_MULTIPLE_COVERAGE));
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "set multiple_coverage 42"));
	TEST_ASSERT(42 == options.get_unsigned_int_option(F2_O_MULTIPLE_COVERAGE));

	::unlink(tempname_str.c_str());
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Command_Processing
 *
 */
void test_use_case_extended_invariants( Test_Data & data )
{
	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	::goto_functionst cfg;
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;

	Command_Processing cmd(options, cfg);

	{
		::std::string const tempname_str(::get_temporary_file("tmp.src", ".c"));
		::std::ofstream of(tempname_str.c_str());
		TEST_CHECK(of.is_open());
		of << "int foo();" << ::std::endl
			<< "int main(int argc, char * argv[])" << ::std::endl
			<< "{" << ::std::endl
			<< "return foo();" << ::std::endl
			<< "}" << ::std::endl;
		of.close();

		::std::ostringstream cmd_str;
		cmd_str << "add sourcecode '" << tempname_str << "'";
		TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, cmd_str.str().c_str()));
		::unlink(tempname_str.c_str());
	}
	
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "set entry main"));
	TEST_ASSERT(::config.main == id2string(ID_main));
	// do it once again
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "set entry main"));

	TEST_ASSERT(0 == options.get_unsigned_int_option(F2_O_LIMIT));
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "set limit count 27"));
	TEST_ASSERT(27 == options.get_unsigned_int_option(F2_O_LIMIT));

	TEST_CHECK(cfg.function_map.end() == cfg.function_map.find("foo"));
	TEST_ASSERT(cmd.finalize(l));
	TEST_CHECK(cfg.function_map.end() != cfg.function_map.find("foo"));
	TEST_CHECK(!cfg.function_map.find("foo")->second.body_available());
	
	{
		// just to make sure we never ever get the same filename
		::std::string const tempname_str(::get_temporary_file("tmp.src", "2.c"));
		::std::ofstream of(tempname_str.c_str());
		TEST_CHECK(of.is_open());
		of << "int foo()" << ::std::endl
			<< "{" << ::std::endl
			<< "return 42;" << ::std::endl
			<< "}" << ::std::endl;
		of.close();

		::std::ostringstream cmd_str;
		cmd_str << "add sourcecode '" << tempname_str << "'";
		TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, cmd_str.str().c_str()));
		::unlink(tempname_str.c_str());
	}

	TEST_ASSERT(cmd.finalize(l));
	TEST_CHECK(cfg.function_map.end() != cfg.function_map.find("foo"));
	TEST_CHECK(cfg.function_map.find("foo")->second.body_available());
	// should be a no-op
	TEST_ASSERT(!cmd.finalize(l));
	
	TEST_ASSERT(Command_Processing::DONE == cmd.process(l, os, "set abstract foo"));
	TEST_ASSERT(cmd.finalize(l));
	TEST_CHECK(cfg.function_map.end() != cfg.function_map.find("foo"));
	TEST_ASSERT(!cfg.function_map.find("foo")->second.body_available());
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_COMMAND_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test_basic, LEVEL_PROD );
TEST_NORMAL_CASE( &test_help, LEVEL_PROD );
TEST_NORMAL_CASE( &test_invalid, LEVEL_PROD );
TEST_NORMAL_CASE( &test_use_case, LEVEL_PROD );
TEST_NORMAL_CASE( &test_use_case_extended_invariants, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
