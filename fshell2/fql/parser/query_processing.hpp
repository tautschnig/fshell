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

#ifndef FSHELL2__FQL__PARSER__QUERY_PROCESSING_HPP
#define FSHELL2__FQL__PARSER__QUERY_PROCESSING_HPP

/*! \file fshell2/fql/parser/query_processing.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:44 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <iostream>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Query;

/*! \brief TODO
*/
class Query_Processing
{
	/*! \copydoc doc_self
	*/
	typedef Query_Processing Self;

	public:
	Query_Processing();

	void parse(::std::ostream & os, char const * query, Query ** query_ast);

	static ::std::ostream & help(::std::ostream & os);

	private:
	/*! \copydoc copy_constructor
	*/
	Query_Processing( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__PARSER__QUERY_PROCESSING_HPP */
