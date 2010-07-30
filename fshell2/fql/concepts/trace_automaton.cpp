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

/*! \file fshell2/fql/concepts/trace_automaton.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Oct 27 20:09:09 CET 2009 
*/

#include <fshell2/fql/concepts/trace_automaton.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/invalid_argument.hpp>

#include <limits>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Target_Graph_Index::Target_Graph_Index(
		target_graph_t const* id_tgg) :
	m_next_index(0) {
	to_index(id_tgg);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_next_index);
}

int Target_Graph_Index::to_index(target_graph_t const* f) {
	::std::map< target_graph_t const*, int >::iterator entry(m_target_graph_to_int.find(f));
	if (m_target_graph_to_int.end() == entry) {
		FSHELL2_PROD_CHECK(::diagnostics::Violated_Invariance,
				m_next_index < ::std::numeric_limits<int>::max());
		entry = m_target_graph_to_int.insert(::std::make_pair(f, m_next_index++)).first;
		m_int_to_target_graph.insert(::std::make_pair(entry->second, f));
	}
	return entry->second;
}

target_graph_t const* Target_Graph_Index::lookup_index(int index) const {
	::std::map< int, target_graph_t const* >::const_iterator entry(m_int_to_target_graph.find(index));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, m_int_to_target_graph.end() != entry);
	return entry->second;
}

int Target_Graph_Index::lookup_target_graph(target_graph_t const* f) const {
	::std::map< target_graph_t const*, int >::const_iterator entry(m_target_graph_to_int.find(f));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, m_target_graph_to_int.end() != entry);
	return entry->second;
}

::std::pair< ta_state_set_t, ta_state_set_t > copy_automaton(trace_automaton_t const& src, trace_automaton_t & dest) {
	::std::pair< ta_state_set_t, ta_state_set_t > init_final;

	typedef ::std::map< ta_state_t, ta_state_t > state_map_t;
	state_map_t new_states;

	for (trace_automaton_t::const_iterator iter(src.begin()); iter != src.end(); ++iter) {
		state_map_t::const_iterator s1_entry(new_states.find(*iter));
		if (new_states.end() == s1_entry)
			s1_entry = new_states.insert(::std::make_pair(*iter, dest.new_state())).first;
		if (src.initial().end() != src.initial().find(*iter))
			init_final.first.insert(s1_entry->second);
		if (src.final(*iter))
			init_final.second.insert(s1_entry->second);
		
		trace_automaton_t::edges_type out_edges(src.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator o_iter(out_edges.begin());
				o_iter != out_edges.end(); ++o_iter) {
			state_map_t::const_iterator s2_entry(new_states.find(o_iter->second));
			if (new_states.end() == s2_entry)
				s2_entry = new_states.insert(::std::make_pair(o_iter->second, dest.new_state())).first;
			dest.set_trans(s1_entry->second, o_iter->first, s2_entry->second);
		}
	}

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			!init_final.first.empty() && !init_final.second.empty());
	return init_final;
}

void simplify_automaton(trace_automaton_t & aut, bool compact) {
#if FSHELL2_DEBUG__LEVEL__ >= 2
	::std::ostringstream oss;
	oss << "Input AUT:" << ::std::endl;
	print_trace_automaton(aut, oss);
	FSHELL2_AUDIT_TRACE(oss.str());
	oss.str("");
#endif

	for (trace_automaton_t::const_iterator iter(aut.begin()); iter != aut.end(); ++iter) {
		trace_automaton_t::edges_type in_edges(aut.delta2_backwards(*iter));
		trace_automaton_t::edges_type out_edges(aut.delta2(*iter));
		// remove all incoming edges labeled with -1 and clone final state
		// markers, if necessary
		for (trace_automaton_t::edges_type::const_iterator i_iter(in_edges.begin());
				i_iter != in_edges.end();) {
			if (Target_Graph_Index::epsilon == i_iter->first) {
				// self-loops on -1 can be discarded, others must be
				// re-routed
				if (i_iter->second != *iter)
					for (trace_automaton_t::edges_type::const_iterator o_iter(out_edges.begin());
							o_iter != out_edges.end(); ++o_iter)
						aut.set_trans(i_iter->second, o_iter->first, o_iter->second);
				// copy acceptance marker
				if (aut.final(*iter)) aut.final(i_iter->second) = 1;
				// edge done, remove it
				trace_automaton_t::edges_type::const_iterator i_iter_bak(i_iter);
				++i_iter;
				aut.del_trans(i_iter_bak->second, i_iter_bak->first, *iter);
			} else {
				++i_iter;
			}
		}
	}

	// remove dead states
	::std::list< ta_state_t > queue;
	::std::copy(aut.begin(), aut.end(), ::std::back_inserter(queue));
	ta_state_set_t dead_states;

	while (!queue.empty()) {
		ta_state_t const state(queue.front());
		queue.pop_front();
		if (dead_states.end() != dead_states.find(state)) continue;
		trace_automaton_t::edges_type in_edges(aut.delta2_backwards(state));
		trace_automaton_t::edges_type out_edges(aut.delta2(state));
		if (aut.initial().end() != aut.initial().find(state)) continue;
		bool del_state(false);
		if (in_edges.empty()) {
			for (trace_automaton_t::edges_type::const_iterator o_iter(out_edges.begin());
					o_iter != out_edges.end();) {
				queue.push_back(o_iter->second);
				trace_automaton_t::edges_type::const_iterator o_iter_bak(o_iter);
				++o_iter;
				aut.del_trans(state, o_iter_bak->first, o_iter_bak->second);
			}
			del_state = true;
		} else if (out_edges.empty()) {
			if (aut.final(state)) continue;
			for (trace_automaton_t::edges_type::const_iterator i_iter(in_edges.begin());
					i_iter != in_edges.end();) {
				queue.push_back(i_iter->second);
				trace_automaton_t::edges_type::const_iterator i_iter_bak(i_iter);
				++i_iter;
				aut.del_trans(i_iter_bak->second, i_iter_bak->first, state);
			}
			del_state = true;
		}

		if (del_state) {
			dead_states.insert(state);
			aut.del_state(state);
		}
	}

	// remove duplicate edges
	for (trace_automaton_t::const_iterator iter(aut.begin()); iter != aut.end(); ++iter) {
		trace_automaton_t::edges_type out_edges(aut.delta2(*iter));
		::std::set< ::std::pair< trace_automaton_t::char_type, ta_state_t > > seen_edges;
		for (trace_automaton_t::edges_type::const_iterator o_iter(out_edges.begin());
				o_iter != out_edges.end();) {
			if (!seen_edges.insert(::std::make_pair(o_iter->first, o_iter->second)).second) {
				// edge done, remove it
				trace_automaton_t::edges_type::const_iterator o_iter_bak(o_iter);
				++o_iter;
				aut.del_trans(*iter, o_iter_bak->first, o_iter_bak->second);
			} else {
				++o_iter;
			}
		}
	}

	if (compact) {
		// make sure we only have the smallest necessary number of states; first
		// backup, then clear entirely, then copy back
		trace_automaton_t clone;
		::std::pair< ta_state_set_t, ta_state_set_t > init_final(copy_automaton(aut, clone));
		clone.initial().swap(init_final.first);
		for (ta_state_set_t::const_iterator iter(init_final.second.begin());
				iter != init_final.second.end(); ++iter)
			clone.final(*iter) = 1;
		// clear
		for (trace_automaton_t::const_iterator iter(aut.begin()); iter != aut.end(); ++iter)
			aut.del_state(*iter);
		// copy back
		::std::pair< ta_state_set_t, ta_state_set_t > init_final2(copy_automaton(clone, aut));
		aut.initial().swap(init_final2.first);
		for (ta_state_set_t::const_iterator iter(init_final2.second.begin());
				iter != init_final2.second.end(); ++iter)
			aut.final(*iter) = 1;
	}
	
#if FSHELL2_DEBUG__LEVEL__ >= 2
	oss << "Simplified AUT:" << ::std::endl;
	print_trace_automaton(aut, oss);
	FSHELL2_AUDIT_TRACE(oss.str());
#endif
}

::std::ostream & print_trace_automaton(trace_automaton_t const& aut, ::std::ostream & os) {
	for (trace_automaton_t::const_iterator iter(aut.begin()); iter != aut.end(); ++iter) {
		trace_automaton_t::edges_type out_edges(aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator o_iter(out_edges.begin());
				o_iter != out_edges.end(); ++o_iter) {
			if (aut.initial().end() != aut.initial().find(*iter)) os << ".";
			os << *iter;
			if (aut.final(*iter)) os << ".";
			os << " -[ " << o_iter->first << " ]-> ";
			if (aut.initial().end() != aut.initial().find(o_iter->second)) os << ".";
			os << o_iter->second;
			if (aut.final(o_iter->second)) os << ".";
			os << ::std::endl;
		}
	}

	return os;
}

void find_non_eps_pred(trace_automaton_t const& aut, ta_state_t const& s, ::std::set< Target_Graph_Index::char_type > & indices) {
	ta_state_set_t seen_states;
	::std::list< ta_state_t > queue;
	queue.push_back(s);
	while (!queue.empty()) {
		ta_state_t const st(queue.front());
		queue.pop_front();
		if (seen_states.end() != seen_states.find(st)) continue;
		trace_automaton_t::edges_type in_edges(aut.delta2_backwards(st));
		for (trace_automaton_t::edges_type::const_iterator i_iter(in_edges.begin());
				i_iter != in_edges.end(); ++i_iter) {
			if (Target_Graph_Index::epsilon == i_iter->first) {
				queue.push_back(i_iter->second);
			} else {
				indices.insert(i_iter->first);
			}
		}
		seen_states.insert(st);
	}
}
	
void find_non_eps_succ(trace_automaton_t const& aut, ta_state_t const& s, ::std::set< Target_Graph_Index::char_type > & indices) {
	ta_state_set_t seen_states;
	::std::list< ta_state_t > queue;
	queue.push_back(s);
	while (!queue.empty()) {
		ta_state_t const st(queue.front());
		queue.pop_front();
		if (seen_states.end() != seen_states.find(st)) continue;
		trace_automaton_t::edges_type out_edges(aut.delta2(st));
		for (trace_automaton_t::edges_type::const_iterator o_iter(out_edges.begin());
				o_iter != out_edges.end(); ++o_iter) {
			if (Target_Graph_Index::epsilon == o_iter->first) {
				queue.push_back(o_iter->second);
			} else {
				indices.insert(o_iter->first);
			}
		}
		seen_states.insert(st);
	}
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

