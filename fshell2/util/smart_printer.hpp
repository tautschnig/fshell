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

#ifndef FSHELL2__UTIL__SMART_PRINTER_HPP
#define FSHELL2__UTIL__SMART_PRINTER_HPP

/*! \file fshell2/util/smart_printer.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Aug 10 13:04:50 CEST 2010 
*/

#include <fshell2/config/config.hpp>

#include <sstream>

class language_uit;

FSHELL2_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Smart_Printer
{
	/*! \copydoc doc_self
	*/
	typedef Smart_Printer Self;

	public:
		Smart_Printer(::language_uit & manager);
		~Smart_Printer();
		void print_now();
		::std::ostream & get_ostream();

	private:
		::language_uit & m_manager;
		::std::ostringstream m_oss;
	
		/*! \copydoc copy_constructor
		*/
		Smart_Printer( Self const& rhs );

		/*! \copydoc assignment_op
		*/
		Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__UTIL__SMART_PRINTER_HPP */
