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

#ifndef FSHELL2__EXCEPTION__QUERY_PROCESSING_ERROR_HPP
#define FSHELL2__EXCEPTION__QUERY_PROCESSING_ERROR_HPP

/*! \file fshell2/exception/query_processing_error.hpp
 * \brief Interface of Query_Processing_Error
 *
 * $Id$
 * \author Michael Tautschnig  <tautschn@model.in.tum.de>
 * \date   Sat Nov 24 13:43:04 CET 2007
*/

#include <fshell2/config/config.hpp>
#include <diagnostics/basic_exceptions/low_level_exception.hpp>

FSHELL2_NAMESPACE_BEGIN;

/*! \brief Errors thrown in query processing
*/
class Query_Processing_Error : public ::diagnostics::Low_Level_Exception
{
	private:
		/*! \copydoc doc_self
		*/
		typedef Query_Processing_Error Self;

	public:
		/*! \brief Constructor
		 * \param what Error string
		*/
		explicit Query_Processing_Error( ::std::string const& what );
		/*! \brief Destructor
		*/
		virtual ~Query_Processing_Error() FSHELL2_NO_THROW;
		/*! \brief Copy constructor
		 * \param rhs Copy from this object
		*/
		Query_Processing_Error( Self const& rhs );
		/*! \brief Implementation of @ref fshell2::Exception
		*/
		virtual char const * name() const;
	private:
		/*! \brief Default constructor is not available
		*/
		Query_Processing_Error();
		/*! \copydoc assignment_op
		*/
		Self& operator=( Self const& rhs );
};

FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__EXCEPTION__QUERY_PROCESSING_ERROR_HPP */

