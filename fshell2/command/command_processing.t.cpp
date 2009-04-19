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
#include <cbmc/src/util/config.h>

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
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l(cmdline);
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;

	TEST_ASSERT(Command_Processing::NO_CONTROL_COMMAND == Command_Processing::get_instance().process(l, os, "quit HELP"));
	TEST_ASSERT(Command_Processing::DONE == Command_Processing::get_instance().process(l, os, "   // comment"));
	
	TEST_ASSERT(Command_Processing::QUIT == Command_Processing::get_instance().process(l, os, "quit"));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Command_Processing
 *
 */
void test_help( Test_Data & data )
{
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l(cmdline);
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;
	
	TEST_ASSERT(Command_Processing::HELP == Command_Processing::get_instance().process(l, os, "help"));
	TEST_ASSERT(Command_Processing::HELP == Command_Processing::get_instance().process(l, os, "  HELP  "));
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
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l(cmdline);
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;

	TEST_THROWING_BLOCK_ENTER;
	Command_Processing::get_instance().process(l, os, "add sourcecode \"no_such_file.c\"");
	TEST_THROWING_BLOCK_EXIT(::fshell2::Command_Processing_Error);
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of Command_Processing
 *
 */
void test_use_case( Test_Data & data )
{
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l(cmdline);
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;

	char * tempname(::strdup("/tmp/srcXXXXXX.c"));
	TEST_CHECK(-1 != ::mkstemps(tempname, 2));
	::std::ofstream of(tempname);
	TEST_CHECK(of.is_open());
	of << "int main(int argc, char * argv[])" << ::std::endl
		<< "{" << ::std::endl
		<< "return 0;" << ::std::endl
		<< "}" << ::std::endl;
	of.close();

	::std::ostringstream cmd_str;
	cmd_str << "add sourcecode \"" << tempname << "\"";

	TEST_ASSERT(Command_Processing::DONE == Command_Processing::get_instance().process(l, os, cmd_str.str().c_str()));
	
	TEST_CHECK(!l.typecheck());
	TEST_CHECK(!l.final());
	
	TEST_ASSERT(Command_Processing::DONE == Command_Processing::get_instance().process(l, os, "show filenames"));
	TEST_ASSERT(!os.str().empty());
	
	os.str("");
	TEST_ASSERT(Command_Processing::DONE == Command_Processing::get_instance().process(l, os, "show sourcecode all"));
	TEST_ASSERT(data.compare("tmp_source_show_all", os.str()));
	
	cmd_str.str("");
	cmd_str << "show sourcecode \"" << tempname << "\"";
	os.str("");
	TEST_ASSERT(Command_Processing::DONE == Command_Processing::get_instance().process(l, os, cmd_str.str().c_str()));
	TEST_ASSERT(data.compare("tmp_source_show", os.str()));
	
	TEST_ASSERT(Command_Processing::DONE == Command_Processing::get_instance().process(l, os, "set entry main"));
	TEST_ASSERT(::config.main == "main");

	TEST_ASSERT(0 == ::config.fshell.max_test_cases);
	TEST_ASSERT(Command_Processing::DONE == Command_Processing::get_instance().process(l, os, "set limit count 27"));
	TEST_ASSERT(27 == ::config.fshell.max_test_cases);

	::unlink(tempname);
	free(tempname);
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
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
