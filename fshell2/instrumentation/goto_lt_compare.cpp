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

/*! \file fshell2/instrumentation/goto_lt_compare.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Nov  7 15:22:44 CET 2010 
*/

#include <fshell2/instrumentation/goto_lt_compare.hpp>
#include <fshell2/config/annotations.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/invalid_argument.hpp>
#endif

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;
	
GOTO_Lt_Compare::GOTO_Lt_Compare()
{
}
	
GOTO_Lt_Compare::GOTO_Lt_Compare(Self const& rhs)
{
}
	
bool GOTO_Lt_Compare::operator()(::goto_programt::const_targett const a,
		::goto_programt::const_targett const b) const
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Invalid_Argument, !a->function.empty());
	FSHELL2_AUDIT_ASSERT(::diagnostics::Invalid_Argument, !b->function.empty());
	
	if (a->function < b->function) return true;
	if (b->function < a->function) return false;
	
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			(a->location_number != b->location_number) ||
			a == b);
	return (a->location_number < b->location_number);
}


FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

