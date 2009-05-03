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

#ifndef FSHELL2__FQL__EVALUATION__COMPUTE_TEST_GOALS_HPP
#define FSHELL2__FQL__EVALUATION__COMPUTE_TEST_GOALS_HPP

/*! \file fshell2/fql/evaluation/compute_test_goals.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri May  1 21:45:36 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>
#include <fshell2/fql/ast/standard_ast_visitor_aspect.hpp>

#include <cbmc/src/cbmc/bmc.h>
#include <cbmc/src/cbmc/bv_cbmc.h>
#include <cbmc/src/propsolve/cnf_clause_list.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Evaluate_Filter;

/*! \brief TODO
*/
class Compute_Test_Goals : public ::bmct, public Standard_AST_Visitor_Aspect<AST_Visitor> 
{
	/*! \copydoc doc_self
	*/
	typedef Compute_Test_Goals Self;

	public:
	typedef ::literalt test_goal_t;
	typedef ::std::set< test_goal_t > value_t;
	typedef ::std::map< Test_Goal_Set const*, value_t > tgs_value_t;
	
	Compute_Test_Goals(::language_uit & manager, ::optionst const& opts, Evaluate_Filter & eval);

	virtual ~Compute_Test_Goals();

	value_t const& compute(Query const& query);

	inline ::cnf_clause_list_assignmentt & get_cnf();

	inline ::boolbvt const& get_bv() const;

	inline ::namespacet const& get_ns() const;

	private:
	virtual bool decide_default();

	void initialize();
	
	/*! \{
	 * \brief Visit a @ref fshell2::fql::Query
	 * \param  n Query
	 */
	virtual void visit(Query const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Test_Goal_Sequence
	 * \param  n Test_Goal_Sequence
	 */
	virtual void visit(Test_Goal_Sequence const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Restriction_Automaton
	 * \param  n Restriction_Automaton
	 */
	virtual void visit(Restriction_Automaton const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Abstraction
	 * \param  n Abstraction
	 */
	virtual void visit(Abstraction const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Predicate
	 * \param  n Predicate
	 */
	virtual void visit(Predicate const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::TGS_Union
	 * \param  n TGS_Union
	 */
	virtual void visit(TGS_Union const* n);
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

	bool m_is_initialized;
	Evaluate_Filter & m_eval_filter;
	::cnf_clause_list_assignmentt m_cnf;
	::bv_cbmct m_bv;

	typedef ::std::multimap< goto_programt::const_targett,
			::std::pair< ::literalt, ::literalt > > pc_to_bool_var_t;
	pc_to_bool_var_t m_pc_to_bool_var_and_guard;

	tgs_value_t m_tgs_map;

	/*! \copydoc copy_constructor
	*/
	Compute_Test_Goals( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};
	
inline ::cnf_clause_list_assignmentt & Compute_Test_Goals::get_cnf() {
	return m_cnf;
}
	
inline ::boolbvt const& Compute_Test_Goals::get_bv() const {
	return m_bv;
}

inline ::namespacet const& Compute_Test_Goals::get_ns() const {
	return this->ns;
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__COMPUTE_TEST_GOALS_HPP */
