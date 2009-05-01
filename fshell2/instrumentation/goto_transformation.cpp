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

/*! \file fshell2/instrumentation/goto_transformation.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr 30 13:50:49 CEST 2009 
*/

#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/exception/instrumentation_error.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

	
GOTO_Transformation::GOTO_Transformation(::goto_functionst & gf) :
	m_goto(gf) {
}

void GOTO_Transformation::copy_annotations(::goto_programt::const_targett src, ::goto_programt & target) {
	for (::goto_programt::targett iter(target.instructions.begin());
			iter != target.instructions.end(); ++iter) {
		iter->function = src->function;
		iter->location = src->location;
		iter->add_local_variables(src->local_variables);
	}
}

int GOTO_Transformation::insert(::std::string const& f, GOTO_Transformation::position_t const pos,
			::goto_program_instruction_typet const stmt_type, ::goto_programt const& prg) {
	::goto_functionst::function_mapt::iterator entry(m_goto.function_map.find(f));
	FSHELL2_PROD_CHECK1(::fshell2::Instrumentation_Error, entry != m_goto.function_map.end(),
			"Function " + f + " not found in goto program");
	FSHELL2_PROD_CHECK1(::fshell2::Instrumentation_Error, !entry->second.body.empty(),
			"Body of function " + f + " not available");
	FSHELL2_PROD_CHECK1(::fshell2::Instrumentation_Error, !entry->second.is_inlined(),
			"Cannot (yet) instrument inlined functions");
	
	int insert_count(0);
	for (::goto_programt::targett iter(entry->second.body.instructions.begin());
			iter != entry->second.body.instructions.end(); ++iter) {
		if (iter->type == stmt_type) {
			::goto_programt tmp;
			tmp.copy_from(prg);
			copy_annotations(iter, tmp);
			switch (pos) {
				case BEFORE:
					entry->second.body.destructive_insert(iter, tmp);
					break;
				case AFTER:
					{
						::goto_programt::targett next(iter);
						++next;
						entry->second.body.destructive_insert(next, tmp);
					}
					break;
			}
			++insert_count;
		}
	}

	entry->second.body.update();

	return insert_count;
}


FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

