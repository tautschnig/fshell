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

/*! \file fshell2/main/parseoptions.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Aug  9 12:39:08 CEST 2010
*/

#include <fshell2/main/parseoptions.hpp>
#include <fshell2/config/features.hpp>

#include <fshell2/main/fshell2.hpp>
#include <fshell2/command/command_processing.hpp>

#include <fstream>
#include <cstdlib>

#include <cbmc/src/util/config.h>
#include <cbmc/src/goto-programs/goto_convert_functions.h>
#include <cbmc/src/goto-programs/read_goto_binary.h>
#include <cbmc/src/ansi-c/ansi_c_language.h>
#include <cbmc/src/cbmc/version.h>
#include <cbmc/src/langapi/mode.h>

FSHELL2_NAMESPACE_BEGIN;

Parseoptions::Parseoptions(int argc, const char **argv) :
	::parseoptions_baset(FSHELL2_OPTIONS, argc, argv),
	::language_uit("FShell 2 " FSHELL2_VERSION, cmdline),
	m_stats(),
	m_cpu_total(0),
	m_session_total(0)
{
	NEW_STAT(m_stats, CPU_Timer, cpu_timer, "Total CPU time");
	cpu_timer.start_timer();
	m_cpu_total = &cpu_timer;
	NEW_STAT(m_stats, Wallclock_Timer, wall_timer, "Session lasted for");
	wall_timer.start_timer();
	m_session_total = &wall_timer;
}

Parseoptions::Parseoptions(int argc, const char **argv,
		::std::string const& extra_options) :
	::parseoptions_baset(FSHELL2_OPTIONS + extra_options, argc, argv),
	::language_uit("FShell 2 " FSHELL2_VERSION, cmdline),
	m_stats(),
	m_cpu_total(0),
	m_session_total(0)
{
	NEW_STAT(m_stats, CPU_Timer, cpu_timer, "Total CPU time");
	cpu_timer.start_timer();
	m_cpu_total = &cpu_timer;
	NEW_STAT(m_stats, Wallclock_Timer, wall_timer, "Session lasted for");
	wall_timer.start_timer();
	m_session_total = &wall_timer;
}

Parseoptions::~Parseoptions() {
#if defined(__linux__) || defined(__APPLE__)
	NEW_STAT(m_stats, Peak_Memory_Usage, mem_usage, "Peak memory usage");
	mem_usage.current();
#endif
	m_session_total->stop_timer();
	m_cpu_total->stop_timer();
	if (m_options.get_bool_option("statistics"))
		m_stats.print(*this);
}

void Parseoptions::set_verbosity(::messaget &message)
{
	int v(8);

	if(cmdline.isset("verbosity"))
	{
		v=::atoi(cmdline.getval("verbosity"));
		if(v<0)
			v=0;
		else if(v>9)
			v=9;
	}

	message.set_verbosity(v);
}

bool Parseoptions::get_command_line_options()
{
	if(::config.set(cmdline))
	{
		usage_error();
		return true;
	}

	m_options.set_option("simplify", !cmdline.isset("no-simplify"));
	m_options.set_option("all-claims", false);

	if(cmdline.isset("unwind"))
		m_options.set_option("unwind", cmdline.getval("unwind"));

	if(cmdline.isset("depth"))
		m_options.set_option("depth", cmdline.getval("depth"));

	if(cmdline.isset("slice-by-trace"))
		m_options.set_option("slice-by-trace", cmdline.getval("slice-by-trace"));

	if(cmdline.isset("unwindset"))
		m_options.set_option("unwindset", cmdline.getval("unwindset"));

	// constant propagation
	m_options.set_option("propagation", true);

	m_options.set_option("bounds-check", false);
	m_options.set_option("div-by-zero-check", false);
	m_options.set_option("signed-overflow-check", false);
	m_options.set_option("unsigned-overflow-check", false);
	m_options.set_option("nan-check", false);
	m_options.set_option("pointer-check", false);
	m_options.set_option("assertions", cmdline.isset("enable-assert"));

	// use assumptions
	m_options.set_option("assumptions", !cmdline.isset("no-assumptions"));

	// generate unwinding assertions
	m_options.set_option("unwinding-assertions",
			!cmdline.isset("no-unwinding-assertions"));

	// generate unwinding assumptions otherwise
	m_options.set_option("partial-loops", cmdline.isset("partial-loops"));

	// remove unused equations
	m_options.set_option("slice-formula", cmdline.isset("slice-formula"));

	m_options.set_option("simplify-if", true);
	m_options.set_option("arrays-uf", "auto");
	m_options.set_option("pretty-names", true);

	if(cmdline.isset("outfile"))
		m_options.set_option("outfile", cmdline.getval("outfile"));

	m_options.set_option("show-goto-functions", cmdline.isset("show-goto-functions"));
	m_options.set_option("show-test-goals", cmdline.isset("show-test-goals"));
	m_options.set_option("show-loops", cmdline.isset("show-loops"));
	m_options.set_option("use-instrumentation", cmdline.isset("use-instrumentation"));
	m_options.set_option("internal-coverage-check",
			!cmdline.isset("no-internal-coverage-check"));
	m_options.set_option("sat-coverage-check", cmdline.isset("sat-coverage-check") ||
			m_options.get_bool_option("use-instrumentation") ||
			!m_options.get_bool_option("internal-coverage-check"));
	m_options.set_option("tco-location", cmdline.isset("tco-location"));
	m_options.set_option("tco-called-functions", cmdline.isset("tco-called-functions"));
	m_options.set_option("tco-assign-globals", cmdline.isset("tco-assign-globals"));

	set_verbosity(*this);

	if(cmdline.isset("statistics") || get_verbosity() >= 8)
		m_options.set_option("statistics", true);

	return false;
}

int Parseoptions::doit()
{
	if(cmdline.isset("version"))
	{
		std::cout << FSHELL2_VERSION << std::endl;
		return 0;
	}
	if (get_command_line_options()) return 1;

	// we only support ANSI C at the moment
	::register_language(::new_ansi_c_language);

	try
	{
		goto_functionst goto_functions;
		
		if(cmdline.args.size()==1 &&
				is_goto_binary(cmdline.args[0]))
		{
			status("Reading GOTO program from file");

			if(read_goto_binary(cmdline.args[0],
						context, goto_functions, get_message_handler()))
				return 1;

			config.ansi_c.set_from_context(context);
		}
		else if(0!=cmdline.args.size())
		{
			if(parse()) return 1;
			if(typecheck()) return 1;
			if(final()) return 1;

			status("Generating GOTO Program");

			goto_convert(
					context, m_options, goto_functions,
					ui_message_handler);
		}

		if(0!=cmdline.args.size()) {
			::fshell2::command::Command_Processing proc(m_options, goto_functions);
			proc.finalize_goto_program(*this);
		}

		::fshell2::FShell2 fshell(m_options, goto_functions);

		if(cmdline.isset("query-file"))
		{
			// open the input file
			::std::ifstream query(cmdline.getval("query-file"));
			if(!query.is_open()) {
				::std::ostringstream ostr;
				ostr << "Failed to open query file " << cmdline.getval("query-file");
				error(ostr.str());
				return 1;
			}
			::std::string line;
			while(!::std::getline(query, line).eof() &&
					!fshell.process_line(*this, line.c_str()));
		} else {
			fshell.interactive(*this);
		}
	}

	catch(const char *e)
	{
		error(e);
		return 2;
	}

	catch(const std::string e)
	{
		error(e);
		return 2;
	}

	catch(int)
	{
		return 2;
	}

	catch(std::bad_alloc)
	{
		error("Out of memory");
		return 2;
	}

	return 0;
}

void Parseoptions::help()
{
	std::cout <<
		"FShell 2, release " FSHELL2_VERSION "\n"
		"\n"
		"(C) 2008, 2009, 2010 Michael Tautschnig, " FSHELL2_PACKAGE_BUGREPORT "\n"
		"\n"
		"Licensed under the Apache License, Version 2.0 (the 'License');\n"
		"you may not use this file except in compliance with the License.\n"
		"You may obtain a copy of the License at\n"
		"\n"
		"    http://www.apache.org/licenses/LICENSE-2.0\n"
		"\n"
		"Unless required by applicable law or agreed to in writing, software\n"
		"distributed under the License is distributed on an 'AS IS' BASIS,\n"
		"WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
		"See the License for the specific language governing permissions and\n"
		"limitations under the License.\n"
		"\n"
		"FShell 2 links to components of CBMC " CBMC_VERSION ":\n"
		"\n"
		"* *            CBMC " CBMC_VERSION " - Copyright (C) 2001-2008           * *\n"
		"* *              Daniel Kroening, Edmund Clarke             * *\n"
		"* * Carnegie Mellon University, Computer Science Department * *\n"
		"* *                 kroening@kroening.com                   * *\n"
		"* *        Protected in part by U.S. patent 7,225,417       * *\n"
		"\n"
		"Usage:                       Purpose:\n"
		"\n"
		" fshell [-?] [-h] [--help]    show help\n"
		" fshell file.c ...            source file names\n"
		"\n"
		"Frontend options:\n"
		" -I path                      set include path (C/C++)\n"
		" -D macro                     define preprocessor macro (C/C++)\n"
		" --16, --32, --64             set width of int\n"
		" --LP64, --ILP64, --LLP64,\n"
		"   --ILP32, --LP32            set width of int, long and pointers\n"
		" --little-endian              allow little-endian word-byte conversions\n"
		" --big-endian                 allow big-endian word-byte conversions\n"
		" --unsigned-char              make \"char\" unsigned by default\n"
		" --ppc-macos                  set MACOS/PPC architecture\n"
#ifdef _WIN32
		" --i386-macos                 set MACOS/I386 architecture\n"
		" --i386-linux                 set Linux/I386 architecture\n"
		" --i386-win32                 set Windows/I386 architecture (default)\n"
		" --winx64                     set Windows/X64 architecture\n"
#else
#ifdef __APPLE__
		" --i386-macos                 set MACOS/I386 architecture (default)\n"
		" --i386-linux                 set Linux/I386 architecture\n"
		" --i386-win32                 set Windows/I386 architecture\n"
		" --winx64                     set Windows/X64 architecture\n"
#else
		" --i386-macos                 set MACOS/I386 architecture\n"
		" --i386-linux                 set Linux/I386 architecture (default)\n"
		" --i386-win32                 set Windows/I386 architecture\n"
		" --winx64                     set Windows/X64 architecture\n"
#endif
#endif
		" --no-arch                    don't set up an architecture\n"
		" --no-library                 disable built-in abstract C library\n"
		"\n"
		"Program instrumentation options:\n"
		" --show-goto-functions        show goto program\n"
		" --show-loops                 show the loops in the program\n"
		" --no-assumptions             ignore user assumptions\n"
		" --no-simplify                UNDOCUMENTED\n"
		" --partial-loops              don't generate unwinding assumptions (also implies --no-unwinding-assertions)\n"
		" --enable-assert              don't ignore user assertions\n"
    	" --fixedbv                    fixed-point arithmetic\n"
    	" --floatbv                    IEEE floating point arithmetic (default)\n"
		"\n"
		"BMC options:\n"
		" --function name              set main function name\n"
		" --depth nr                   limit search depth\n"
		" --unwind nr                  unwind nr times\n"
		" --unwindset L:B,...          unwind loop L with a bound of B\n"
		"                              (use --show-loops to get the loop IDs)\n"
		" --no-unwinding-assertions    do not generate unwinding assertions\n"
		" --slice-formula              remove assignments unrelated to property\n"
		" --slice-by-trace             UNDOCUMENTED\n"
		"\n"
		"Test case output options:\n"
		" --tco-location               show locations and type info in listing of test cases\n"
		" --tco-called-functions       list sequence of function calls performed by test case\n"
		" --tco-assign-globals         show assignments to global variables performed by test case\n"
		"\n"
		"Other options:\n"
		" --query-file Filename        read FShell query from Filename\n"
		" --version                    show version and exit\n"
		" --verbosity level            set verbosity of status reports (0-9)\n"
		" --statistics                 display statistics (implied by --verbosity > 2)\n"
		" --xml-ui                     use XML-formatted output\n"
		" --outfile Filename           write test inputs to Filename (appends)\n"
		" --show-test-goals            print test goals derived from query\n"
		" --use-instrumentation        instrument trace automata into GOTO program\n"
		" --sat-coverage-check         use SAT solver for internal coverage analysis (implied by --use-instrumentation)\n"
		" --no-internal-coverage-check disable built-in coverage analysis (implies --sat-coverage-check)\n"
		"\n";
}

FSHELL2_NAMESPACE_END;

