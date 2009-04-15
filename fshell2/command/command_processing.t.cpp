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
void test( Test_Data & data )
{
	::cmdlinet cmdline;
	::config.set(cmdline);
	::language_uit l(cmdline);
	::std::ostringstream os;

	using ::fshell2::command::Command_Processing;

	TEST_ASSERT(Command_Processing::NO_CONTROL_COMMAND == Command_Processing::get_instance().process(l, os, "quit HELP"));
	TEST_ASSERT(Command_Processing::BLANK == Command_Processing::get_instance().process(l, os, "   // comment"));
	
	TEST_ASSERT(Command_Processing::CMD_HELP == Command_Processing::get_instance().process(l, os, "help"));
	TEST_ASSERT(Command_Processing::CMD_HELP == Command_Processing::get_instance().process(l, os, "  HELP  "));
	Command_Processing::help(os);
	TEST_ASSERT(data.compare("command_help", os.str()));
	os.str("");
	
	TEST_ASSERT(Command_Processing::CMD_QUIT == Command_Processing::get_instance().process(l, os, "quit"));

	TEST_THROWING_BLOCK_ENTER;
	Command_Processing::get_instance().process(l, os, "add sourcecode \"no_such_file.c\"");
	TEST_THROWING_BLOCK_EXIT(::fshell2::Command_Processing_Error);

	char * tempname(::strdup("/tmp/srcXXXXXX.c"));
	TEST_ASSERT(-1 != ::mkstemps(tempname, 2));
	::std::ofstream of(tempname);
	of << "int main(int argc, char * argv[])" << ::std::endl
		<< "{" << ::std::endl
		<< "return 0;" << ::std::endl
		<< "}" << ::std::endl;
	char * cmd = static_cast<char*>(::malloc(::strlen("add sourcecode \"") + ::strlen(tempname) + 3)); // extra space for show ...
	::strcpy(cmd, "add sourcecode \"");
	::strcat(cmd, tempname);
	::strcat(cmd, "\"");

	TEST_ASSERT(Command_Processing::CMD_PROCESSED == Command_Processing::get_instance().process(l, os, cmd));
	
	TEST_ASSERT(!l.typecheck());
	TEST_ASSERT(!l.final());
	
	TEST_ASSERT(Command_Processing::CMD_PROCESSED == Command_Processing::get_instance().process(l, os, "show filenames"));
	TEST_ASSERT(!os.str().empty());
	
	os.str("");
	TEST_ASSERT(Command_Processing::CMD_PROCESSED == Command_Processing::get_instance().process(l, os, "show sourcecode all"));
	TEST_ASSERT(data.compare("tmp_source_show_all", os.str()));
	
	::strcpy(cmd, "show sourcecode \"");
	::strcat(cmd, tempname);
	::strcat(cmd, "\"");
	os.str("");
	TEST_ASSERT(Command_Processing::CMD_PROCESSED == Command_Processing::get_instance().process(l, os, cmd));
	TEST_ASSERT(data.compare("tmp_source_show", os.str()));
	
	TEST_ASSERT(Command_Processing::CMD_PROCESSED == Command_Processing::get_instance().process(l, os, "start main"));

	::unlink(tempname);
	free(tempname);
	free(cmd);
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_COMMAND_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
