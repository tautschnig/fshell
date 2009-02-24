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

/*! \file fshell2/exception/command_processing_error.cpp
 * \brief Implementation of Command_Processing_Error
 *
 * $Id: command_processing_error.cpp 297 2007-11-27 00:58:23Z tautschn $
 * \author Michael Tautschnig  <tautschn@model.in.tum.de>
 * \date   Sat Nov 24 13:43:04 CET 2007
*/

#include <fshell2/exception/command_processing_error.hpp>

FSHELL2_NAMESPACE_BEGIN;

Command_Processing_Error::Command_Processing_Error( ::std::string const& what )
: ::diagnostics::Low_Level_Exception( what )
{
}

Command_Processing_Error::~Command_Processing_Error() FSHELL2_NO_THROW
{
}

Command_Processing_Error::Command_Processing_Error( Self const& rhs )
: ::diagnostics::Low_Level_Exception( rhs )
{
}

char const * Command_Processing_Error::name() const
{
	return "Command_Processing_Error";
}

FSHELL2_NAMESPACE_END;

