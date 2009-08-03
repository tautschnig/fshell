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

#ifndef FSHELL2__MACRO__MACRO_PROCESSING_HPP
#define FSHELL2__MACRO__MACRO_PROCESSING_HPP

/*! \file fshell2/macro/macro_processing.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:07 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <iostream>
#include <string>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_MACRO_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Macro_Processing
{
	/*! \copydoc doc_self
	*/
	typedef Macro_Processing Self;

	public:
	Macro_Processing();
	
	~Macro_Processing();

	::std::string expand(char const * cmd);

	static ::std::ostream & help(::std::ostream & os);

	private:
	bool m_has_defines;
	char * m_deffilename;
	char * m_checkfilename;
	int m_file_index;
	char ** m_cpp_argv;

	int preprocess(char * filename, ::std::ostream & out) const;

	/*! \copydoc copy_constructor
	*/
	Macro_Processing( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_MACRO_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__MACRO__MACRO_PROCESSING_HPP */
