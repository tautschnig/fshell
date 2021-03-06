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

/*! \file fshell2/fql/concepts/cfa.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Aug 11 11:14:21 CEST 2009 
*/

#include <fshell2/fql/concepts/cfa.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/invalid_argument.hpp>
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#endif

#include <fshell2/instrumentation/goto_lt_compare.hpp>

#include <algorithm>
#include <iterator>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

bool CFA::CFANode_Lt_Compare::operator()(node_t const& a, node_t const& b) const
{
	::fshell2::instrumentation::GOTO_Lt_Compare lt_cmp;
	if (lt_cmp(a.second, b.second)) return true;
	if (lt_cmp(b.second, a.second)) return false;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a.first == b.first);
	return false;
}

bool CFA::CFAEdge_Lt_Compare::operator()(edge_t const& a, edge_t const& b) const
{
	::fshell2::instrumentation::GOTO_Lt_Compare lt_cmp;
	if (lt_cmp(a.first.second, b.first.second)) return true;
	if (lt_cmp(b.first.second, a.first.second)) return false;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a.first == b.first);
	CFA::CFANode_Lt_Compare node_lt_cmp;
	return node_lt_cmp(a.second, b.second);
}

CFA::CFA() {
}

CFA::CFA(Self const& rhs) :
	m_disconnected_nodes(rhs.m_disconnected_nodes),
	m_edges(rhs.m_edges),
	m_initial(rhs.m_initial)
{
	FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD;
}
	
CFA& CFA::operator=(Self const& rhs)
{
	FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD;
	
	m_disconnected_nodes = rhs.m_disconnected_nodes;
	m_edges = rhs.m_edges;
	m_initial = rhs.m_initial;

	return *this;
}
	
void CFA::m_class_invariance() const {
	// m_disconnected_nodes must not intersect with nodes of m_edges
	for (edges_t::const_iterator iter(m_edges.begin());
			iter != m_edges.end(); ++iter) {
		FSHELL2_DEBUG_CHECK(::diagnostics::Invalid_Argument,
			m_disconnected_nodes.end() == m_disconnected_nodes.find(iter->first));
		FSHELL2_DEBUG_CHECK(::diagnostics::Invalid_Argument,
			m_disconnected_nodes.end() == m_disconnected_nodes.find(iter->second));
	}
	
	// m_initial must be contained in at least one of m_disconnected_nodes or
	// nodes of m_edges
	for (nodes_t::const_iterator iter(m_initial.begin());
			iter != m_initial.end(); ++iter) {
		bool found(false);
		found = (m_disconnected_nodes.end() != m_disconnected_nodes.find(*iter));
		for (edges_t::const_iterator iter2(m_edges.begin());
				!found && iter2 != m_edges.end(); ++iter2)
			found = (iter2->first == *iter) || (iter2->second == *iter);
		FSHELL2_DEBUG_CHECK(::diagnostics::Invalid_Argument, found);
	}
}
	
CFA::nodes_t const CFA::get_L() const {
	nodes_t all_nodes(m_disconnected_nodes);
	for (edges_t::const_iterator iter(m_edges.begin()); iter != m_edges.end(); ++iter) {
		all_nodes.insert(iter->first);
		all_nodes.insert(iter->second);
	}
	return all_nodes;
}

CFA& CFA::setunion(Self const& other) {
	FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD;

	::std::copy(other.m_disconnected_nodes.begin(), other.m_disconnected_nodes.end(),
			::std::inserter(m_disconnected_nodes, m_disconnected_nodes.begin()));
	::std::copy(other.m_edges.begin(), other.m_edges.end(),
			::std::inserter(m_edges, m_edges.begin()));
	::std::copy(other.m_initial.begin(), other.m_initial.end(),
			::std::inserter(m_initial, m_initial.begin()));
	
	// remove now non-disconnected nodes
	for (edges_t::const_iterator iter(m_edges.begin());
			iter != m_edges.end(); ++iter) {
		nodes_t::iterator entry(m_disconnected_nodes.find(iter->first));
		if (m_disconnected_nodes.end() != entry) m_disconnected_nodes.erase(entry);
		entry = m_disconnected_nodes.find(iter->second);
		if (m_disconnected_nodes.end() != entry) m_disconnected_nodes.erase(entry);
	}

	return *this;
}

CFA& CFA::setintersection(Self const& other) {
	FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD;

	CFANode_Lt_Compare node_lt_cmp;
	CFAEdge_Lt_Compare edge_lt_cmp;

	// first compute intersection of all nodes
	nodes_t L(get_L()), other_L(other.get_L());
	m_disconnected_nodes.clear();
	::std::set_intersection(L.begin(), L.end(), other_L.begin(), other_L.end(),
			::std::inserter(m_disconnected_nodes, m_disconnected_nodes.begin()),
			node_lt_cmp);

	edges_t old_edges;
	old_edges.swap(m_edges);
	::std::set_intersection(old_edges.begin(), old_edges.end(),
			other.m_edges.begin(), other.m_edges.end(),
			::std::inserter(m_edges, m_edges.begin()),
			edge_lt_cmp);
	
	initial_states_t old_initial;
	old_initial.swap(m_initial);
	::std::set_intersection(old_initial.begin(), old_initial.end(),
			other.m_initial.begin(), other.m_initial.end(),
			::std::inserter(m_initial, m_initial.begin()),
			node_lt_cmp);
	
	// remove now non-disconnected nodes
	for (edges_t::const_iterator iter(m_edges.begin());
			iter != m_edges.end(); ++iter) {
		nodes_t::iterator entry(m_disconnected_nodes.find(iter->first));
		if (m_disconnected_nodes.end() != entry) m_disconnected_nodes.erase(entry);
		entry = m_disconnected_nodes.find(iter->second);
		if (m_disconnected_nodes.end() != entry) m_disconnected_nodes.erase(entry);
	}

	return *this;
}

CFA& CFA::setsubtract(Self const& other) {
	FSHELL2_AUDIT_CLASS_INVARIANCE_GUARD;
	
	CFANode_Lt_Compare node_lt_cmp;
	CFAEdge_Lt_Compare edge_lt_cmp;

	// first compute set difference of all nodes
	nodes_t L(get_L()), other_L(other.get_L());
	m_disconnected_nodes.clear();
	::std::set_difference(L.begin(), L.end(), other_L.begin(), other_L.end(),
			::std::inserter(m_disconnected_nodes, m_disconnected_nodes.begin()),
			node_lt_cmp);

	edges_t old_edges;
	old_edges.swap(m_edges);
	::std::set_difference(old_edges.begin(), old_edges.end(),
			other.m_edges.begin(), other.m_edges.end(),
			::std::inserter(m_edges, m_edges.begin()),
			edge_lt_cmp);
	// remove edges where at least one of the nodes is not in L anymore
	// NO, don't do that, ASE paper says these nodes stay
	// for (edges_t::iterator iter(m_edges.begin()); iter != m_edges.end();) {
	// 	if ((m_disconnected_nodes.end() == m_disconnected_nodes.find(iter->first)) ||
	// 			(m_disconnected_nodes.end() == m_disconnected_nodes.find(iter->second))) {
	// 		edges_t::iterator del_iter(iter++);
	// 		m_edges.erase(del_iter);
	// 		continue;
	// 	}
	// 	++iter;
	// }
	
	// limit initial states to L
	initial_states_t old_initial;
	old_initial.swap(m_initial);
	::std::set_intersection(m_disconnected_nodes.begin(), m_disconnected_nodes.end(),
			old_initial.begin(), old_initial.end(),
			::std::inserter(m_initial, m_initial.begin()),
			node_lt_cmp);

	// remove now non-disconnected nodes
	for (edges_t::const_iterator iter(m_edges.begin());
			iter != m_edges.end(); ++iter) {
		nodes_t::iterator entry(m_disconnected_nodes.find(iter->first));
		if (m_disconnected_nodes.end() != entry) m_disconnected_nodes.erase(entry);
		entry = m_disconnected_nodes.find(iter->second);
		if (m_disconnected_nodes.end() != entry) m_disconnected_nodes.erase(entry);
	}
	
	return *this;
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

