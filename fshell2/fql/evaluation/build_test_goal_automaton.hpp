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

#ifndef FSHELL2__FQL__EVALUATION__BUILD_TEST_GOAL_AUTOMATON_HPP
#define FSHELL2__FQL__EVALUATION__BUILD_TEST_GOAL_AUTOMATON_HPP

/*! \file fshell2/fql/evaluation/build_test_goal_automaton.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Oct 31 17:34:02 CET 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>
#include <fshell2/fql/ast/standard_ast_visitor_aspect.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/concepts/trace_automaton.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

class CFG;

FSHELL2_INSTRUMENTATION_NAMESPACE_END;

FSHELL2_FQL_NAMESPACE_BEGIN;

class Evaluate_Filter;
class Evaluate_Path_Monitor;
class Predicate_Instrumentation;

/*! \brief TODO
*/
class Build_Test_Goal_Automaton : public Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Build_Test_Goal_Automaton Self;

	public:

	typedef ::std::map< target_graph_t::node_t, int > node_counts_t;
	
	typedef ta_state_set_t test_goal_states_t;
	typedef ::std::map< Test_Goal_Sequence::seq_entry_t const* const, test_goal_states_t > test_goal_map_t;
	typedef ::std::map< ta_state_t, test_goal_map_t::iterator > test_goal_reverse_map_t;
	
	test_goal_states_t const& get_test_goal_states(Test_Goal_Sequence::seq_entry_t const& s) const;
	inline bool is_test_goal_state(ta_state_t const& state) const;

	Build_Test_Goal_Automaton(Evaluate_Filter const& eval_filter,
			Evaluate_Path_Monitor & pm_eval,
			Predicate_Instrumentation const& pred_instr,
			::fshell2::instrumentation::CFG const& cfg);

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Query
	 * \param  n Query
	 */
	virtual void visit(Query const* n);
	/*! \} */

	trace_automaton_t const& get() const;

	private:
	Evaluate_Filter const& m_eval_filter;
	Evaluate_Path_Monitor & m_pm_eval;
	Predicate_Instrumentation const& m_pred_instr;
	::fshell2::instrumentation::CFG const& m_cfg;
	
	trace_automaton_t m_test_goal_automaton;
	ta_state_set_t m_current_final;
	::std::list< target_graph_t > m_more_target_graphs;
	test_goal_map_t m_test_goal_map;
	test_goal_map_t::iterator m_test_goal_map_entry;
	test_goal_reverse_map_t m_reverse_test_goal_map;
	
	void dfs_build(ta_state_t const& state, target_graph_t::node_t const& root, int const bound,
		node_counts_t const& nc, target_graph_t const& tgg);
	
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
	 * \brief Visit a @ref fshell2::fql::TGS_Postcondition
	 * \param  n TGS_Postcondition
	 */
	virtual void visit(TGS_Postcondition const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::TGS_Precondition
	 * \param  n TGS_Precondition
	 */
	virtual void visit(TGS_Precondition const* n);
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
	Build_Test_Goal_Automaton( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

bool Build_Test_Goal_Automaton::is_test_goal_state(ta_state_t const& state) const {
	return (m_reverse_test_goal_map.end() != m_reverse_test_goal_map.find(state));
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__BUILD_TEST_GOAL_AUTOMATON_HPP */
