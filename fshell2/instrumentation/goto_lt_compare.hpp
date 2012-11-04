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

#ifndef FSHELL2__INSTRUMENTATION__GOTO_LT_COMPARE_HPP
#define FSHELL2__INSTRUMENTATION__GOTO_LT_COMPARE_HPP

/*! \file fshell2/instrumentation/goto_lt_compare.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Nov  7 15:22:38 CET 2010 
*/

#include <fshell2/config/config.hpp>

#include <goto-programs/goto_functions.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class GOTO_Lt_Compare
{
	/*! \copydoc doc_self
	*/
	typedef GOTO_Lt_Compare Self;

	public:
	GOTO_Lt_Compare();
	
	/*! \copydoc copy_constructor
	*/
	GOTO_Lt_Compare( Self const& rhs );
	
	bool operator()(::goto_programt::const_targett const a,
			::goto_programt::const_targett const b) const;

	private:
	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__INSTRUMENTATION__GOTO_LT_COMPARE_HPP */
