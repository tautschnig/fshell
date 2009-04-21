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

#include <cbmc/src/goto-programs/goto_functions.h>

class language_uit;
class optionst;

FSHELL2_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class FShell2
{
	/*! \copydoc doc_self
	*/
	typedef FShell2 Self;

	public:
	static FShell2 & get_instance();

	bool process_line(::language_uit & manager, ::std::ostream & os, char const * line);

	void interactive(::language_uit & manager, ::std::ostream & os);

	void set_options(::optionst const& opts);

	void set_primary_cfg(::goto_functionst & cfg);

	~FShell2();

	private:
	FShell2();

	// this is really painful, we should get rid of this singleton stuff and do
	// proper construction
	::optionst const * m_opts;
	::goto_functionst * m_cfg;

	void try_query(::language_uit & manager, ::std::ostream & os, char const * line);

	/*! \copydoc copy_constructor
	*/
	FShell2( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__MAIN__FSHELL2_HPP */
