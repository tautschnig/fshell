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

#ifndef FSHELL2__MAIN__FSHELL2_HPP
#define FSHELL2__MAIN__FSHELL2_HPP

/*! \file fshell2/main/fshell2.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Apr 15 11:58:49 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <iostream>

#include <fshell2/command/command_processing.hpp>
#include <fshell2/macro/macro_processing.hpp>
#include <fshell2/fql/parser/query_parser.hpp>

#include <cbmc/src/goto-programs/goto_functions.h>

class language_uit;
class optionst;

FSHELL2_NAMESPACE_BEGIN;

FSHELL2_STATISTICS_NAMESPACE_BEGIN;

class Statistics;

FSHELL2_STATISTICS_NAMESPACE_END;

/*! \brief TODO
*/
class FShell2
{
	/*! \copydoc doc_self
	*/
	typedef FShell2 Self;

	public:
	FShell2(::optionst const& opts, ::goto_functionst & gf);

	bool process_line(::language_uit & manager, char const * line);
	bool process_line(::language_uit & manager, char const * line,
			::fshell2::statistics::Statistics & stats);

	void interactive(::language_uit & manager);

	~FShell2();

	private:
	::optionst const& m_opts;
	::goto_functionst & m_gf;
	::fshell2::command::Command_Processing m_cmd;
	::fshell2::macro::Macro_Processing m_macro;
	::fshell2::fql::Query_Parser m_fql_parser;
	bool m_first_run;

	void try_query(::language_uit & manager, char const * line,
			::fshell2::statistics::Statistics & stats);

	/*! \copydoc copy_constructor
	*/
	FShell2( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__MAIN__FSHELL2_HPP */
