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

#ifndef FSHELL2__FQL__EVALUATION__EVALUATE_COVERAGE_PATTERN_HPP
#define FSHELL2__FQL__EVALUATION__EVALUATE_COVERAGE_PATTERN_HPP

/*! \file fshell2/fql/evaluation/evaluate_coverage_pattern.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Oct 31 17:34:02 CET 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>
#include <fshell2/fql/ast/standard_ast_visitor_aspect.hpp>
#include <fshell2/fql/concepts/trace_automaton.hpp>
#include <fshell2/fql/evaluation/evaluate_path_pattern.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Evaluate_Coverage_Pattern : private Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Evaluate_Coverage_Pattern Self;

	public:
	class Test_Goal_States {
		public:
		Coverage_Pattern_Expr const * m_cp;
		ta_state_set_t m_tg_states;
		::std::list< Test_Goal_States > m_children;
		explicit Test_Goal_States(Coverage_Pattern_Expr const * cp) :
			m_cp(cp) {
		}
	};
	
	explicit Evaluate_Coverage_Pattern(::language_uit & manager);

	virtual ~Evaluate_Coverage_Pattern();
	
	Test_Goal_States const& do_query(::goto_functionst & gf,
			::fshell2::instrumentation::CFG & cfg, Query const& query);
	
	inline bool is_test_goal_state(ta_state_t const& state) const;

	trace_automaton_t const& get() const;

	inline Evaluate_Path_Pattern const& get_pp_eval() const;

	private:
	Evaluate_Path_Pattern m_pp_eval;
	Test_Goal_States m_test_goal_states;
	Test_Goal_States * m_current_tg_states;
	trace_automaton_t m_test_goal_automaton;
	ta_state_set_t m_current_final;
	
	static bool is_test_goal_state(Test_Goal_States const& tgs, ta_state_t const& state);
	void copy_from_path_pattern(Path_Pattern_Expr const* n);

	/*! \{
	 * \brief Visit a @ref fshell2::fql::CP_Alternative
	 * \param  n CP_Alternative
	 */
	virtual void visit(CP_Alternative const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::CP_Concat
	 * \param  n CP_Concat
	 */
	virtual void visit(CP_Concat const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Depcov
	 * \param  n Depcov
	 */
	virtual void visit(Depcov const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Edgecov
	 * \param  n Edgecov
	 */
	virtual void visit(Edgecov const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Nodecov
	 * \param  n Nodecov
	 */
	virtual void visit(Nodecov const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Pathcov
	 * \param  n Pathcov
	 */
	virtual void visit(Pathcov const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Predicate
	 * \param  n Predicate
	 */
	virtual void visit(Predicate const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Query
	 * \param  n Query
	 */
	virtual void visit(Query const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Quote
	 * \param  n Quote
	 */
	virtual void visit(Quote const* n);
	/*! \} */

	/*! \copydoc copy_constructor
	*/
	Evaluate_Coverage_Pattern( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

bool Evaluate_Coverage_Pattern::is_test_goal_state(ta_state_t const& state) const {
	return is_test_goal_state(m_test_goal_states, state);
}
	
inline Evaluate_Path_Pattern const& Evaluate_Coverage_Pattern::get_pp_eval() const {
	return m_pp_eval;
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__EVALUATE_COVERAGE_PATTERN_HPP */
