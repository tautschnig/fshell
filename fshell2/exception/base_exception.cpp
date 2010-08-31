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

/*! \file fshell2/exception/base_exception.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Aug 30 15:26:03 CEST 2010 
*/

#include <fshell2/exception/base_exception.hpp>

FSHELL2_NAMESPACE_BEGIN;

Base_Exception::~Base_Exception() FSHELL2_NO_THROW
{
}

#if FSHELL2_DEBUG__LEVEL__ > -1
Base_Exception::Base_Exception( ::std::string const& what )
: ::diagnostics::Low_Level_Exception( what )
{
}

Base_Exception::Base_Exception( Self const& rhs )
: ::diagnostics::Low_Level_Exception( rhs )
{
}

#else
Base_Exception::Base_Exception( ::std::string const& what )
: m_what( what )
{
}

Base_Exception::Base_Exception( Self const& rhs )
: m_what( rhs.m_what )
{
}

char const * Base_Exception::what() const FSHELL2_NO_THROW
{
	return m_what.c_str();
}
#endif

FSHELL2_NAMESPACE_END;

