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

/*! \file fshell2/fql/evaluation/evaluate_filter.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr 30 13:31:27 CEST 2009 
*/

#include <fshell2/fql/evaluation/evaluate_filter.hpp>
#include <fshell2/config/annotations.hpp>
#include <fshell2/exception/query_processing_error.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <diagnostics/basic_exceptions/not_implemented.hpp>

#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_complement.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_enclosing_scopes.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pm_alternative.hpp>
#include <fshell2/fql/ast/pm_concat.hpp>
#include <fshell2/fql/ast/pm_filter_adapter.hpp>
#include <fshell2/fql/ast/pm_next.hpp>
#include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>

#include <algorithm>
#include <iterator>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Evaluate_Filter::Evaluate_Filter(::goto_functionst const& ts) :
	m_ts(ts) {
	m_cfg(m_ts);
}

Evaluate_Filter::~Evaluate_Filter() {
}

Evaluate_Filter::value_t const& Evaluate_Filter::get(Filter const& f) const {
	filter_value_t::const_iterator entry(m_filter_map.find(&f));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
			entry != m_filter_map.end());
	return entry->second;
}

void Evaluate_Filter::visit(Edgecov const* n) {
	n->get_filter()->accept(this);
}

void Evaluate_Filter::visit(Filter_Complement const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter()->accept(this);
	Filter_Function * id(Filter_Function::Factory::get_instance().create<F_IDENTITY>());
	id->accept(this);

	filter_value_t::const_iterator id_set(m_filter_map.find(id));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, id_set != m_filter_map.end());
	filter_value_t::const_iterator f_set(m_filter_map.find(n->get_filter()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, f_set != m_filter_map.end());
	::std::set_difference(id_set->second.begin(), id_set->second.end(),
			f_set->second.begin(), f_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Evaluate_Filter::visit(Filter_Compose const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter_a()->accept(this);
	n->get_filter_b()->accept(this);

	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Filter::visit(Filter_Enclosing_Scopes const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter()->accept(this);

	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

void Evaluate_Filter::visit(Filter_Function const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	switch (n->get_filter_type()) {
		case F_IDENTITY:
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_FILE:
			{
				::std::string const& arg(n->get_string_arg<F_FILE>());
				for (::goto_functionst::function_mapt::const_iterator iter(m_ts.function_map.begin());
						iter != m_ts.function_map.end(); ++iter) {
					if (!iter->second.body_available) continue;
					for (::goto_programt::instructionst::const_iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (f_iter->location.is_nil() || f_iter->location.get_file() != arg) continue;
						::cfgt< empty_t >::entriest::const_iterator cfg_node(m_cfg.entries.find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg.entries.end());
						for (::goto_programt::const_targetst::const_iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter)
							entry.first->second.insert(::std::make_pair(f_iter, *s_iter));
					}
				}
			}
			break;
		case F_LINE:
			n->get_int_arg<F_LINE>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_COLUMN:
			n->get_int_arg<F_COLUMN>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_FUNC:
			{
				::goto_functionst::function_mapt::const_iterator fct(m_ts.function_map.find(
							n->get_string_arg<F_FUNC>()));
				FSHELL2_PROD_CHECK1(::fshell2::Query_Processing_Error, fct != m_ts.function_map.end() &&
						fct->second.body_available, ::diagnostics::internal::to_string("Cannot evaluate ",
							*n, " (function not available)"));
				for (::goto_programt::instructionst::const_iterator f_iter(fct->second.body.instructions.begin());
						f_iter != fct->second.body.instructions.end(); ++f_iter) {
					// don't cover edges leaving the function
					if (f_iter->is_function_call() || f_iter->is_return()) continue;
					::cfgt< empty_t >::entriest::const_iterator cfg_node(m_cfg.entries.find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg.entries.end());
					for (::goto_programt::const_targetst::const_iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter)
						entry.first->second.insert(::std::make_pair(f_iter, *s_iter));
				}
			}
			break;
		case F_LABEL:
			break;
			{
				::std::string const& arg(n->get_string_arg<F_LABEL>());
				for (::goto_functionst::function_mapt::const_iterator iter(m_ts.function_map.begin());
						iter != m_ts.function_map.end(); ++iter) {
					if (!iter->second.body_available) continue;
					for (::goto_programt::instructionst::const_iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (::std::find(f_iter->labels.begin(), f_iter->labels.end(), arg) == f_iter->labels.end()) continue;
						::cfgt< empty_t >::entriest::const_iterator cfg_node(m_cfg.entries.find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg.entries.end());
						for (::goto_programt::const_targetst::const_iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter)
							entry.first->second.insert(::std::make_pair(f_iter, *s_iter));
					}
				}
			}
			break;
		case F_CALL:
			n->get_string_arg<F_CALL>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_CALLS:
			for (::goto_functionst::function_mapt::const_iterator iter(m_ts.function_map.begin());
					iter != m_ts.function_map.end(); ++iter) {
				if (!iter->second.body_available) continue;
				for (::goto_programt::instructionst::const_iterator f_iter(iter->second.body.instructions.begin());
						f_iter != iter->second.body.instructions.end(); ++f_iter) {
					if (!f_iter->is_function_call()) continue;
					::cfgt< empty_t >::entriest::const_iterator cfg_node(m_cfg.entries.find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg.entries.end());
					for (::goto_programt::const_targetst::const_iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter)
						entry.first->second.insert(::std::make_pair(f_iter, *s_iter));
				}
			}
			break;
		case F_ENTRY:
			{
				::goto_functionst::function_mapt::const_iterator fct(m_ts.function_map.find(
							n->get_string_arg<F_ENTRY>()));
				FSHELL2_PROD_CHECK1(::fshell2::Query_Processing_Error, fct != m_ts.function_map.end() &&
						fct->second.body_available, ::diagnostics::internal::to_string("Cannot evaluate ",
							*n, " (function not available)"));
				for (::goto_programt::instructionst::const_iterator f_iter(fct->second.body.instructions.begin());
						f_iter != fct->second.body.instructions.end(); ++f_iter) {
					::cfgt< empty_t >::entriest::const_iterator cfg_node(m_cfg.entries.find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg.entries.end());
					if (f_iter->is_skip() || f_iter->is_location() || f_iter->is_end_function() ||
							f_iter->is_atomic_begin() || f_iter->is_atomic_end()) continue;
					if (f_iter->is_other()) {
						::irep_idt const& stmt(::to_code(f_iter->code).get_statement());
						// some generate code, according to symex_other.cpp
						if (stmt != "cpp_delete" && stmt != "cpp_delete[]" && stmt != "printf") continue;
					}
					for (::goto_programt::const_targetst::const_iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter)
						entry.first->second.insert(::std::make_pair(f_iter, *s_iter));
					// a single match suffices
					break;
				}
			}
			break;
		case F_EXIT:
			{
				::goto_functionst::function_mapt::const_iterator fct(m_ts.function_map.find(
							n->get_string_arg<F_EXIT>()));
				FSHELL2_PROD_CHECK1(::fshell2::Query_Processing_Error, fct != m_ts.function_map.end() &&
						fct->second.body_available, ::diagnostics::internal::to_string("Cannot evaluate ",
							*n, " (function not available)"));
				for (::goto_programt::instructionst::const_iterator f_iter(fct->second.body.instructions.begin());
						f_iter != fct->second.body.instructions.end(); ++f_iter) {
					if (!f_iter->is_return()) continue;
					::cfgt< empty_t >::entriest::const_iterator cfg_node(m_cfg.entries.find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg.entries.end());
					for (::goto_programt::const_targetst::const_iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter)
						entry.first->second.insert(::std::make_pair(f_iter, *s_iter));
				}
			}
			break;
		case F_EXPR:
			n->get_string_arg<F_EXPR>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_REGEXP:
			n->get_string_arg<F_REGEXP>();
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_BASICBLOCKENTRY:
			for (::goto_functionst::function_mapt::const_iterator iter(m_ts.function_map.begin());
					iter != m_ts.function_map.end(); ++iter) {
				if (!iter->second.body_available) continue;
				bool take_next(false);
				for (::goto_programt::instructionst::const_iterator f_iter(iter->second.body.instructions.begin());
						f_iter != iter->second.body.instructions.end(); ++f_iter) {
					::cfgt< empty_t >::entriest::const_iterator cfg_node(m_cfg.entries.find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg.entries.end());
					if (cfg_node->second.predecessors.empty()) take_next = true;
					if (f_iter->is_skip() || f_iter->is_location() || f_iter->is_end_function() ||
							f_iter->is_atomic_begin() || f_iter->is_atomic_end()) continue;
					if (f_iter->is_other()) {
						::irep_idt const& stmt(::to_code(f_iter->code).get_statement());
						// some generate code, according to symex_other.cpp
						if (stmt != "cpp_delete" && stmt != "cpp_delete[]" && stmt != "printf") continue;
					}
					if (!cfg_node->second.successors.empty() && (take_next ||
								cfg_node->second.successors.size() > 1 || cfg_node->second.predecessors.size() > 1)) {
						take_next = false;
						for (::goto_programt::const_targetst::const_iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter)
							entry.first->second.insert(::std::make_pair(f_iter, *s_iter));
					}
				}
			}
			break;
		case F_CONDITIONEDGE:
			for (::goto_functionst::function_mapt::const_iterator iter(m_ts.function_map.begin());
					iter != m_ts.function_map.end(); ++iter) {
				if (!iter->second.body_available) continue;
				for (::goto_programt::instructionst::const_iterator f_iter(iter->second.body.instructions.begin());
						f_iter != iter->second.body.instructions.end(); ++f_iter) {
					if (!f_iter->is_goto() || f_iter->guard.is_true() || f_iter->guard.is_false()) continue;
					::cfgt< empty_t >::entriest::const_iterator cfg_node(m_cfg.entries.find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg.entries.end());
					for (::goto_programt::const_targetst::const_iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter)
						entry.first->second.insert(::std::make_pair(f_iter, *s_iter));
				}
			}
			break;
		case F_DECISIONEDGE:
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
		case F_CONDITIONGRAPH:
			FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
			break;
	}
}

void Evaluate_Filter::visit(Filter_Intersection const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter_a()->accept(this);
	n->get_filter_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());
	::std::set_intersection(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Evaluate_Filter::visit(Filter_Setminus const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter_a()->accept(this);
	n->get_filter_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());
	::std::set_difference(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Evaluate_Filter::visit(Filter_Union const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, value_t())));
	if (!entry.second) return;
	
	n->get_filter_a()->accept(this);
	n->get_filter_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());
	::std::set_union(a_set->second.begin(), a_set->second.end(),
			b_set->second.begin(), b_set->second.end(),
			::std::inserter(entry.first->second, entry.first->second.begin()));
}

void Evaluate_Filter::visit(PM_Alternative const* n) {
	n->get_path_monitor_a()->accept(this);
	n->get_path_monitor_b()->accept(this);
}

void Evaluate_Filter::visit(PM_Concat const* n) {
	n->get_path_monitor_a()->accept(this);
	n->get_path_monitor_b()->accept(this);
}

void Evaluate_Filter::visit(PM_Filter_Adapter const* n) {
	n->get_filter()->accept(this);
}

void Evaluate_Filter::visit(PM_Next const* n) {
	n->get_path_monitor_a()->accept(this);
	n->get_path_monitor_b()->accept(this);
}

void Evaluate_Filter::visit(PM_Repeat const* n) {
	n->get_path_monitor()->accept(this);
}

void Evaluate_Filter::visit(Pathcov const* n) {
	n->get_filter()->accept(this);
}

void Evaluate_Filter::visit(Query const* n) {
	if (n->get_prefix()) {
		n->get_prefix()->accept(this);
	}

	n->get_cover()->accept(this);
	
	if (n->get_passing()) {
		n->get_passing()->accept(this);
	}
}

void Evaluate_Filter::visit(Statecov const* n) {
	n->get_filter()->accept(this);
}

void Evaluate_Filter::visit(TGS_Intersection const* n) {
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);
}

void Evaluate_Filter::visit(TGS_Setminus const* n) {
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);
}

void Evaluate_Filter::visit(TGS_Union const* n) {
	n->get_tgs_a()->accept(this);
	n->get_tgs_b()->accept(this);
}

void Evaluate_Filter::visit(Test_Goal_Sequence const* n) {
	for (Test_Goal_Sequence::seq_t::const_iterator iter(n->get_sequence().begin());
			iter != n->get_sequence().end(); ++iter) {
		if (iter->first) {
			iter->first->accept(this);
		}
		iter->second->accept(this);
	}

	if (n->get_suffix_monitor()) {
		n->get_suffix_monitor()->accept(this);
	}
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

