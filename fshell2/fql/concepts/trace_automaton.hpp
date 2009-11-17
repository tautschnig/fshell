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

#ifndef FSHELL2__FQL__CONCEPTS__TRACE_AUTOMATON_HPP
#define FSHELL2__FQL__CONCEPTS__TRACE_AUTOMATON_HPP

/*! \file fshell2/fql/concepts/trace_automaton.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Oct 27 20:09:04 CET 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/concepts/target_graph.hpp>

#include <map>
#include <set>
#include <astl/include/nfa_mmap_backedge.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Target_Graph_Index
{
	/*! \copydoc doc_self
	*/
	typedef Target_Graph_Index Self;
	
	public:
	typedef int char_type;

	explicit Target_Graph_Index(target_graph_t const* id_tgg);

	int to_index(target_graph_t const* f);

	static bool lt(const char_type x, const char_type y) {
		return x < y;
	}

	int id_index() const {
		return 0;
	}

	static const int epsilon = -1;
	
	int epsilon_index() const {
		return epsilon;
	}

	int lookup_target_graph(target_graph_t const* f) const;
	target_graph_t const* lookup_index(int index) const;

	private:
	::std::map< target_graph_t const*, int > m_target_graph_to_int;
	::std::map< int, target_graph_t const* > m_int_to_target_graph;
	int m_next_index;

	/*! \copydoc copy_constructor
	*/
	Target_Graph_Index(Self const& rhs);
	
	/*! \copydoc assignment_op
	 */
	Self& operator=(Self const& rhs);
};
	
typedef ::astl::NFA_mmap_backedge< Target_Graph_Index > trace_automaton_t;
typedef trace_automaton_t::state_type ta_state_t;
typedef ::std::set< ta_state_t > ta_state_set_t;

::std::pair< ta_state_set_t, ta_state_set_t > copy_automaton(trace_automaton_t const& src, trace_automaton_t & dest);

void simplify_automaton(trace_automaton_t & aut, bool compact);

::std::ostream & print_trace_automaton(trace_automaton_t const& aut, ::std::ostream & os);

void find_non_eps_pred(trace_automaton_t const& aut, ta_state_t const& s, ::std::set< Target_Graph_Index::char_type > & indices);
void find_non_eps_succ(trace_automaton_t const& aut, ta_state_t const& s, ::std::set< Target_Graph_Index::char_type > & indices);

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__CONCEPTS__TRACE_AUTOMATON_HPP */
