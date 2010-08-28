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

#include <cbmc/src/langapi/language_ui.h>
#include <cbmc/src/util/parseoptions.h>
#include <cbmc/src/util/options.h>

#define FSHELL2_OPTIONS \
  "(function):(slice-by-trace):" \
  "(no-simplify)(unwind):(unwindset):(slice-formula)" \
  "D:I:(depth):" \
  "(no-assumptions)(no-unwinding-assertions)" \
  "(partial-loops)(xml-ui)" \
  "(16)(32)(64)(little-endian)(big-endian)" \
  "(show-goto-functions)(show-loops)" \
  "(verbosity):(no-library)" \
  "(version)(outfile):(show-test-goals)" \
  "(i386-linux)(i386-macos)(i386-win32)(winx64)" \
  "(ppc-macos)(unsigned-char)" \
  "(no-arch)(query-file):(statistics)(use-instrumentation)" \
  "(sat-subsumption)"

FSHELL2_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Parseoptions :
	public ::parseoptions_baset,
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
		::fshell2::statistics::Statistics m_stats;
		::fshell2::statistics::CPU_Timer * m_timer;
		::optionst m_options;

		void set_verbosity(::messaget &message);
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
