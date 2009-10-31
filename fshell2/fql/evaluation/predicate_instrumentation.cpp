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

/*! \file fshell2/fql/evaluation/predicate_instrumentation.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Oct 31 17:33:19 CET 2009 
*/

#include <fshell2/fql/evaluation/predicate_instrumentation.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Predicate_Instrumentation::Predicate_Instrumentation(::goto_functionst & gf) :
	m_gf(gf) {
}

Predicate_Instrumentation::~Predicate_Instrumentation() {
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

