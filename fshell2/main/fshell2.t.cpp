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

/*! \file fshell2/main/fshell2.t.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Apr 15 11:58:54 CEST 2009 
*/


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/util/statistics.hpp>
#include <fshell2/main/fshell2.hpp>

#include <cerrno>
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

#include <fshell2/config/features.hpp>

#ifdef FSHELL2_HAVE_LIBREADLINE
#  include <readline/readline.h>
#else
extern "C" {
#  include <linenoise/linenoise.h>
}
#define rl_instream linenoiseInStream
#endif

#include <util/config.h>
#include <util/cmdline.h>
#include <util/tempfile.h>
#include <langapi/language_ui.h>
#include <langapi/mode.h>
#include <ansi-c/ansi_c_language.h>

#define TEST_COMPONENT_NAME FShell2
#define TEST_COMPONENT_NAMESPACE fshell2

FSHELL2_NAMESPACE_BEGIN;

/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of FShell2
 *
 */
void test_single( Test_Data & data )
{
	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	::goto_functionst cfg;

	::fshell2::FShell2 fshell(options, cfg);
	
	TEST_ASSERT(fshell.process_line(l, "QUIT"));
}

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of FShell2
 *
 */
void test_interactive( Test_Data & data )
{
	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	options.set_option("stop-on-fail", true);
	::goto_functionst cfg;
	::std::ostringstream os;

	::fshell2::FShell2 fshell(options, cfg);
	
	::std::string const tempname_str(::get_temporary_file("tmp.query", ""));
	::std::ofstream of(tempname_str.c_str());
	TEST_CHECK(of.is_open());
	of << "show sourcecode all" << ::std::endl
		<< "#define bla blubb" << ::std::endl
		<< "quit" << ::std::endl;
	of.close();
		
	// open the input file
	FILE * in(fopen(tempname_str.c_str(), "r"));
	TEST_CHECK(0 == errno);
	// change the readline input to the file
	rl_instream = in;

	fshell.interactive(l);

	/*
	// print and check the output
	TEST_TRACE(os.str());
	::std::string data_name(::diagnostics::internal::to_string(
				"interactive_query_out_", sizeof(long) * 8, "bit"));
	TEST_ASSERT(data.compare(data_name, os.str()));
	*/
	
	// change back to stdin
	rl_instream = 0;
	fclose(in);

	::unlink(tempname_str.c_str());
}

::std::string const tag1("Possibly feasible test goals:");
::std::string const tag2("Test goals not fulfilled:");
::std::string const tag3("Test cases removed by minimization:");
::std::string const tag4("Test cases:");

#define QUERY(dataname, querystr) \
	{ \
		::fshell2::statistics::Statistics stats; \
		TEST_ASSERT(!fshell.process_line(l, querystr, stats)); \
		oss.str(""); \
		stats.print(msg); \
		::std::istringstream is(oss.str()); \
		oss.str(""); \
		::std::string line; \
		while (!::std::getline(is, line).eof()) \
			if (0 == line.compare(0, tag1.size(), tag1) || \
					0 == line.compare(0, tag2.size(), tag2) || \
					0 == line.compare(0, tag3.size(), tag3) || \
					0 == line.compare(0, tag4.size(), tag4)) \
				oss << line << ::std::endl; \
		TEST_ASSERT(data.compare(dataname ":" querystr, oss.str())); \
	} DUMMY_FUNC

void do_test_single2( Test_Data & data, bool const use_instrumentation )
{
	::std::string const tempname_str(::get_temporary_file("tmp.src", ".c"));
	::std::ofstream of(tempname_str.c_str());
	TEST_CHECK(of.is_open());
of 
  << "int main(int argc, char* argv[]) {" << ::std::endl
  << "  int x=0;" << ::std::endl
  << "L0a:  if (argc>2) {" << ::std::endl
  << "    if (argc<27)" << ::std::endl
  << "L1:   --x;" << ::std::endl
  << "    else" << ::std::endl
  << "	    ++x;" << ::std::endl
  << "	}" << ::std::endl
  << "L2:" << ::std::endl
  << "  x--;" << ::std::endl
  << "  x--;" << ::std::endl
  << "  x--;" << ::std::endl
  << "  x--;" << ::std::endl
  << "  return 0;" << ::std::endl
  << "}" << ::std::endl;
	of.close();

	::register_language(new_ansi_c_language);
	::cmdlinet cmdline;
	::config.set(cmdline);
	::ui_message_handlert ui_message_handler(cmdline, "FShell2");
	::language_uit l(cmdline, ui_message_handler);
	::optionst options;
	options.set_option("stop-on-fail", true);
	options.set_option("show-test-goals", true);
	options.set_option("statistics", true);
	options.set_option("use-instrumentation", use_instrumentation);
	options.set_option("sat-coverage-check", use_instrumentation);
	options.set_option("internal-coverage-check", !use_instrumentation);
	::goto_functionst gf;

	::fshell2::FShell2 fshell(options, gf);
	
	TEST_ASSERT(!fshell.process_line(l, ::diagnostics::internal::to_string("add sourcecode '", tempname_str, "'").c_str()));
	::unlink(tempname_str.c_str());
	
	::std::ostringstream oss;
	::stream_message_handlert mh(oss);
	::messaget msg(mh);

	QUERY("query1", "cover edges(@basicblockentry)");
	QUERY("query2", "cover edges(@basicblockentry) passing @func(main)*.@label(L1)->@label(L2).@func(main)*");
	QUERY("query3", "cover edges(@basicblockentry) passing @func(main)*.@label(L2)->@label(L1).@func(main)*");
	QUERY("query4", "cover edges(@conditionedge)->edges(@conditionedge)");
	QUERY("query5", "cover edges(@basicblockentry). \"@func(main)*\" .edges(@basicblockentry)");
	QUERY("query6", "cover paths(id,1)");
	QUERY("query7", "cover @label(L2).{x>=0}");
	QUERY("query8", "cover @label(L2).{x>0}");
	QUERY("query9", "cover {x>0}.@label(L2)");
	QUERY("query10", "cover nodes(id)");
	QUERY("query11", "cover \"ID*\"");
	QUERY("query12", "cover ^ID$");
	QUERY("query13", "cover \"ID*.@5.ID*\" + \"ID*.@7.ID*\"");
	QUERY("query14", "cover ({x==0}+{x!=0}).({argc<10}+{argc>=10}).ID.({x==0}+{x!=0}).({argc<10}+{argc>=10})");
	QUERY("query15", "cover @label(L0a) -> @label(L1) + @label(L1) -> @label(L2)");
	
	TEST_ASSERT(fshell.process_line(l, "QUIT"));
}	


////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of FShell2
 *
 */
void test_single2( Test_Data & data )
{
	do_test_single2(data, false);
}	

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of FShell2
 *
 */
void test_single2_instrumentation( Test_Data & data )
{
	do_test_single2(data, true);
}	

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

FSHELL2_NAMESPACE_END;

TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test_single, LEVEL_PROD );
#if !defined(__MINGW32__) && !defined(__MACH__)
// resetting rl_instream apparently doesn't work
TEST_NORMAL_CASE( &test_interactive, LEVEL_PROD );
#endif
TEST_NORMAL_CASE( &test_single2, LEVEL_PROD );
TEST_NORMAL_CASE( &test_single2_instrumentation, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
