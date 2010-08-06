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

#ifndef FSHELL2__FQL__CONCEPTS__CFA_HPP
#define FSHELL2__FQL__CONCEPTS__CFA_HPP

/*! \file fshell2/fql/concepts/cfa.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Aug 11 08:28:13 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

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
	
	typedef ::std::set< node_t > nodes_t;
	typedef ::std::set< edge_t > edges_t;
	typedef nodes_t initial_states_t;

	CFA();

	/*! \copydoc copy_constructor
	*/
	CFA( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );

	/*! \{
	 * \brief Get nodes (program locations) of CFA 
	 *
	 * \return Reference to disconnected nodes (those nodes not contained in
	 * edges) or new set of nodes that includes all nodes.
	*/
	inline nodes_t const& get_disconnected_nodes() const;
	nodes_t const get_L() const;
	/*! \} */
	/*! \brief Set disconnected nodes of CFA
	 * \param nodes Set of nodes.
	 * \pre None of @a nodes must be contained in any of the edges.
	 * \post CFA has @a nodes as new disconnected nodes, values of @a nodes is
	 * set to previous set of disconnected nodes.
	*/
	inline void set_disconnected_nodes(nodes_t & nodes);

	/*! \brief Get CFA edges
	 *
	 * \return Reference to set of edges
	*/
	inline edges_t const& get_E() const;
	/*! \brief Set CFA edges
	 * \param edges Set of edges.
	 * \post CFA has @a edges as new edges, values of @a edges is set to
	 * previous set of edges.
	*/
	inline void set_E(edges_t & edges);

	/*! \brief Get CFA initial states
	 *
	 * \return Reference to set of initial states
	*/
	inline initial_states_t const& get_I() const;
	/*! \brief Set CFA initial states
	 * \param initial Set of edges.
	 * \pre All nodes in @a initial must either be contained in edges or be
	 * disconnected nodes of the CFA.
	 * \post CFA has @a initial as new initial states, values of @a initial is
	 * set to previous set of initial states.
	*/
	inline void set_I(initial_states_t & initial);

	/*! \{
	 * \brief Set-theoretic operations on CFAs: union, intersection and setminus
	 * \param other CFA that is united/intersected with or subtracted from this
	 * CFA.
	 * \return Reference modified (current) CFA.
	*/
	Self& setunion(Self const& other);
	Self& setintersection(Self const& other);
	Self& setsubtract(Self const& other);
	/*! \} */

	/*! \brief Diagnostics class invariant
	 *
	 * The set of disconnected nodes must not intersect with the nodes contained
	 * in edges. Initial states must be contained in edges or disconnected
	 * nodes.
	*/
	void m_class_invariance() const;

	private:
	nodes_t m_disconnected_nodes;
	edges_t m_edges;
	initial_states_t m_initial;
};
	
CFA::nodes_t const& CFA::get_disconnected_nodes() const {
	return m_disconnected_nodes;
}

void CFA::set_disconnected_nodes(nodes_t & nodes) {
	FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD;
	m_disconnected_nodes.swap(nodes);
}

CFA::edges_t const& CFA::get_E() const {
	return m_edges;
}

void CFA::set_E(edges_t & edges) {
	FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD;
	m_edges.swap(edges);
}

CFA::initial_states_t const& CFA::get_I() const {
	return m_initial;
}
	
void CFA::set_I(initial_states_t & initial) {
	FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD;
	m_initial.swap(initial);
}
	

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__CONCEPTS__CFA_HPP */
