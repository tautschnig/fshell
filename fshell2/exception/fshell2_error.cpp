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

/*! \file fshell2/exception/fshell2_error.cpp
 * \brief Implementation of FShell2_Error
 *
 * $Id$
 * \author Michael Tautschnig  <tautschn@model.in.tum.de>
 * \date   Fri May  1 14:36:52 CEST 2009
*/

#include <fshell2/exception/fshell2_error.hpp>

FSHELL2_NAMESPACE_BEGIN;

FShell2_Error::FShell2_Error( ::std::string const& what )
: ::diagnostics::Low_Level_Exception( what )
{
}

FShell2_Error::~FShell2_Error() FSHELL2_NO_THROW
{
}

FShell2_Error::FShell2_Error( Self const& rhs )
: ::diagnostics::Low_Level_Exception( rhs )
{
}

char const * FShell2_Error::name() const
{
	return "FShell2_Error";
}

FSHELL2_NAMESPACE_END;

