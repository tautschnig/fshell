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

#ifndef FSHELL2__EXCEPTION__BASE_EXCEPTION_HPP
#define FSHELL2__EXCEPTION__BASE_EXCEPTION_HPP

/*! \file fshell2/exception/base_exception.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Aug 30 15:26:00 CEST 2010 
*/

#include <fshell2/config/config.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/low_level_exception.hpp>
#else
#  include <string>
#  include <exception>
#endif

FSHELL2_NAMESPACE_BEGIN;

/*! \brief Wrapper around ::diagnostics::Low_Level_Exception, if available.
 * Otherwise mimics ::diagnostics::Low_Level_Exception
*/
class Base_Exception : 
#if FSHELL2_DEBUG__LEVEL__ > -1
	public ::diagnostics::Low_Level_Exception
#else
	public ::std::exception
#endif
{
	/*! \copydoc doc_self
	*/
	typedef Base_Exception Self;

	public:
		/*! \brief Constructor
		 * \param what Error string
		*/
		explicit Base_Exception( ::std::string const& what );
		/*! \brief Destructor
		*/
		virtual ~Base_Exception() FSHELL2_NO_THROW;
		/*! \brief Copy constructor
		 * \param rhs Copy from this object
		*/
		Base_Exception( Self const& rhs );

	private:
	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );

#if FSHELL2_DEBUG__LEVEL__ == -1
	public:
	virtual char const * name() const =0;
	virtual char const * what() const FSHELL2_NO_THROW;
	
	protected:
	::std::string m_what;
#endif
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__EXCEPTION__BASE_EXCEPTION_HPP */
