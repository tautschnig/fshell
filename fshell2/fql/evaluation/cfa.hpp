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

#ifndef FSHELL2__FQL__EVALUATION__CFA_HPP
#define FSHELL2__FQL__EVALUATION__CFA_HPP

/*! \file fshell2/fql/evaluation/cfa.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Aug 11 08:28:13 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <set>

#include <cbmc/src/goto-programs/goto_functions.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class CFA
{
	/*! \copydoc doc_self
	*/
	typedef CFA Self;

	public:
	typedef ::std::pair< ::goto_programt *, ::goto_programt::targett > node_t;
	typedef ::std::pair< node_t, node_t > edge_t;
	typedef ::std::set< edge_t > edges_t;
	typedef ::std::set< node_t > initial_states_t;

	CFA();

	~CFA();
	
	/*! \copydoc copy_constructor
	*/
	CFA( Self const& rhs );

	inline edges_t const& get_edges() const;
	inline void set_edges(edges_t & edges);

	inline initial_states_t const& get_initial_states() const;
	inline void set_initial_states(initial_states_t & initial);

	private:
	edges_t m_edges;
	initial_states_t m_initial;

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};
	
CFA::edges_t const& CFA::get_edges() const {
	return m_edges;
}

void CFA::set_edges(edges_t & edges) {
	m_edges.swap(edges);
}

CFA::initial_states_t const& CFA::get_initial_states() const {
	return m_initial;
}

void CFA::set_initial_states(initial_states_t & initial) {
	m_initial.swap(initial);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__CFA_HPP */
