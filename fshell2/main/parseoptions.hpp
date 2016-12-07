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

#ifndef FSHELL2__MAIN__PARSEOPTIONS_HPP
#define FSHELL2__MAIN__PARSEOPTIONS_HPP

/*! \file fshell2/main/parseoptions.hpp
 * \brief Adapted version of CBMC's parseoptions.h
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Aug  9 12:27:33 CEST 2010
*/

#include <fshell2/config/config.hpp>

#include <fshell2/util/statistics.hpp>

#include <langapi/language_ui.h>
#include <util/parse_options.h>
#include <util/options.h>

#define FSHELL2_OPTIONS \
  "(program-only)(function):" \
  "(unwind):(unwindset):" \
  "D:I:(depth):" \
  "(no-unwinding-assertions)" \
  "(partial-loops)" \
  "(xml-ui)" \
  "(floatbv)(fixedbv)(no-assumptions)" \
  "(16)(32)(64)(LP64)(ILP64)(LLP64)(ILP32)(LP32)" \
  "(little-endian)(big-endian)" \
  "(show-goto-functions)(show-loops)(dump-c)" \
  "(verbosity):(no-library)" \
  "(version)" \
  "(i386-linux)(i386-macos)(i386-win32)(win32)(winx64)(gcc)" \
  "(ppc-macos)(unsigned-char)" \
  "(outfile):(show-test-goals)" \
  "(no-arch)" \
  "(round-to-nearest)(round-to-plus-inf)(round-to-minus-inf)(round-to-zero)" \
  "(query-file):(statistics)(use-instrumentation)" \
  "(sat-coverage-check)(no-internal-coverage-check)" \
  "(enable-assert)" \
  "(tco-location)(tco-called-functions)(tco-assign-globals)" \
  "(max-argc):"

FSHELL2_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Parseoptions :
	public ::parse_options_baset,
	public ::language_uit
{
	/*! \copydoc doc_self
	*/
	typedef Parseoptions Self;
	
	public:
		Parseoptions(int argc, const char **argv);
		Parseoptions(int argc, const char **argv, ::std::string const& extra_options);

		virtual ~Parseoptions();

		virtual int doit();
		virtual void help();

	private:
	        ::ui_message_handlert ui_message_handler;
	
		::fshell2::statistics::Statistics m_stats;
		::fshell2::statistics::CPU_Timer * m_cpu_total;
		::fshell2::statistics::Wallclock_Timer * m_session_total;
		::optionst m_options;

		void eval_verbosity();
		bool get_command_line_options();

		/*! \copydoc copy_constructor
		*/
		Parseoptions( Self const& rhs );

		/*! \copydoc assignment_op
		*/
		Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__MAIN__PARSEOPTIONS_HPP */
