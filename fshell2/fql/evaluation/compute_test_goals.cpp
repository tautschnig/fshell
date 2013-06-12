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

/*! \file fshell2/fql/evaluation/compute_test_goals.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri May  1 21:45:40 CEST 2009 
*/

#include <fshell2/fql/evaluation/compute_test_goals.hpp>
#include <fshell2/config/annotations.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/not_implemented.hpp>
#endif

#include <fshell2/util/smart_printer.hpp>
#include <fshell2/util/statistics.hpp>

#include <fshell2/exception/fshell2_error.hpp>

#include <fshell2/instrumentation/cfg.hpp>

#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/fql/evaluation/evaluate_path_pattern.hpp>
#include <fshell2/fql/evaluation/evaluate_coverage_pattern.hpp>
#include <fshell2/fql/evaluation/automaton_inserter.hpp>

#include <fshell2/fql/ast/cp_alternative.hpp>
#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/depcov.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/nodecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>

#include <goto-instrument/dump_c.h>

#include <limits>
#include <algorithm>
#include <iterator>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

CNF_Conversion::CNF_Conversion(::language_uit & manager, ::optionst const& opts) :
	::bmct(opts, manager.symbol_table, manager.ui_message_handler),
	m_opts(opts), m_cnf(), m_bv(this->ns, m_cnf) {
	this->set_verbosity(manager.get_verbosity());
	this->set_ui(manager.ui_message_handler.get_ui());

	m_cnf.set_message_handler(*(this->message_handler));
	m_cnf.set_verbosity(this->get_verbosity());
	m_bv.set_message_handler(*(this->message_handler));
	m_bv.set_verbosity(this->get_verbosity());
}

CNF_Conversion::~CNF_Conversion() {
}
	
void CNF_Conversion::set_test_goal_groups(test_goal_groups_t & tg,
		test_goal_id_map_t & id_map) {
	m_test_goal_groups.clear();
	m_test_goal_groups.swap(tg);

	m_test_goal_id_map.clear();
	m_test_goal_id_map.swap(id_map);
}

bool CNF_Conversion::decide_default() {
  if(this->options.get_option("arrays-uf")=="never")
    m_bv.unbounded_array=bv_cbmct::U_NONE;
  else if(this->options.get_option("arrays-uf")=="always")
    m_bv.unbounded_array=bv_cbmct::U_ALL;

  status("Passing problem to "+m_bv.decision_procedure_text());

  this->do_conversion(m_bv);

  return false;
}

void CNF_Conversion::convert(::goto_functionst const& gf) {
	// build the Boolean equation
	FSHELL2_PROD_CHECK1(::fshell2::FShell2_Error, !this->run(gf),
			"Failed to build Boolean program representation");
}

Compute_Test_Goals::Compute_Test_Goals(
		::language_uit & manager, ::optionst const& opts) :
	m_manager(manager),
	m_opts(opts),
	m_equation(m_manager, m_opts),
	m_test_goal_states(0),
	m_test_goals(0),
	m_alt_nesting(0)
{
}

Compute_Test_Goals::~Compute_Test_Goals() {
}

void Compute_Test_Goals::store_mapping(::literalt const& tg, ::bvt const& or_set,
		::goto_programt::const_targett const& src_context,
		::goto_programt::const_targett const& dest_context)
{
	ctx_coll_t tmp;
	tmp.push_back(::std::make_pair(src_context, dest_context));
	store_mapping(tg, or_set, tmp);
}

void Compute_Test_Goals::store_mapping(::literalt const& tg, ::bvt const& or_set,
		ctx_coll_t const& contexts)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			::literalt::unused_var_no() != tg.var_no());
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			m_tg_to_ctx_map.end() == m_tg_to_ctx_map.find(tg));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			m_and_map.end() == m_and_map.find(tg));

	for (::bvt::const_iterator iter(or_set.begin()); iter != or_set.end(); ++iter) {
		//// ::std::cerr << "OR-reverse: " << tg.dimacs() << " = OR(... " << iter->dimacs() << " ...)" << ::std::endl;
		m_reverse_or_map[*iter].insert(tg);
	}
	
	if (!tg.is_true() && !tg.is_false()) {
		ctx_coll_t & ctxs(m_tg_to_ctx_map.insert(
			::std::make_pair(tg, ctx_coll_t())).first->second);
		ctxs.reserve(ctxs.size() + contexts.size());
		ctxs.insert(ctxs.end(), contexts.begin(), contexts.end());
	}
}
	
void Compute_Test_Goals::print_test_goal(::literalt const& tg,
		::std::ostream & os) const {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			!tg.is_false());
	if (tg.is_true()) {
		os << "<TRUE>";
		return;
	}

	and_map_t::const_iterator a_iter(m_and_map.find(tg));
	if (m_and_map.end() != a_iter) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				m_tg_to_ctx_map.end() == m_tg_to_ctx_map.find(tg));
		print_test_goal(a_iter->second.first, os);
		os << "...";
		print_test_goal(a_iter->second.second, os);
	} else {
		tg_to_ctx_map_t::const_iterator entry(m_tg_to_ctx_map.find(tg));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				m_tg_to_ctx_map.end() != entry);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !entry->second.empty());
		if (entry->second.size() > 1) os << "(";
		for (ctx_coll_t::const_iterator iter(entry->second.begin());
				iter != entry->second.end(); ++iter) {
			if (entry->second.begin() != iter) os << "|";
			if (iter->first->location.is_nil()) os << "NO_LOCATION";
			else os << iter->first->location.get_file() << ":"
				<< iter->first->location.get_line();
			if (iter->second != iter->first && iter->first->is_goto() &&
					!iter->second->location.is_nil())
				os << "-" << iter->second->location.get_file() << ":"
					<< iter->second->location.get_line();
		}
		if (entry->second.size() > 1) os << ")";
	}
}

void Compute_Test_Goals::make_id_map()
{
	m_tg_id_map.clear();
	m_skips.clear();
	
	typedef ::std::vector< ::std::pair< test_goal_groups_t::const_iterator,
			test_goals_t::const_iterator > > ptrs_t;
	ptrs_t ptrs;
	ptrs.reserve(m_test_goal_groups.size());
	for (test_goal_groups_t::const_iterator g_iter(m_test_goal_groups.begin());
			g_iter != m_test_goal_groups.end(); ++g_iter) {
		if (g_iter->empty()) {
			ptrs.clear();
			break;
		}
		ptrs.push_back(::std::make_pair(g_iter, g_iter->begin()));
	}
	
	if (ptrs.empty()) return;

	while (ptrs.front().second != ptrs.front().first->end()) {
		m_tg_id_map.push_back(::bvt());
		m_tg_id_map.back().reserve(m_test_goal_groups.size());
		for (ptrs_t::const_iterator iter(ptrs.begin()); iter != ptrs.end(); ++iter)
			m_tg_id_map.back().push_back(*iter->second);
		
		ptrs_t::size_type rindex(ptrs.size());
		for (; rindex > 0; --rindex) {
			++(ptrs[rindex - 1].second);
			if (ptrs[rindex - 1].second != ptrs[rindex - 1].first->end()) break;
		}
		for (ptrs_t::iterator iter(ptrs.begin() + rindex); rindex > 0 && iter != ptrs.end(); ++iter)
			iter->second = iter->first->begin();
	}
	
	m_skips.resize(m_test_goal_groups.size(), 0);
	test_goals_t::size_type prec_skip(1);
	skips_t::reverse_iterator s_iter(m_skips.rbegin());
	// should be a const_reverse_iterator, but some buggy STL versions (OS X!)
	// lack a proper operator!=
	for (test_goal_groups_t::reverse_iterator g_iter(m_test_goal_groups.rbegin());
			g_iter != m_test_goal_groups.rend(); ++g_iter, ++s_iter) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !g_iter->empty());
		// users of skips will do +1 through for-loop increments
		*s_iter = prec_skip - 1;
		prec_skip *= g_iter->size();
	}
}

void Compute_Test_Goals::print_all_test_goals()
{
	test_goal_id_t id(0);
	for (test_goal_id_map_t::const_iterator iter(m_tg_id_map.begin());
			iter != m_tg_id_map.end(); ++iter, ++id) {
		::std::ostringstream osstg, ossdata;
		osstg << "Test goal " << id << " (" << iter->front().dimacs();
		print_test_goal(iter->front(), ossdata);
		for (::bvt::const_iterator l_iter(++(iter->begin())); l_iter != iter->end(); ++l_iter) {
			osstg << "&" << l_iter->dimacs();
			ossdata << "...";
			print_test_goal(*l_iter, ossdata);
		}
		osstg << "): " << ossdata.str();
		m_manager.print(0, osstg.str());
	}
}

void Compute_Test_Goals::visit(CP_Alternative const* n) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_cp == n);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 2 == m_test_goal_states->m_children.size());
	++m_alt_nesting;
	Evaluate_Coverage_Pattern::Test_Goal_States const* m_tgs_bak(m_test_goal_states);
	::std::list< Evaluate_Coverage_Pattern::Test_Goal_States >::const_iterator iter(m_test_goal_states->m_children.begin());
	m_test_goal_states = &(*iter);
	n->get_cp_a()->accept(this);
	m_test_goal_states = &(*(++iter));
	n->get_cp_b()->accept(this);
	m_test_goal_states = m_tgs_bak;
	--m_alt_nesting;
}

void Compute_Test_Goals::visit(CP_Concat const* n) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_cp == n);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 2 == m_test_goal_states->m_children.size());
	Evaluate_Coverage_Pattern::Test_Goal_States const* m_tgs_bak(m_test_goal_states);
	::std::list< Evaluate_Coverage_Pattern::Test_Goal_States >::const_iterator iter(m_test_goal_states->m_children.begin());
	
	m_test_goal_states = &(*iter);
	n->get_cp_a()->accept(this);
	test_goals_t backup;
	if (0 == m_alt_nesting) {
		m_test_goal_groups.push_back(test_goals_t());
		m_test_goals = &(m_test_goal_groups.back());
	} else {
		backup.swap(*m_test_goals);
	}
	m_test_goal_states = &(*(++iter));
	n->get_cp_b()->accept(this);
	m_test_goal_states = m_tgs_bak;
			
	if (0 != m_alt_nesting) {
		test_goals_t backup2;
		backup2.swap(*m_test_goals);
		for (test_goals_t::const_iterator agi(backup.begin()); agi != backup.end(); ++agi)
			for (test_goals_t::const_iterator bgi(backup2.begin());
					bgi != backup2.end(); ++bgi) {
				::literalt const sg(m_equation.get_cnf().land(*agi, *bgi));
				// if (agi->is_true()) ::std::cerr << agi->dimacs() << " is TRUE" << ::std::endl;
				// if (agi->is_false()) ::std::cerr << agi->dimacs() << " is FALSE" << ::std::endl;
				// ::std::cerr << sg.dimacs() << " == " << agi->dimacs() << " AND " << bgi->dimacs() << ::std::endl;
				if (!agi->is_true() && !agi->is_false() && !bgi->is_true() && !bgi->is_false()) {
					m_and_map.insert(::std::make_pair(sg, ::std::make_pair(*agi, *bgi)));
					//// ::std::cerr << "reverse-AND: " << sg.dimacs() << " == " << agi->dimacs() << " AND " << bgi->dimacs() << ::std::endl;
				}
				//// else
				////	::std::cerr << "Not adding " << sg.dimacs() << " == " << agi->dimacs() << " AND " << bgi->dimacs() << ::std::endl;
				m_reverse_and_map[*agi].insert(::std::make_pair(sg, *bgi));
				m_reverse_and_map[*bgi].insert(::std::make_pair(sg, *agi));
				m_test_goals->insert(sg);
			}
	}
}

void Compute_Test_Goals::visit(Depcov const* n) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_cp == n);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_children.empty());
	
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

void Compute_Test_Goals::visit(Edgecov const* n) {
	do_atom(n, false, false);
}

void Compute_Test_Goals::visit(Nodecov const* n) {
	do_atom(n, false, false);
}

void Compute_Test_Goals::visit(Pathcov const* n) {
	do_atom(n, false, false);
}

void Compute_Test_Goals::visit(Predicate const* n) {
	do_atom(n, false, true);
}

void Compute_Test_Goals::visit(Query const* n) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 0 == m_alt_nesting);
	m_test_goal_groups.push_back(test_goals_t());
	m_test_goals = &(m_test_goal_groups.back());
	n->get_cover()->accept(this);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 0 == m_alt_nesting);
	
	make_id_map();
	if (m_opts.get_bool_option("show-test-goals")) print_all_test_goals();

	m_equation.set_test_goal_groups(m_test_goal_groups, m_tg_id_map);
}
	
void Compute_Test_Goals::visit(Quote const* n) {
	do_atom(n, true, true);
}
	
Compute_Test_Goals_From_Instrumentation::Compute_Test_Goals_From_Instrumentation(
		::language_uit & manager, ::optionst const& opts) :
	Compute_Test_Goals(manager, opts),
	m_aut_insert(m_manager)
{
}

Compute_Test_Goals_From_Instrumentation::~Compute_Test_Goals_From_Instrumentation() {
}

CNF_Conversion & Compute_Test_Goals_From_Instrumentation::do_query(::goto_functionst & gf, Query const& query) {
	m_test_goal_groups.clear();
	m_test_goals = 0;
	m_and_map.clear();
	m_tg_to_ctx_map.clear();
	m_reverse_and_map.clear();
	m_reverse_or_map.clear();
	
	m_pc_to_guard.clear();
	
	// build a CFG to have forward and backward edges
	::fshell2::instrumentation::CFG cfg;
	cfg.compute_edges(gf);
	
	// do automata instrumentation
	m_test_goal_states = &(m_aut_insert.do_query(gf, cfg, query));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_cp == query.get_cover());

	// print instrumented program, if requested
	if (m_opts.get_bool_option("show-goto-functions")) {
		Smart_Printer smp(m_manager);
		gf.output(m_equation.get_ns(), smp.get_ostream());
	} else if (m_opts.get_bool_option("dump-c")) {
		Smart_Printer smp(m_manager);
		::dump_c(gf, m_equation.get_ns(), smp.get_ostream());
	}

	// convert CFA to CNF
	m_equation.convert(gf);

	// build a map from GOTO instructions to guard literals by walking the
	// equation; group them by calling points
	::goto_programt::const_targett most_recent_caller(m_equation.get_equation().SSA_steps.front().source.pc);
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				m_equation.get_equation().SSA_steps.begin() );
			iter != m_equation.get_equation().SSA_steps.end(); ++iter)
	{
		if (iter->source.pc->is_function_call()) most_recent_caller = iter->source.pc;
		//if (!iter->is_location()) continue;
		if (!iter->is_assignment() ||
			iter->assignment_type == ::symex_targett::HIDDEN ||
			iter->assignment_type == ::symex_targett::GUARD ||
			iter->assignment_type == ::symex_targett::PHI) continue;
		// ::goto_programt tmp;
		// tmp.output_instruction(m_equation.get_ns(), "", ::std::cerr, iter->source.pc);
		// iter->output(m_equation.get_ns(), ::std::cerr);
		m_pc_to_guard[ iter->source.pc ][ most_recent_caller ].insert(iter->guard_literal);
		// ::std::cerr << "Added guard " << iter->guard_literal.dimacs() << ::std::endl;
	}
	
	query.accept(this);

	return m_equation;
}
	
bool Compute_Test_Goals_From_Instrumentation::find_all_contexts(context_to_pcs_t & context_to_pcs) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !m_test_goal_states->m_tg_states.empty());
	
	bool includes_initial_state(false);
	for (ta_state_set_t::const_iterator iter(m_test_goal_states->m_tg_states.begin());
			iter != m_test_goal_states->m_tg_states.end(); ++iter) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_aut_insert.is_test_goal_state(*iter));
		// ignore states without incoming edges, unless initial
		if (m_aut_insert.get_tg_aut().initial().end() != m_aut_insert.get_tg_aut().initial().find(*iter))
			includes_initial_state = true;
		if (m_aut_insert.get_tg_aut().delta2_backwards(*iter).empty()) continue;
		Automaton_Inserter::instrumentation_points_t const& nodes(
				m_aut_insert.get_test_goal_instrumentation(*iter));
		for (Automaton_Inserter::instrumentation_points_t::const_iterator n_iter(
					nodes.begin()); n_iter != nodes.end(); ++n_iter) {
			//FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, n_iter->second->is_location());
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, n_iter->second->is_assign());
			pc_to_context_and_guards_t::const_iterator guards_entry(m_pc_to_guard.find(n_iter->second));
			if (m_pc_to_guard.end() == guards_entry) {
				::std::ostringstream warn;
				warn << "Transition to test goal state " << *iter << " in "
					<< n_iter->second->function << " is unreachable";
				m_equation.warning(warn.str());
				continue;
			}

			// collect all possible function calls that may cause transitions to test goal state
			for (context_to_guards_t::const_iterator c_iter(guards_entry->second.begin());
					c_iter != guards_entry->second.end(); ++c_iter)
				context_to_pcs[ c_iter->first ].insert(n_iter->second);
		}
	}

	return includes_initial_state;
}

void Compute_Test_Goals_From_Instrumentation::context_to_literals(::goto_programt::const_targett const& context,
		pcs_t const& pcs, ::bvt & test_goal_literals) const {
	// forall transitions to test goal states in this context
	for (pcs_t::const_iterator t_iter(pcs.begin()); t_iter != pcs.end(); ++t_iter) {
		pc_to_context_and_guards_t::const_iterator guards_entry(m_pc_to_guard.find(*t_iter));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_pc_to_guard.end() != guards_entry);
		context_to_guards_t::const_iterator inner_guards_entry(guards_entry->second.find(context));
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				guards_entry->second.end() != inner_guards_entry);

		// forall equation entries in the given context for this
		// transition
		for (::std::set< ::literalt >::const_iterator l_iter(
					inner_guards_entry->second.begin()); l_iter != inner_guards_entry->second.end();
				++l_iter) {
			// unreachable
			if (l_iter->is_false() || l_iter->var_no() == ::literalt::unused_var_no()) continue;
			// we will always take this edge, a trivial goal
			if (l_iter->is_true()) {
				test_goal_literals.clear();
				// just make sure we have a single entry in there to get at
				// least one test case
				// ::std::cerr << "Adding true guard " << l_iter->var_no() << ::std::endl;
				test_goal_literals.push_back(*l_iter);
				return;
			}
			// ::std::cerr << "Adding guard " << l_iter->var_no() << ::std::endl;
			test_goal_literals.push_back(*l_iter);
		}
	}
}

void Compute_Test_Goals_From_Instrumentation::do_atom(Coverage_Pattern_Expr const* n,
		bool epsilon_permitted, bool make_single) {
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_cp == n);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_children.empty());
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goals);

	context_to_pcs_t context_to_pcs;
	bool has_initial(find_all_contexts(context_to_pcs));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, epsilon_permitted || !has_initial);

	::bvt set;
	if (has_initial) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, make_single);
		set.push_back(::const_literal(true));
	}
	// forall calling contexts
	for (context_to_pcs_t::const_iterator c_iter(context_to_pcs.begin()); 
			c_iter != context_to_pcs.end(); ++c_iter) {
		context_to_literals(c_iter->first, c_iter->second, set);
		if (!make_single && !set.empty()) {
			// ::std::cerr << "Adding subgoal composed of " << set.size() << " guards" << ::std::endl;
			::literalt const tg(m_equation.get_cnf().lor(set));
			store_mapping(tg, set, c_iter->first, c_iter->first);
			m_test_goals->insert(tg);
			set.clear();
		}
	}
	if (!set.empty()) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, make_single);
		// ::std::cerr << "Adding subgoal composed of " << set.size() << " guards" << ::std::endl;
		::literalt const tg(m_equation.get_cnf().lor(set));
		ctx_coll_t contexts;
		contexts.reserve(context_to_pcs.size());
		for (context_to_pcs_t::const_iterator c_iter(context_to_pcs.begin()); 
				c_iter != context_to_pcs.end(); ++c_iter)
			contexts.push_back(::std::make_pair(c_iter->first, c_iter->first));
		store_mapping(tg, set, contexts);
		m_test_goals->insert(tg);
	}
}
	
bool Compute_Test_Goals_From_Instrumentation::get_satisfied_test_goals(
		::cnf_clause_list_assignmentt const& cnf, test_goal_ids_t & tgs) const
{
	return false;
}


Compute_Test_Goals_Boolean::Compute_Test_Goals_Boolean(::language_uit & manager,
		::optionst const& opts, ::fshell2::statistics::Statistics & stats) :
	Compute_Test_Goals(manager, opts),
	m_stats(stats),
	m_cp_eval(m_manager)
{
}

Compute_Test_Goals_Boolean::~Compute_Test_Goals_Boolean()
{
}

CNF_Conversion & Compute_Test_Goals_Boolean::do_query(::goto_functionst & gf, Query const& query) {
	m_test_goal_groups.clear();
	m_test_goals = 0;
	m_and_map.clear();
	m_tg_to_ctx_map.clear();
	m_reverse_and_map.clear();
	m_reverse_or_map.clear();
	
	m_loc_to_node_target_graphs_map.clear();
	m_loc_to_edge_target_graphs_map.clear();
	m_tg_to_lit_map.clear();
	m_step_to_trans.clear();
	m_node_step_to_trans.clear();
	m_node_rep_map.clear();
	
	// build a CFG to have forward and backward edges
	::fshell2::instrumentation::CFG cfg;
	cfg.compute_edges(gf);
	
	// compute trace automata for coverage and path patterns
	m_test_goal_states = &(m_cp_eval.do_query(gf, cfg, query));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_cp == query.get_cover());
	
	::goto_programt tmp;
	tmp.add_instruction(ASSERT)->make_assertion(::false_exprt());
	
	::fshell2::instrumentation::GOTO_Transformation inserter(m_manager, gf);
	inserter.insert(ID_main, ::fshell2::instrumentation::GOTO_Transformation::BEFORE, ::END_FUNCTION, tmp);

	// print instrumented program, if requested
	if (m_opts.get_bool_option("show-goto-functions")) {
		Smart_Printer smp(m_manager);
		gf.output(m_equation.get_ns(), smp.get_ostream());
	} else if (m_opts.get_bool_option("dump-c")) {
		Smart_Printer smp(m_manager);
		::dump_c(gf, m_equation.get_ns(), smp.get_ostream());
	}

	// convert CFA to CNF
	m_equation.convert(gf);
	
	unsigned cnt_vars_before(m_equation.get_cnf().no_variables());
	unsigned cnt_cl_before(m_equation.get_cnf().no_clauses());

	build(m_cp_eval.get(), true);
	build(m_cp_eval.get_pp_eval().get(query.get_passing()), false);
	
	NEW_STAT(m_stats, Counter< unsigned >, more_vars_cnt, "Additional Boolean variables");
	more_vars_cnt.inc(m_equation.get_cnf().no_variables() - cnt_vars_before);
	NEW_STAT(m_stats, Counter< unsigned >, more_cl_cnt, "Additional clauses");
	more_cl_cnt.inc(m_equation.get_cnf().no_clauses() - cnt_cl_before);
	
	cnt_vars_before = m_equation.get_cnf().no_variables();
	cnt_cl_before = m_equation.get_cnf().no_clauses();
	
	query.accept(this);

	NEW_STAT(m_stats, Counter< unsigned >, more_vars_cnt2, "Additional Boolean variables from query");
	more_vars_cnt2.inc(m_equation.get_cnf().no_variables() - cnt_vars_before);
	NEW_STAT(m_stats, Counter< unsigned >, more_cl_cnt2, "Additional clauses from query");
	more_cl_cnt2.inc(m_equation.get_cnf().no_clauses() - cnt_cl_before);
	
	return m_equation;
}

::bvt const& Compute_Test_Goals_Boolean::state_to_bvt_lookup(::bv_utilst & bv_utils,
		bv_cache_t & bv_cache, ta_state_t const& state, unsigned const width)
{
	bv_cache_t::iterator entry(bv_cache.find(state));
	
	if (bv_cache.end() == entry) {
		entry = bv_cache.insert(::std::make_pair(state, ::bvt())).first;
		::bvt tmp(bv_utils.build_constant(state, width));
		entry->second.swap(tmp);
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				entry->second.size() == width);
	}
	
	return entry->second;
}

::literalt const& Compute_Test_Goals_Boolean::make_equals(::bv_utilst & bv_utils,
		eq_cache_t & eq_cache, ::bvt const& state_vec, bv_cache_t & bv_cache,
		ta_state_t const& state, unsigned const width)
{
	eq_cache_t::iterator entry(eq_cache.find(state));

	if (eq_cache.end() == entry)
		entry = eq_cache.insert(::std::make_pair(state,
					bv_utils.equal(state_vec, state_to_bvt_lookup(bv_utils,
							bv_cache, state, width)))).first;

	return entry->second;
}

void Compute_Test_Goals_Boolean::build(trace_automaton_t const& aut, bool map_tg) {
	FSHELL2_AUDIT_TRACE("Building trace automaton simulation");
	ta_state_map_t state_map, reverse_state_map;
	ta_state_set_t compact_final;
	compact_state_numbers(aut, state_map, reverse_state_map, compact_final);

	// traverse the automaton and collect relevant target graphs; we only need a
	// subset of the target graphs and we should better know which ones
	local_target_graph_set_t local_target_graph_set;
	Evaluate_Path_Pattern const& pp_eval(m_cp_eval.get_pp_eval());
	for (trace_automaton_t::const_iterator iter(aut.begin());
			iter != aut.end(); ++iter) {
		trace_automaton_t::edges_type const out_edges(aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator e_iter(out_edges.begin());
				e_iter != out_edges.end(); ++e_iter) {
			target_graph_t const& tgg(pp_eval.lookup_index(e_iter->first));
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, tgg.get_E().empty() ||
					tgg.get_disconnected_nodes().empty());
			if (local_target_graph_set.insert(e_iter->first).second) {
				// store the mapping from edges to target graphs
				for (target_graph_t::edges_t::const_iterator tg_iter(tgg.get_E().begin());
						tg_iter != tgg.get_E().end(); ++tg_iter)
				{
					m_loc_to_edge_target_graphs_map[ tg_iter->first.second ][ *tg_iter ].insert(e_iter->first);
					FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
						"Edge ", tg_iter->first.second->location_number, "->",
						tg_iter->second.second->location_number,
						" maps to ", e_iter->first));
				}
				// store the mapping from nodes to target graphs
				for (target_graph_t::nodes_t::const_iterator tg_iter(tgg.get_disconnected_nodes().begin());
						tg_iter != tgg.get_disconnected_nodes().end(); ++tg_iter)
					m_loc_to_node_target_graphs_map[ tg_iter->second ][ *tg_iter ].insert(e_iter->first);
			}
		}
	}
	
	transition_map_t transitions;
	for (trace_automaton_t::const_iterator iter(aut.begin());
			iter != aut.end(); ++iter) {
		ta_state_t const mapped_state(state_map.find(*iter)->second);
		trace_automaton_t::edges_type const out_edges(aut.delta2(*iter));
		for (trace_automaton_t::edges_type::const_iterator e_iter(out_edges.begin());
				e_iter != out_edges.end(); ++e_iter)
			transitions[ e_iter->first ][ mapped_state ].insert(
					state_map.find(e_iter->second)->second);
	}
	
	unsigned width(1);
	ta_state_map_t::size_type num_states(state_map.size());
	while (num_states >>= 1) ++width;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, ::std::numeric_limits< ta_state_t >::min() >= 0);
	//// ::std::cerr << "Aut size: " << state_map.size() << ::std::endl;
	//// ::std::cerr << "Max unmapped state id: " << state_map.rbegin()->first << ::std::endl;
	//// ::std::cerr << "State vec size: " << width << ::std::endl;
	
	::cnf_clause_list_assignmentt & cnf(m_equation.get_cnf());
	::bv_utilst bv_utils(cnf);
	bv_cache_t bv_cache;
	eq_cache_t prev_eq_cache, next_eq_cache;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == aut.initial().size());
	::bvt prev_state(state_to_bvt_lookup(bv_utils, bv_cache,
				state_map.find(*(aut.initial().begin()))->second, width));

	// increase efficiency for large automata, as created by PATHS(...)
	ta_state_set_t possibly_reached_states;
	possibly_reached_states.insert(state_map.find(*(aut.initial().begin()))->second);
	
	::symex_target_equationt::SSA_stepst::const_iterator previous_pc(
					m_equation.get_equation().SSA_steps.end());
	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				m_equation.get_equation().SSA_steps.begin() );
			iter != m_equation.get_equation().SSA_steps.end(); ++iter)
	{
		//// ::std::cerr << "--------------------------------------------------------------------------------" << ::std::endl;
		//// ::goto_programt tmp;
		//// tmp.output_instruction(m_equation.get_ns(), "", ::std::cerr, iter->source.pc);
		//// iter->output(m_equation.get_ns(), ::std::cerr);
		//// ::std::cerr << "Guard literal: " << iter->guard_literal.dimacs() << ::std::endl;

		// cannot reach final assert(0) from this node
		if (iter->guard_literal.var_no() == ::literalt::unused_var_no()) continue;
		
		if (Evaluate_Filter::ignore_function(*(iter->source.pc))) continue;
		if (Evaluate_Filter::ignore_instruction(*(iter->source.pc))) continue;

		// ignore hidden assignments
		if (iter->is_assignment() &&
			(iter->assignment_type == ::symex_targett::HIDDEN ||
			 iter->assignment_type == ::symex_targett::GUARD ||
			 iter->assignment_type == ::symex_targett::PHI)) continue;
		// some instructions produce multiple steps; ignore all but the first;
		// examples of such are:
		// function calls (assignment of parameters)
		// returns (assignment of return values)
		// goto + assume/assert produced by unwinding loops
		if ((m_equation.get_equation().SSA_steps.end() == previous_pc) ||
				(iter->source.pc != previous_pc->source.pc))
			previous_pc = iter;
		else if ((iter->is_assignment() && previous_pc->is_location()) ||
				 (iter->is_function_call() && previous_pc->is_location()) ||
				((iter->is_assume() || iter->is_assert()) && iter->source.pc->is_goto() && previous_pc->is_location()))
				continue;

#if FSHELL2_DEBUG__LEVEL__ >= 2
		::std::ostringstream oss;
		oss << "--------------------------------------------------------------------------------" << ::std::endl;
		::goto_programt tmp;
		tmp.output_instruction(m_equation.get_ns(), "", oss, iter->source.pc);
		iter->output(m_equation.get_ns(), oss);
		oss << "Guard literal: " << iter->guard_literal.dimacs();
		FSHELL2_AUDIT_TRACE(oss.str());
#endif
		do_step(cnf, bv_utils, bv_cache, prev_eq_cache, next_eq_cache,
				prev_state, possibly_reached_states, width, iter,
				local_target_graph_set, map_tg, transitions, reverse_state_map);
	}

	::bvt finals;
	for (ta_state_set_t::const_iterator iter(compact_final.begin());
			iter != compact_final.end(); ++iter) {
		finals.push_back(make_equals(bv_utils, prev_eq_cache, prev_state,
					bv_cache, *iter, width));
		//// ::std::cerr << "Added final state " << *iter << " dimacs: " << finals.back().dimacs() << ::std::endl;
	}
	cnf.lcnf(::bvt(1, cnf.lor(finals)));
}

void Compute_Test_Goals_Boolean::do_step(::cnf_clause_list_assignmentt & cnf,
		::bv_utilst & bv_utils, bv_cache_t & bv_cache, eq_cache_t & prev_eq_cache,
		eq_cache_t & next_eq_cache, ::bvt & prev_state,
		ta_state_set_t & possibly_reached_states, unsigned const width,
		::symex_target_equationt::SSA_stepst::const_iterator const step,
		local_target_graph_set_t const& local_target_graph_set, bool map_tg,
		transition_map_t const& transitions, ta_state_map_t const& reverse_state_map)
{
	using ::fshell2::instrumentation::GOTO_Transformation;

	::goto_programt::const_targett const pc(step->source.pc);

	// determine the guard literals for goto-edges
	::literalt goto_guard(::const_literal(false));
	if (pc->is_goto() && step->is_location()) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == pc->targets.size());
		if (!pc->guard.is_true()) {
			::symex_target_equationt::SSA_stepst::const_iterator guard_finder(step);
			++guard_finder;
			if (guard_finder->is_assignment() && guard_finder->assignment_type == ::symex_targett::GUARD)
				++guard_finder;
			goto_guard = guard_finder->guard_literal;
		}
		goto_guard.invert();
	}

	bool const is_instrumented(GOTO_Transformation::is_instrumented(pc));

	loc_to_node_target_graphs_t::const_iterator l_n_t_g_entry(
			m_loc_to_node_target_graphs_map.find(pc));
	if (m_loc_to_node_target_graphs_map.end() != l_n_t_g_entry) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !is_instrumented);
	
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == l_n_t_g_entry->second.size());
		// this node appears in any of the two automata
		node_to_target_graphs_t const& entry(l_n_t_g_entry->second);
	
		// we could do a bunch of transitions, count their number
		ta_state_set_t sources;
		// find the target graphs it could belong to
		::std::set< int > target_graphs;
		::std::set_intersection(entry.begin()->second.begin(), entry.begin()->second.end(),
				local_target_graph_set.begin(), local_target_graph_set.end(),
				::std::inserter(target_graphs, target_graphs.begin()));
		if (!target_graphs.empty()) {
			for (::std::set< int >::const_iterator tgg_iter(target_graphs.begin());
					tgg_iter != target_graphs.end(); ++tgg_iter) {
				transition_map_t::const_iterator t_entry(transitions.find(*tgg_iter));
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, transitions.end() != t_entry);

				for (symbol_transition_map_t::const_iterator t_iter(t_entry->second.begin());
						t_iter != t_entry->second.end(); ++t_iter)
					sources.insert(t_iter->first);
			}

			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !sources.empty());
			for (ta_state_set_t::const_iterator cnt(sources.begin()); cnt != sources.end(); ++cnt) {
				// note that *cnt is not useful, we could be in any of the states

				// prepare next state
				::bvt next_state;
				next_state.reserve(width);
				for (::bvt::size_type i(0); i != width; ++i)
					next_state.push_back(cnf.new_variable());

				::bvt trans;
				for (::std::set< int >::const_iterator tgg_iter(target_graphs.begin());
						tgg_iter != target_graphs.end(); ++tgg_iter) {
					transition_map_t::const_iterator t_entry(transitions.find(*tgg_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, transitions.end() != t_entry);

					for (symbol_transition_map_t::const_iterator t_iter(t_entry->second.begin());
							t_iter != t_entry->second.end(); ++t_iter) {
						if (possibly_reached_states.end() == possibly_reached_states.find(t_iter->first)) continue;
						// (state-vec == source-state) -> new-state-vec == (one of dest states)
						::bvt dests;
						for (ta_state_set_t::const_iterator s_iter(t_iter->second.begin());
								s_iter != t_iter->second.end(); ++s_iter) {
							dests.push_back(make_equals(bv_utils, next_eq_cache,
										next_state, bv_cache, *s_iter, width));
							possibly_reached_states.insert(*s_iter);
							// std::cerr << "Adding trans " << t_iter->first << "-[" <<
							//   *tgg_iter << "]>" << *s_iter << " next==" << *s_iter << ": " << dests.back().dimacs() << ::std::endl;
							if (map_tg) {
								FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 
										reverse_state_map.end() != reverse_state_map.find(*s_iter));
								ta_state_t const unmapped_state(
										reverse_state_map.find(*s_iter)->second);
								::literalt land;
								FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
										::literalt::unused_var_no() == land.var_no());
								if (m_cp_eval.is_test_goal_state(unmapped_state)) {
									land = cnf.land(step->guard_literal, dests.back());
									FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
										"State ", unmapped_state, " maps to ", land.dimacs()));
									m_tg_to_lit_map[ unmapped_state ][ ::std::make_pair(entry.begin()->first, entry.begin()->first) ].insert(land);
								}
								m_node_step_to_trans.insert(::std::make_pair(step, Edge_Goal(step->guard_literal, land,
												reverse_state_map.find(t_iter->first)->second, unmapped_state)));
								m_node_rep_map.insert(::std::make_pair(step, sources.size()));
							}
						}
						::literalt const req_src(make_equals(bv_utils,
									prev_eq_cache, prev_state, bv_cache,
									t_iter->first, width));
						trans.push_back(cnf.land(req_src, cnf.lor(dests)));
						//// ::std::cerr << req_src.dimacs() << " AND dests" << ::std::endl;
					}
				}

				// we can choose to do one of the edge transitions instead or
				// must make them equal if node is not seen
				cnf.lcnf(::bvt(1, cnf.lor(bv_utils.equal(prev_state, next_state), cnf.land(step->guard_literal, cnf.lor(trans)))));

				prev_state.swap(next_state);
				prev_eq_cache.swap(next_eq_cache);
				next_eq_cache.clear();
			}
		}
	}

	loc_to_edge_target_graphs_t::const_iterator l_e_t_g_entry(
			m_loc_to_edge_target_graphs_map.find(pc));
	if (m_loc_to_edge_target_graphs_map.end() == l_e_t_g_entry) {
		// statement may have been added by earlier instrumentation and we
		// don't care about it anyway; otherwise we must not take this edge
		if (!is_instrumented)
			cnf.lcnf(::bvt(1, cnf.lnot(step->guard_literal)));

		return;
	} else {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !is_instrumented
				|| pc->is_location());
		// some of the outgoing edges of the current node appear in any of
		// the two automata
		edge_to_target_graphs_t const& entry(l_e_t_g_entry->second);
		// we cannot have more than two outgoing edges
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				entry.size() == 1 || entry.size() == 2);
		// if more than one, it must be a goto (and hence cannot be the
		// result of instrumentation)
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				entry.size() == 1 || (pc->is_goto() && !is_instrumented));

		// prepare next state
		::bvt next_state;
		next_state.reserve(width);
		for (::bvt::size_type i(0); i != width; ++i)
			next_state.push_back(cnf.new_variable());
		::bvt edge_guards;
		::literalt eq_literal(::const_literal(false));

		// determine which of the edges is the goto-edge, if applicable
		edge_to_target_graphs_t::const_iterator goto_edge(entry.end());
		if (pc->is_goto()) {
			goto_edge = entry.begin();
			bool const ige(GOTO_Transformation::is_goto_edge(goto_edge->first));
			if (!ige) ++goto_edge;
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, ige ||
					entry.end() == goto_edge ||
					GOTO_Transformation::is_goto_edge(goto_edge->first));
		}

		// for each of the possible edges see which target graphs it could
		// belong to
		for (edge_to_target_graphs_t::const_iterator e_iter(entry.begin());
				e_iter != entry.end(); ++e_iter) {
			::std::set< int > target_graphs;
			::std::set_intersection(e_iter->second.begin(), e_iter->second.end(),
					local_target_graph_set.begin(), local_target_graph_set.end(),
					::std::inserter(target_graphs, target_graphs.begin()));

			::literalt const al(cnf.land(step->guard_literal,
						(goto_edge == e_iter ? goto_guard : goto_guard.negation())));
			edge_guards.push_back(al);

			// collect all possible transitions
			::bvt trans;
			for (::std::set< int >::const_iterator tgg_iter(target_graphs.begin());
					tgg_iter != target_graphs.end(); ++tgg_iter) {
				transition_map_t::const_iterator t_entry(transitions.find(*tgg_iter));
				FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, transitions.end() != t_entry);

				for (symbol_transition_map_t::const_iterator t_iter(t_entry->second.begin());
						t_iter != t_entry->second.end(); ++t_iter) {
					if (possibly_reached_states.end() == possibly_reached_states.find(t_iter->first)) continue;
					// (state-vec == source-state) -> new-state-vec == (one of dest states)
					::bvt dests;
					for (ta_state_set_t::const_iterator s_iter(t_iter->second.begin());
							s_iter != t_iter->second.end(); ++s_iter) {
						dests.push_back(make_equals(bv_utils, next_eq_cache,
									next_state, bv_cache, *s_iter, width));
						possibly_reached_states.insert(*s_iter);
#if FSHELL2_DEBUG__LEVEL__ >= 2
						{
							std::ostringstream oss;
							oss << "Adding trans " << t_iter->first << "-["
							<< *tgg_iter << "]>" << *s_iter << " next==" << *s_iter
							<< ": " << dests.back().dimacs();
							FSHELL2_AUDIT_TRACE(oss.str());
						}
#endif
						if (map_tg) {
							FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 
									reverse_state_map.end() != reverse_state_map.find(*s_iter));
							ta_state_t const unmapped_state(
									reverse_state_map.find(*s_iter)->second);
							::literalt land;
							FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
									::literalt::unused_var_no() == land.var_no());
							if (m_cp_eval.is_test_goal_state(unmapped_state)) {
								land = cnf.land(al, dests.back());
								FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
									"State2 ", unmapped_state, " maps to ", land.dimacs()));
								m_tg_to_lit_map[ unmapped_state ][ e_iter->first ].insert(land);
							}
							//// ::std::cerr << "================================================================================" << ::std::endl;
							//// ::std::cerr << "Storing:" << ::std::endl;
							//// ::goto_programt tmp;
							//// tmp.output_instruction(m_equation.get_ns(), "", ::std::cerr, step->source.pc);
							//// step->output(m_equation.get_ns(), ::std::cerr);
							//// ::std::cerr << "with guard " << al.dimacs() << " and trans " << reverse_state_map.find(t_iter->first)->second
							//// 	<< " -> " << unmapped_state << ::std::endl;
							m_step_to_trans.insert(::std::make_pair(step, Edge_Goal(al, land,
												reverse_state_map.find(t_iter->first)->second, unmapped_state)));
						}
					}
					::literalt const req_src(make_equals(bv_utils,
								prev_eq_cache, prev_state, bv_cache,
								t_iter->first, width));
					trans.push_back(cnf.land(req_src, cnf.lor(dests)));
					//// ::std::cerr << req_src.dimacs() << " AND dests" << ::std::endl;
				}
			}
			// we can choose to skip this predicate
			if (is_instrumented) {
				eq_literal = bv_utils.equal(prev_state, next_state);
				trans.push_back(eq_literal);
			}
			// if there is no matching target graph and the node was not
			// added by instrumentation, trans will be empty; lor(trans) is
			// false
			cnf.lcnf(::bvt(1, cnf.limplies(al, cnf.lor(trans))));
			//// ::std::cerr << "CNF: " << al.dimacs() << " -> " << lor2.dimacs() << " (one-of-trans)" << ::std::endl;
		}
		::literalt const edge(cnf.lor(edge_guards));
		if (!edge.is_true() && eq_literal.is_false())
			eq_literal = bv_utils.equal(prev_state, next_state);
		cnf.lcnf(::bvt(1, cnf.lor(edge, eq_literal)));
		//// ::std::cerr << "CNF: " << not_edge.dimacs() << " -> " << eq_literal.dimacs() << " (state-vec-eq)" << ::std::endl;

		prev_state.swap(next_state);
		prev_eq_cache.swap(next_eq_cache);
		next_eq_cache.clear();
	}
}

void Compute_Test_Goals_Boolean::do_atom(Coverage_Pattern_Expr const* n,
		bool epsilon_permitted, bool make_single)
{
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_cp == n);
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goal_states->m_children.empty());
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !m_test_goal_states->m_tg_states.empty());
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_test_goals);

	typedef ::std::vector< state_edge_lit_map_t::const_iterator > selected_t;
	selected_t selected;
	selected.reserve(m_test_goal_states->m_tg_states.size());

	bool has_initial(false);
	for (ta_state_set_t::const_iterator iter(m_test_goal_states->m_tg_states.begin());
			iter != m_test_goal_states->m_tg_states.end(); ++iter) {
		// ignore states without incoming edges, unless initial
		if (m_cp_eval.get().initial().end() != m_cp_eval.get().initial().find(*iter))
			has_initial = true;
		// some states might be impossible to reach
		state_edge_lit_map_t::const_iterator entry(m_tg_to_lit_map.find(*iter));
		if (m_tg_to_lit_map.end() != entry) {
			FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
				"Selecting state ", *iter));
			selected.push_back(entry);
		}
	}
	FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
		"has_initial=", has_initial, ", selected.size()=", selected.size()));

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, epsilon_permitted || !has_initial);

	::bvt set;
	if (has_initial) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, make_single);
		set.push_back(::const_literal(true));
	}
	for (selected_t::const_iterator iter(selected.begin()); iter != selected.end(); ++iter) {
		for (edge_to_literal_map_t::const_iterator c_iter((*iter)->second.begin());
				c_iter != (*iter)->second.end(); ++c_iter) {
			::std::copy(c_iter->second.begin(), c_iter->second.end(), ::std::back_inserter(set));
			if (!make_single && !set.empty()) {
				FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
					"Adding1 subgoal composed of ", set.size(), " guards"));
				::literalt const tg(m_equation.get_cnf().lor(set));
				store_mapping(tg, set, c_iter->first.first.second,
						c_iter->first.second.second);
				m_test_goals->insert(tg);
				set.clear();
			}
		}
	}
	if (!set.empty()) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, make_single);
		FSHELL2_AUDIT_TRACE(::diagnostics::internal::to_string(
			"Adding2 subgoal composed of ", set.size(), " guards"));
		::literalt const tg(m_equation.get_cnf().lor(set));
		ctx_coll_t contexts;
		for (selected_t::const_iterator iter(selected.begin()); iter != selected.end(); ++iter)
			for (edge_to_literal_map_t::const_iterator c_iter((*iter)->second.begin()); 
					c_iter != (*iter)->second.end(); ++c_iter)
				contexts.push_back(::std::make_pair(c_iter->first.first.second,
							c_iter->first.second.second));
		store_mapping(tg, set, contexts);
		m_test_goals->insert(tg);
	}
}

class KnownSAT {
	public:
               typedef ::std::set< ::literalt > init_t;
		KnownSAT(init_t const& base) :
			m_min_var_no(0), m_max_var_no(0)
		{
			// try to benefit from base being sorted -> resize only once when
			// called from here
			for(init_t::const_iterator iter(base.begin()); iter != base.end(); ++iter)
				if (insert(*iter)) break;
			for(init_t::const_reverse_iterator riter(base.rbegin()); riter != base.rend(); ++riter)
				insert(*riter);
		}

		bool insert(::literalt const& lit) {
			if (lit.is_constant()) return false;
			
			unsigned const var_no(lit.var_no());
			int const mask(lit.sign() ? 1 : 2);
			bool retval(true);
			if (m_lits.empty()) {
				m_lits.push_back(mask);
				m_min_var_no = m_max_var_no = var_no;
			} else if (var_no > m_max_var_no) {
                               m_lits.insert(m_lits.end(), var_no - m_max_var_no, 0);
				m_max_var_no = var_no;
				m_lits.back() = mask;
			} else if (var_no < m_min_var_no) {
                               m_lits.insert(m_lits.begin(), m_min_var_no - var_no, 0);
				m_min_var_no = var_no;
				m_lits.front() = mask;
			} else {
				retval = (0 == (m_lits[ var_no - m_min_var_no ] & mask));
				m_lits[ var_no - m_min_var_no ] |= mask;
			}

			return retval;
		}
			
		void add_to_list_unique(::std::list< ::literalt > & new_sat) {
			unsigned offset(0);
			for (::std::vector< char >::const_iterator iter(m_lits.begin());
					iter != m_lits.end(); ++iter, ++offset) {
				if (*iter & 1) new_sat.push_back(::literalt(m_min_var_no + offset, true));
				if (*iter & 2) new_sat.push_back(::literalt(m_min_var_no + offset, false));
			}
		}

		bool has(::literalt const& lit) const {
                       FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !lit.is_true() || lit.sign());
                       FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !lit.is_false() || !lit.sign());
                       if (lit.is_constant()) return lit.sign();

			unsigned const var_no(lit.var_no());
                       return (var_no >= m_min_var_no && var_no <= m_max_var_no &&
                                       (m_lits[ var_no - m_min_var_no ] & (lit.sign() ? 1 : 2)));
		}

	private:
		::std::vector< char > m_lits;
		unsigned m_min_var_no;
		unsigned m_max_var_no;

		KnownSAT(KnownSAT const& other);
		KnownSAT & operator=(KnownSAT const& other);
};

bool Compute_Test_Goals_Boolean::get_satisfied_test_goals(
		::cnf_clause_list_assignmentt const& cnf, test_goal_ids_t & tgs) const
{
	// simulate cp-automaton according to edge guards
	// start from initial state and check possible successors - but do not
	// re-use edge guards
	// once an accepting state is reached we must determine which test goal we
	// have satisfied

	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, 1 == m_cp_eval.get().initial().size());
	typedef ::std::map< ta_state_t, ::std::list< ::std::set< ::literalt > > > state_vec_t;
	state_vec_t current_states;
	current_states[ *m_cp_eval.get().initial().begin() ].push_back(::std::set< ::literalt >());
	unsigned current_states_size(0);

	for (::symex_target_equationt::SSA_stepst::const_iterator iter( 
				m_equation.get_equation().SSA_steps.begin() );
			iter != m_equation.get_equation().SSA_steps.end(); ++iter)
	{
		if (!iter->guard_literal.is_true() && cnf.l_get(iter->guard_literal) != ::tvt(true)) continue;
		//// ::std::cerr << "--------------------------------------------------------------------------------" << ::std::endl;
		//// ::goto_programt tmp;
		//// tmp.output_instruction(m_equation.get_ns(), "", ::std::cerr, iter->source.pc);
		//// iter->output(m_equation.get_ns(), ::std::cerr);
		//// ::std::cerr << "Guard literal: " << iter->guard_literal.dimacs() << ::std::endl;
		// lookup possible transitions in this SSA step and their edge guards
		node_rep_map_t::const_iterator rep_entry(m_node_rep_map.find(iter));
		for (unsigned cnt(m_node_rep_map.end() == rep_entry ? 1 : rep_entry->second + 1); cnt > 0; --cnt) {
			// this is slower than the code below, penalty of more than 90-140
			// seconds on picosat BB^2 - for whatever reason
			// ::std::cerr << (1 == cnt ? "Doing edges" : "Doing one round of nodes") << ::std::endl;
			// ::std::pair< step_to_trans_mmap_t::const_iterator, step_to_trans_mmap_t::const_iterator >
			// 	iter_pair(1 == cnt ? m_step_to_trans.equal_range(iter) :
			// 			m_node_step_to_trans.equal_range(iter));
			::std::pair< step_to_trans_mmap_t::const_iterator, step_to_trans_mmap_t::const_iterator >
				iter_pair;
			if (1 == cnt) {
				// ::std::cerr << "Doing edges" << ::std::endl;
				iter_pair = m_step_to_trans.equal_range(iter);
			} else {
				// ::std::cerr << "Doing one round of nodes" << ::std::endl;
				iter_pair = m_node_step_to_trans.equal_range(iter);
			}
			// is there an entry for this step?
			//// ::std::cerr << "Looked up step" << ::std::endl;
			if (iter_pair.first == iter_pair.second) continue;

			current_states_size = 0;
			state_vec_t next_states;
			// we can always choose to skip predicates or (some of) NODES
			if (1 != cnt || ::fshell2::instrumentation::GOTO_Transformation::is_instrumented(iter->source.pc))
				next_states.insert(current_states.begin(), current_states.end());
			for (;iter_pair.first != iter_pair.second; ++(iter_pair.first)) {
				::literalt const& edge_guard(iter_pair.first->second.m_edge_guard);
				//// ::std::cerr << "Checking " << edge_guard.dimacs() << ::std::endl;
				if (!edge_guard.is_true() && cnf.l_get(edge_guard) != ::tvt(true)) continue;
				ta_state_t const& src(iter_pair.first->second.m_src);
				state_vec_t::const_iterator entry(current_states.find(src));
				if (current_states.end() == entry) continue;
				ta_state_t const& dest(iter_pair.first->second.m_dest);
				bool const is_tgs(m_cp_eval.is_test_goal_state(dest));
				reverse_or_lit_map_t::const_iterator or_entry(m_reverse_or_map.end());
				if (is_tgs) {
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
							::literalt::unused_var_no() != iter_pair.first->second.m_tg.var_no());
					//// ::std::cerr << "Looking up OR " << i_iter->dimacs() << ::std::endl;
					or_entry = m_reverse_or_map.find(iter_pair.first->second.m_tg);
				}
				// ::std::cerr << "trans " << src << " -> " << dest << ::std::endl;
				for (::std::list< ::std::set< ::literalt > >::const_iterator i_iter(entry->second.begin());
						i_iter != entry->second.end(); ++i_iter) {
					// moving this initialization out of the loop causes an
					// overall penalty of 40-100 seconds on picosat BB^2
					state_vec_t::iterator next_entry(next_states.insert(::std::make_pair(dest,
									::std::list< ::std::set< ::literalt > >())).first);
					next_entry->second.push_back(*i_iter);
					::std::set< ::literalt > & known_sat(next_entry->second.back());
					++current_states_size;
					if (is_tgs) {
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, m_reverse_or_map.end() != or_entry);
						for (::std::set< ::literalt >::const_iterator or_entries(or_entry->second.begin());
								or_entries != or_entry->second.end(); ++or_entries) {
							//// ::std::cerr << "SATISFIED OR: " << or_entries->dimacs() << ::std::endl;
							if (!or_entries->is_true()) known_sat.insert(*or_entries);
						}
					}
				}
			}

			//// ::std::cerr << "Next round to come" << ::std::endl;
			current_states.swap(next_states);
		}
	}

	::std::ostringstream status;
	status << "Need to analyze " << current_states_size << " candidates for coverage";
	m_manager.status(status.str());
	test_goal_id_map_t const& id_map(m_equation.get_test_goal_id_map());
	for (state_vec_t::const_iterator iter(current_states.begin());
			iter != current_states.end(); ++iter) {
		if (!m_cp_eval.get().final(iter->first)) continue;

		for (::std::list< ::std::set< ::literalt > >::const_iterator i_iter(iter->second.begin());
				i_iter != iter->second.end(); ++i_iter) {

			::std::map< ::literalt, ::std::list< ::literalt > > and_missing_one;
			KnownSAT known_sat(*i_iter);
			::std::list< ::literalt > new_sat;
			// known_sat.add_to_list_unique(new_sat);
			// it is a set, we can safely copy from it directly
			new_sat.insert(new_sat.end(), i_iter->begin(), i_iter->end());
			while (!new_sat.empty()) {
				::literalt const s(new_sat.front());
				new_sat.pop_front();

				reverse_and_lit_map_t::const_iterator and_entry(m_reverse_and_map.find(s));
				if (m_reverse_and_map.end() != and_entry)
					for (::std::set< ::std::pair< ::literalt, ::literalt > >::const_iterator
							and_iter(and_entry->second.begin());
							and_iter != and_entry->second.end(); ++and_iter) {
						// s AND true is s
						if (and_iter->second.is_true()) continue;
						// already done
						if (known_sat.has(and_iter->first)) continue;

						::literalt const& other(and_iter->second);
						if (known_sat.has(other)) {
							if (known_sat.insert(and_iter->first))
								new_sat.push_back(and_iter->first);
						} else
							and_missing_one[ other ].push_back(and_iter->first);
					}

				::std::map< ::literalt, ::std::list< ::literalt > >::iterator entry(
						and_missing_one.find(s));
				if (and_missing_one.end() != entry) {
					for (::std::list< ::literalt >::const_iterator n_iter(entry->second.begin());
							n_iter != entry->second.end(); ++n_iter)
						if (known_sat.insert(*n_iter))
							new_sat.push_back(*n_iter);
					and_missing_one.erase(entry);
				}
			}

			test_goal_id_t id(0);
			for (test_goal_id_map_t::const_iterator tg_iter(id_map.begin());
					tg_iter != id_map.end(); ++tg_iter, ++id) {
				bool found(true);
				skips_t::const_iterator s_iter(m_skips.begin());
				for (::bvt::const_iterator v_iter(tg_iter->begin());
						found && v_iter != tg_iter->end(); ++v_iter, ++s_iter)
					found = known_sat.has(*v_iter);
				if (found) {
					tgs.insert(id);
					break;
				} else {
					--s_iter;
					tg_iter += *s_iter;
					id += *s_iter;
				}
			}
		}
	}

	return true;
}
	
FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

