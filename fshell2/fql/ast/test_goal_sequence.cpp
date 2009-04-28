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

/*! \file fshell2/fql/ast/test_goal_sequence.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:54 CEST 2009 
*/

#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/invalid_argument.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Test_Goal_Sequence::Test_Goal_Sequence(Test_Goal_Sequence::seq_t & seq,
		Restriction_Automaton * suffix_aut) :
	m_seq(seq), m_suffix_aut(suffix_aut) {
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, !m_seq.empty());
}

void Test_Goal_Sequence::accept(AST_Visitor * v) const {
	v->visit(this);
}

void Test_Goal_Sequence::accept(AST_Visitor const * v) const {
	v->visit(this);
}

bool Test_Goal_Sequence::destroy() {
	if (this->m_ref_count) return false;
	Factory::get_instance().destroy(this);
	for (seq_t::iterator iter(m_seq.begin()); iter != m_seq.end(); ++iter) {
		if (iter->first) {
			iter->first->decr_ref_count();
			iter->first->destroy();
		}
		iter->second->decr_ref_count();
		iter->second->destroy();
	}
	m_seq.clear();
	if (m_suffix_aut) {
		m_suffix_aut->decr_ref_count();
		m_suffix_aut->destroy();
	}
	return true;
}

Test_Goal_Sequence::~Test_Goal_Sequence() {
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

