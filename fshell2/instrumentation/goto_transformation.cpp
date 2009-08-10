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

GOTO_Transformation::value_t const& GOTO_Transformation::insert(::std::string const& f,
		GOTO_Transformation::position_t const pos,
		::goto_program_instruction_typet const stmt_type, ::goto_programt const& prg) {
	::goto_functionst::function_mapt::iterator entry(m_goto.function_map.find(f));
	FSHELL2_PROD_CHECK1(::fshell2::Instrumentation_Error, entry != m_goto.function_map.end(),
			"Function " + f + " not found in goto program");
	FSHELL2_PROD_CHECK1(::fshell2::Instrumentation_Error, !entry->second.body.empty(),
			"Body of function " + f + " not available");
	FSHELL2_PROD_CHECK1(::fshell2::Instrumentation_Error, !entry->second.is_inlined(),
			"Cannot (yet) instrument inlined functions");
	
	value_t bak;
	for (::goto_programt::targett iter(entry->second.body.instructions.begin());
			iter != entry->second.body.instructions.end(); ++iter) {
		if (iter->type == stmt_type) {
			::goto_programt::targett next(iter);
			++next;
			insert(pos, ::std::make_pair(iter, next), prg);
			bak.insert(m_inserted.begin(), m_inserted.end());
		}
	}

	entry->second.body.update();

	m_inserted.swap(bak);
	return m_inserted;
}
	
GOTO_Transformation::value_t const& GOTO_Transformation::insert(position_t const pos,
		::std::pair< ::goto_programt::targett, ::goto_programt::targett > const& edge,
		::goto_programt const& prg) {
	::goto_programt tmp;
	tmp.copy_from(prg);
	// TODO OUCH, we don't know which list to splice, but splice is implemented
	// as a member function of the iterator, so this should be safe at least
	::goto_programt::instructionst hack;
	m_inserted.clear();
	bool do_insert(!tmp.instructions.empty());
	switch (pos) {
		case BEFORE:
			copy_annotations(edge.first, tmp);
			// TODO should be entry->second.body.destructive_insert(edge.second,
			// tmp), but we don't know which function it is; destructive_insert
			// is implemented via splice
			hack.splice(edge.first, tmp.instructions);
			if (do_insert) {
				::goto_programt::targett pred(edge.first);
				pred--;
				m_inserted.insert(::std::make_pair(pred, edge.first));
			}
			break;
		case AFTER:
			copy_annotations(edge.second, tmp);
			::goto_programt::targett last_inst(tmp.instructions.end());
			last_inst--;
			hack.splice(edge.second, tmp.instructions);
			if (do_insert) {
				m_inserted.insert(::std::make_pair(last_inst, ++last_inst));
			}
			break;
	}
	return m_inserted;
}
	
void GOTO_Transformation::update_all() {
	for (::goto_functionst::function_mapt::iterator iter(m_goto.function_map.begin());
			iter != m_goto.function_map.end(); ++iter) {
		iter->second.body.update();
	}
}


FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

