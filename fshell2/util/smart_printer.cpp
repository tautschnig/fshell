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

/*! \file fshell2/util/smart_printer.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Aug 10 13:04:47 CEST 2010 
*/

#include <fshell2/util/smart_printer.hpp>

#include <cbmc/src/langapi/language_ui.h>

FSHELL2_NAMESPACE_BEGIN;

Smart_Printer::Smart_Printer(::language_uit & manager) :
	m_manager(manager) {
}

Smart_Printer::~Smart_Printer() {
	if (!m_oss.str().empty()) m_manager.print(m_oss.str());
}

void Smart_Printer::print_now() {
	if (!m_oss.str().empty()) m_manager.print(m_oss.str());
	m_oss.str("");
}

::std::ostream & Smart_Printer::get_ostream() {
	return m_oss;
}

FSHELL2_NAMESPACE_END;

