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

#ifndef FSHELL2__FQL__EVALUATION__AUTOMATON_INSERTER_HPP
#define FSHELL2__FQL__EVALUATION__AUTOMATON_INSERTER_HPP

/*! \file fshell2/fql/evaluation/automaton_inserter.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:32:01 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>
#include <fshell2/fql/ast/standard_ast_visitor_aspect.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/fql/concepts/trace_automaton.hpp>

#include <list>
#include <map>

#include <cbmc/src/goto-programs/goto_functions.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

class CFG;

FSHELL2_INSTRUMENTATION_NAMESPACE_END;

FSHELL2_FQL_NAMESPACE_BEGIN;

class Evaluate_Filter;
class Evaluate_Path_Monitor;

/*! \brief TODO
*/
class Automaton_Inserter : public Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Automaton_Inserter Self;

	public:
	typedef ::std::list< ::fshell2::instrumentation::GOTO_Transformation::goto_node_t > instrumentation_points_t;
	typedef ::std::map< ta_state_t, instrumentation_points_t > instrumentation_map_t;
	
	typedef ta_state_set_t test_goal_states_t;
	typedef ::std::map< Test_Goal_Sequence::seq_entry_t const* const, test_goal_states_t > test_goal_map_t;
	typedef ::std::map< ta_state_t, test_goal_map_t::iterator > test_goal_reverse_map_t;
	
	test_goal_states_t const& get_test_goal_states(Test_Goal_Sequence::seq_entry_t const& s) const;
	inline bool is_test_goal_state(ta_state_t const& state) const;

	Automaton_Inserter(Evaluate_Path_Monitor & pm_eval,
			Evaluate_Filter const& filter_eval,
			::goto_functionst & gf,
			::fshell2::instrumentation::CFG & cfg, ::contextt & context);

	void insert(Query const& query);

	instrumentation_points_t const& get_test_goal_instrumentation(ta_state_t const& state) const;
	CFA::edge_t const& get_target_graph_edge(::goto_programt::const_targett const& node) const;

	private:
	trace_automaton_t m_cov_seq_aut;
	ta_state_set_t m_current_final;
	Evaluate_Path_Monitor & m_pm_eval;
	Evaluate_Filter const& m_filter_eval;
	::goto_functionst & m_gf;
	::contextt & m_context;
	::fshell2::instrumentation::CFG & m_cfg;
	::fshell2::instrumentation::GOTO_Transformation m_inserter;
	instrumentation_map_t m_tg_instrumentation_map;
	::std::map< ::goto_programt::const_targett, CFA::edge_t > m_target_edge_map;
	test_goal_map_t m_test_goal_map;
	test_goal_map_t::iterator m_test_goal_map_entry;
	test_goal_reverse_map_t m_reverse_test_goal_map;

	void insert(char const * suffix, trace_automaton_t const& aut, ::exprt & final_cond, bool map_tg);
	
	/*! \{
	 * \brief Visit a @ref fshell2::fql::Edgecov
	 * \param  n Edgecov
	 */
	virtual void visit(Edgecov const* n);
	/*! \} */
	
	/*! \{
	 * \brief Visit a @ref fshell2::fql::Pathcov
	 * \param  n Pathcov
	 */
	virtual void visit(Pathcov const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Query
	 * \param  n Query
	 */
	virtual void visit(Query const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Statecov
	 * \param  n Statecov
	 */
	virtual void visit(Statecov const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::TGS_Intersection
	 * \param  n TGS_Intersection
	 */
	virtual void visit(TGS_Intersection const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::TGS_Setminus
	 * \param  n TGS_Setminus
	 */
	virtual void visit(TGS_Setminus const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::TGS_Union
	 * \param  n TGS_Union
	 */
	virtual void visit(TGS_Union const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Test_Goal_Sequence
	 * \param  n Test_Goal_Sequence
	 */
	virtual void visit(Test_Goal_Sequence const* n);
	/*! \} */

	/*! \copydoc copy_constructor
	*/
	Automaton_Inserter( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

bool Automaton_Inserter::is_test_goal_state(ta_state_t const& state) const {
	return (m_reverse_test_goal_map.end() != m_reverse_test_goal_map.find(state));
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__AUTOMATON_INSERTER_HPP */
