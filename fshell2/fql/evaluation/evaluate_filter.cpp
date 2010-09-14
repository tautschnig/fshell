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

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#  include <diagnostics/basic_exceptions/invalid_protocol.hpp>
#  include <diagnostics/basic_exceptions/invalid_argument.hpp>
#  include <diagnostics/basic_exceptions/not_implemented.hpp>
#endif

#include <fshell2/instrumentation/cfg.hpp>
#include <fshell2/instrumentation/goto_utils.hpp>
#include <fshell2/fql/ast/cp_alternative.hpp>
#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/depcov.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/nodecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pp_alternative.hpp>
#include <fshell2/fql/ast/pp_concat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>
#include <fshell2/fql/ast/repeat.hpp>
#include <fshell2/fql/ast/transform_pred.hpp>

#include <algorithm>
#include <iterator>
#include <sstream>

#include <cbmc/src/util/config.h>
#include <cbmc/src/langapi/language_ui.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Evaluate_Filter::Evaluate_Filter(::language_uit & manager) :
	m_manager(manager),
	m_pred_instr(m_manager),
	m_gf(0),
	m_cfg(0)
{
}

Evaluate_Filter::~Evaluate_Filter() {
}

void Evaluate_Filter::do_query(::goto_functionst & gf,
		::fshell2::instrumentation::CFG & cfg, Query const& query) {
	m_pred_instr.clear();
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !m_gf);
	m_gf = &gf;
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, !m_cfg);
	m_cfg = &cfg;
	m_filter_map.clear();
	m_predicate_map.clear();
	
	// we should always have the target graph for ID
	Filter_Function::Factory::get_instance().create<F_IDENTITY>()->accept(this);
	
	query.accept(this);
	m_cfg = 0;
	m_gf = 0;
}

target_graph_t const& Evaluate_Filter::get(Filter_Expr const& f) const {
	filter_value_t::const_iterator entry(m_filter_map.find(&f));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry != m_filter_map.end());
	return entry->second;
}
	
target_graph_t const& Evaluate_Filter::get(Predicate const& p) const {
	pred_to_filter_value_t::const_iterator entry(m_predicate_map.find(&p));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Protocol,
			entry != m_predicate_map.end());
	return entry->second;
}
	
bool Evaluate_Filter::ignore_function(::goto_functionst::goto_functiont const& fct) {
	if (!fct.body_available) return true;
	if (fct.body.instructions.empty()) return true;
	if (fct.is_inlined()) return true;
	if (!fct.body.instructions.empty() &&
			!fct.body.instructions.front().location.is_nil() &&
			((fct.body.instructions.front().location.get_file() == "<builtin-library>") ||
			(fct.body.instructions.front().location.get_file() == "<built-in>"))) return true;
	if (!fct.body.instructions.empty() &&
			fct.body.instructions.front().function == "main") return true;

	return false;
}
	
bool Evaluate_Filter::ignore_instruction(::goto_programt::instructiont const& e) {
	switch (e.type) {
		case SKIP:
		case END_FUNCTION:
		case ATOMIC_BEGIN:
		case ATOMIC_END:
		case DECL:
			return true;
		case OTHER:
			{
				::irep_idt const& stmt(::to_code(e.code).get_statement());
				// some generate code, according to symex_other.cpp
				if (stmt != "cpp_delete" && stmt != "cpp_delete[]" && stmt != "printf") return true;
			}
			return false;
		case NO_INSTRUCTION_TYPE:
		case GOTO:
		case ASSUME:
		case ASSERT:
		case LOCATION:
		case RETURN:
		case ASSIGN:
		case FUNCTION_CALL:
		case START_THREAD:
		case END_THREAD:
		case DEAD:
			return false;
	}

	return false;
}

void Evaluate_Filter::visit(CP_Alternative const* n) {
	n->get_cp_a()->accept(this);
	n->get_cp_b()->accept(this);
}

void Evaluate_Filter::visit(CP_Concat const* n) {
	n->get_cp_a()->accept(this);
	n->get_cp_b()->accept(this);
}

void Evaluate_Filter::visit(Depcov const* n) {
	n->get_filter_a()->accept(this);
	n->get_filter_b()->accept(this);
	n->get_filter_c()->accept(this);
}

void Evaluate_Filter::visit(Edgecov const* n) {
	n->get_filter_expr()->accept(this);
}

void Evaluate_Filter::visit(Filter_Compose const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, target_graph_t())));
	if (!entry.second) return;
	
	n->get_filter_expr_a()->accept(this);
	n->get_filter_expr_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_expr_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_expr_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());

	entry.first->second = a_set->second;
	entry.first->second.setintersection(b_set->second);
	CFA::nodes_t const L(entry.first->second.get_L());
	CFA::initial_states_t initial;
	::std::set_intersection(a_set->second.get_I().begin(), a_set->second.get_I().end(),
			L.begin(), L.end(), ::std::inserter(initial, initial.begin()));
	entry.first->second.set_I(initial);
}

template <filter_function_t F>
static ::goto_functionst::function_mapt::iterator get_funct_entry(::goto_functionst & gf,
		Filter_Function const* n)
{
	::std::string prefix("c::");
	::goto_functionst::function_mapt::iterator fct(gf.function_map.find(prefix
				+ n->get_string_arg<F>()));
	if (fct == gf.function_map.end() || Evaluate_Filter::ignore_function(fct->second)) {
		::std::ostringstream err;
		err << "Cannot evaluate " << *n << " (function not available)";
		FSHELL2_PROD_CHECK1(::fshell2::Query_Processing_Error, false, err.str());
	}

	return fct;
}

void Evaluate_Filter::visit(Filter_Function const* n) {
	using ::fshell2::instrumentation::CFG;
	using ::fshell2::instrumentation::GOTO_Transformation;

	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, target_graph_t())));
	if (!entry.second) return;
				
	CFA::edges_t edges;
	CFA::initial_states_t initial;

	switch (n->get_filter_type()) {
		case F_IDENTITY:
			for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
					iter != m_gf->function_map.end(); ++iter) {
				if (ignore_function(iter->second)) continue;
				bool first(true);
				for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
						f_iter != iter->second.body.instructions.end(); ++f_iter) {
					if (ignore_instruction(*f_iter)) continue;
					if (GOTO_Transformation::is_instrumented(f_iter)) continue;
					::fshell2::instrumentation::CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
					for (::fshell2::instrumentation::CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter) {
						if (first && iter->first == ("c::" + ::config.main)) {
							initial.insert(::std::make_pair(&(iter->second.body), f_iter));
							first = false;
						}
						edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
					}
				}
			}
			break;
		case F_FILE:
			{
				::std::string const& arg(n->get_string_arg<F_FILE>());
				for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
						iter != m_gf->function_map.end(); ++iter) {
					if (ignore_function(iter->second)) continue;
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
						!iter->second.body.instructions.front().location.is_nil());
					if (iter->second.body.instructions.front().location.get_file() != arg) continue;
					bool first(true);
					for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (ignore_instruction(*f_iter)) continue;
						if (GOTO_Transformation::is_instrumented(f_iter)) continue;
						CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
						for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter) {
							if (first) {
								initial.insert(::std::make_pair(&(iter->second.body), f_iter));
								first = false;
							}
							edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
						}
					}
				}
			}
			break;
		case F_LINE:
			{
				::std::ostringstream arg_str;
				arg_str << n->get_int_arg<F_LINE>();
				for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
						iter != m_gf->function_map.end(); ++iter) {
					if (ignore_function(iter->second)) continue;
					for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (ignore_instruction(*f_iter)) continue;
						if (GOTO_Transformation::is_instrumented(f_iter)) continue;
						if (f_iter->location.is_nil() ||
								f_iter->location.get_line().empty() ||
								f_iter->location.get_line() != arg_str.str()) continue;
						CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
						for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter) {
							initial.insert(::std::make_pair(&(iter->second.body), f_iter));
							edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
						}
					}
				}
			}
			break;
		case F_COLUMN:
			{
				::std::ostringstream arg_str;
				arg_str << n->get_int_arg<F_COLUMN>();
				for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
						iter != m_gf->function_map.end(); ++iter) {
					if (ignore_function(iter->second)) continue;
					for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (ignore_instruction(*f_iter)) continue;
						if (GOTO_Transformation::is_instrumented(f_iter)) continue;
						if (f_iter->location.is_nil() ||
								f_iter->location.get_column().empty() ||
								f_iter->location.get_column() != arg_str.str()) continue;
						CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
						for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter) {
							initial.insert(::std::make_pair(&(iter->second.body), f_iter));
							edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
						}
					}
				}
			}
			break;
		case F_FUNC:
			{
				::goto_functionst::function_mapt::iterator fct(get_funct_entry<F_FUNC>(*m_gf, n));
				bool first(true);
				for (::goto_programt::instructionst::iterator f_iter(fct->second.body.instructions.begin());
						f_iter != fct->second.body.instructions.end(); ++f_iter) {
					if (ignore_instruction(*f_iter)) continue;
					if (GOTO_Transformation::is_instrumented(f_iter)) continue;
					CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
					for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter) {
						// cover edges leaving the function? yes.
						// if (s_iter->first != &(fct->second.body)) continue;
						if (first) {
							initial.insert(::std::make_pair(&(fct->second.body), f_iter));
							first = false;
						}
						edges.insert(::std::make_pair(::std::make_pair(&(fct->second.body), f_iter), s_iter->first));
					}
				}
			}
			break;
		case F_LABEL:
			{
				::std::string const& arg(n->get_string_arg<F_LABEL>());
				for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
						iter != m_gf->function_map.end(); ++iter) {
					if (ignore_function(iter->second)) continue;
					for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (ignore_instruction(*f_iter)) continue;
						if (GOTO_Transformation::is_instrumented(f_iter)) continue;
						if (::std::find(f_iter->labels.begin(), f_iter->labels.end(), arg) == f_iter->labels.end()) continue;
						CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
						for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter) {
							initial.insert(::std::make_pair(&(iter->second.body), f_iter));
							edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
						}
					}
				}
			}
			break;
		case F_CALL:
			{
				::std::string const& arg(n->get_string_arg<F_CALL>());
				for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
						iter != m_gf->function_map.end(); ++iter) {
					if (ignore_function(iter->second)) continue;
					for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (ignore_instruction(*f_iter)) continue;
						if (GOTO_Transformation::is_instrumented(f_iter)) continue;
						if (!f_iter->is_function_call()) continue;
						::std::string const& f(::to_code_function_call(f_iter->code).function().get("identifier").as_string());
						::std::string::size_type delim(f.rfind("::"));
						if (arg != f.substr(::std::string::npos == delim?0:delim+2)) continue;
						CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
						for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter) {
							initial.insert(::std::make_pair(&(iter->second.body), f_iter));
							edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
						}
					}
				}
			}
			break;
		case F_CALLS:
			for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
					iter != m_gf->function_map.end(); ++iter) {
				if (ignore_function(iter->second)) continue;
				for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
						f_iter != iter->second.body.instructions.end(); ++f_iter) {
					if (ignore_instruction(*f_iter)) continue;
					if (GOTO_Transformation::is_instrumented(f_iter)) continue;
					if (!f_iter->is_function_call()) continue;
					CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
					for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter) {
						initial.insert(::std::make_pair(&(iter->second.body), f_iter));
						edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
					}
				}
			}
			break;
		case F_ENTRY:
			{
				::goto_functionst::function_mapt::iterator fct(get_funct_entry<F_ENTRY>(*m_gf, n));
				for (::goto_programt::instructionst::iterator f_iter(fct->second.body.instructions.begin());
						f_iter != fct->second.body.instructions.end(); ++f_iter) {
					if (ignore_instruction(*f_iter)) continue;
					if (GOTO_Transformation::is_instrumented(f_iter)) continue;
					CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
					for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter) {
						initial.insert(::std::make_pair(&(fct->second.body), f_iter));
						edges.insert(::std::make_pair(::std::make_pair(&(fct->second.body), f_iter), s_iter->first));
					}
					// a single match suffices
					break;
				}
			}
			break;
		case F_EXIT:
			{
				::goto_functionst::function_mapt::iterator fct(get_funct_entry<F_EXIT>(*m_gf, n));
				for (::goto_programt::instructionst::iterator f_iter(fct->second.body.instructions.begin());
						f_iter != fct->second.body.instructions.end(); ++f_iter) {
					if (ignore_instruction(*f_iter)) continue;
					if (GOTO_Transformation::is_instrumented(f_iter)) continue;
					if (!f_iter->is_return()) continue;
					CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
					for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter) {
						initial.insert(::std::make_pair(&(fct->second.body), f_iter));
						edges.insert(::std::make_pair(::std::make_pair(&(fct->second.body), f_iter), s_iter->first));
					}
				}
			}
			break;
		case F_EXPR:
			n->get_string_arg<F_EXPR>();
			FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
			break;
		case F_REGEXP:
			n->get_string_arg<F_REGEXP>();
			FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
			break;
		case F_BASICBLOCKENTRY:
			for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
					iter != m_gf->function_map.end(); ++iter) {
				if (ignore_function(iter->second)) continue;
				bool take_next(true);
				for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
						f_iter != iter->second.body.instructions.end(); ++f_iter) {
					if (ignore_instruction(*f_iter)) continue;
					if (GOTO_Transformation::is_instrumented(f_iter)) continue;
					CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
					if (!cfg_node->second.successors.empty() && (take_next ||
								cfg_node->second.successors.size() > 1 || cfg_node->second.predecessors.size() > 1)) {
						take_next = false;
						for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter) {
							initial.insert(::std::make_pair(&(iter->second.body), f_iter));
							edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
						}
					}
				}
			}
			break;
		case F_CONDITIONEDGE:
			for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
					iter != m_gf->function_map.end(); ++iter) {
				if (ignore_function(iter->second)) continue;
				for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
						f_iter != iter->second.body.instructions.end(); ++f_iter) {
					if (ignore_instruction(*f_iter)) continue;
					if (GOTO_Transformation::is_instrumented(f_iter)) continue;
					if (!f_iter->is_goto() || f_iter->guard.is_true() || f_iter->guard.is_false()) continue;
					CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
					FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
					for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
							s_iter != cfg_node->second.successors.end(); ++s_iter) {
						initial.insert(::std::make_pair(&(iter->second.body), f_iter));
						edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
					}
				}
			}
			break;
		case F_DECISIONEDGE:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
			break;
		case F_CONDITIONGRAPH:
			FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
			break;
		case F_DEF:
			{
				::std::string const& base_name(n->get_string_arg<F_DEF>());
				for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
						iter != m_gf->function_map.end(); ++iter) {
					if (ignore_function(iter->second)) continue;
					for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (ignore_instruction(*f_iter)) continue;
						if (GOTO_Transformation::is_instrumented(f_iter)) continue;
						if (!f_iter->is_assign()) continue;
						::std::list< ::exprt const* > symbols;
						::fshell2::instrumentation::find_symbols(::to_code_assign(f_iter->code).lhs(), symbols);
						bool use_stmt(false);
						for (::std::list< ::exprt const* >::const_iterator n_iter(symbols.begin());
								n_iter != symbols.end(); ++n_iter) {
							::std::string const& n((*n_iter)->get("identifier").as_string());
							::std::string::size_type delim(n.rfind("::"));
							if (base_name == n.substr(::std::string::npos == delim?0:delim+2)) {
								use_stmt = true;
								break;
							}
						}
						if (!use_stmt) continue;
						CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
						for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter) {
							initial.insert(::std::make_pair(&(iter->second.body), f_iter));
							edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
						}
					}
				}
			}
			break;
		case F_USE:
			{
				::std::string const& base_name(n->get_string_arg<F_USE>());
				for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
						iter != m_gf->function_map.end(); ++iter) {
					if (ignore_function(iter->second)) continue;
					for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (ignore_instruction(*f_iter)) continue;
						if (GOTO_Transformation::is_instrumented(f_iter)) continue;
						if (!f_iter->is_assign()) continue;
						::std::list< ::exprt const* > symbols;
						::fshell2::instrumentation::find_symbols(::to_code_assign(f_iter->code).rhs(), symbols);
						bool use_stmt(false);
						for (::std::list< ::exprt const* >::const_iterator n_iter(symbols.begin());
								n_iter != symbols.end(); ++n_iter) {
							::std::string const& n((*n_iter)->get("identifier").as_string());
							::std::string::size_type delim(n.rfind("::"));
							if (base_name == n.substr(::std::string::npos == delim?0:delim+2)) {
								use_stmt = true;
								break;
							}
						}
						if (!use_stmt) continue;
						CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
						for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter) {
							initial.insert(::std::make_pair(&(iter->second.body), f_iter));
							edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
						}
					}
				}
			}
			break;
		case F_STMTTYPE:
			{
				int types(n->get_int_arg<F_STMTTYPE>());
				for (::goto_functionst::function_mapt::iterator iter(m_gf->function_map.begin());
						iter != m_gf->function_map.end(); ++iter) {
					if (ignore_function(iter->second)) continue;
					for (::goto_programt::instructionst::iterator f_iter(iter->second.body.instructions.begin());
							f_iter != iter->second.body.instructions.end(); ++f_iter) {
						if (ignore_instruction(*f_iter)) continue;
						if (GOTO_Transformation::is_instrumented(f_iter)) continue;
						bool use_stmt(false);
						if (types & (STT_IF | STT_FOR | STT_WHILE | STT_SWITCH))
							if (f_iter->is_goto() && (!f_iter->guard.is_true() && !f_iter->guard.is_false())) use_stmt = true;
						if (types & (STT_CONDOP))
							if (f_iter->is_assign() && ::to_code_assign(f_iter->code).rhs().id() == "if") use_stmt = true;
						if (types & (STT_ASSERT))
							if (f_iter->is_assert()) use_stmt = true;
						if (!use_stmt) continue;
						CFG::entries_t::iterator cfg_node(m_cfg->find(f_iter));
						FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, cfg_node != m_cfg->end());
						for (CFG::successors_t::iterator s_iter(cfg_node->second.successors.begin());
								s_iter != cfg_node->second.successors.end(); ++s_iter) {
							initial.insert(::std::make_pair(&(iter->second.body), f_iter));
							edges.insert(::std::make_pair(::std::make_pair(&(iter->second.body), f_iter), s_iter->first));
						}
					}
				}
			}
			break;
	}
	
	if (edges.empty()) {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, initial.empty());
		::std::ostringstream warn;
		warn << "Filter expression " << *n << " evaluates to empty target graph";
		m_manager.warning(warn.str());
	}

	entry.first->second.set_E(edges);
	entry.first->second.set_I(initial);
}

void Evaluate_Filter::visit(Filter_Intersection const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, target_graph_t())));
	if (!entry.second) return;
	
	n->get_filter_expr_a()->accept(this);
	n->get_filter_expr_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_expr_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_expr_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());

	entry.first->second = a_set->second;
	entry.first->second.setintersection(b_set->second);
}

void Evaluate_Filter::visit(Filter_Setminus const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, target_graph_t())));
	if (!entry.second) return;
	
	n->get_filter_expr_a()->accept(this);
	n->get_filter_expr_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_expr_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_expr_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());

	entry.first->second = a_set->second;
	entry.first->second.setsubtract(b_set->second);
}

void Evaluate_Filter::visit(Filter_Union const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, target_graph_t())));
	if (!entry.second) return;
	
	n->get_filter_expr_a()->accept(this);
	n->get_filter_expr_b()->accept(this);

	filter_value_t::const_iterator a_set(m_filter_map.find(n->get_filter_expr_a()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, a_set != m_filter_map.end());
	filter_value_t::const_iterator b_set(m_filter_map.find(n->get_filter_expr_b()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, b_set != m_filter_map.end());

	entry.first->second = a_set->second;
	entry.first->second.setunion(b_set->second);
}

void Evaluate_Filter::visit(Nodecov const* n) {
	n->get_filter_expr()->accept(this);
}

void Evaluate_Filter::visit(PP_Alternative const* n) {
	n->get_pp_a()->accept(this);
	n->get_pp_b()->accept(this);
}

void Evaluate_Filter::visit(PP_Concat const* n) {
	n->get_pp_a()->accept(this);
	n->get_pp_b()->accept(this);
}

void Evaluate_Filter::visit(Pathcov const* n) {
	n->get_filter_expr()->accept(this);
}

void Evaluate_Filter::visit(Predicate const* n) {
	::std::pair< pred_to_filter_value_t::iterator, bool > entry(m_predicate_map.insert(
				::std::make_pair(n, target_graph_t())));
	if (!entry.second) return;
	
	filter_value_t::const_iterator id_set(m_filter_map.find(FQL_CREATE_FF0(F_IDENTITY)));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, id_set != m_filter_map.end());
	
	Predicate_Instrumentation::node_set_t nodes(id_set->second.get_L());
	CFA::edges_t edges;
	m_pred_instr.insert_predicate(*m_gf, nodes, n, edges);
	
	entry.first->second.set_E(edges);
	CFA::initial_states_t initial;
	for (CFA::edges_t::const_iterator iter(edges.begin()); iter != edges.end(); ++iter)
		initial.insert(iter->first);
	entry.first->second.set_I(initial);
}

void Evaluate_Filter::visit(Query const* n) {
	FSHELL2_DEBUG_ASSERT1(::diagnostics::Invalid_Argument, !n->get_prefix() && n->get_passing(),
			"Query not normalized");

	n->get_cover()->accept(this);
	n->get_passing()->accept(this);
}

void Evaluate_Filter::visit(Quote const* n) {
	n->get_pp()->accept(this);
}

void Evaluate_Filter::visit(Repeat const* n) {
	n->get_pp()->accept(this);
}

void Evaluate_Filter::visit(Transform_Pred const* n) {
	::std::pair< filter_value_t::iterator, bool > entry(m_filter_map.insert(
				::std::make_pair(n, target_graph_t())));
	if (!entry.second) return;
	
	n->get_filter_expr()->accept(this);
	filter_value_t::const_iterator filt_set(m_filter_map.find(n->get_filter_expr()));
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, filt_set != m_filter_map.end());

	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

