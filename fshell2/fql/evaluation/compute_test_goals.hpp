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
#include <fshell2/fql/concepts/trace_automaton.hpp>
#include <fshell2/fql/evaluation/automaton_inserter.hpp>

#include <cbmc/src/cbmc/bmc.h>
#include <cbmc/src/cbmc/bv_cbmc.h>
#include <cbmc/src/solvers/sat/cnf_clause_list.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Evaluate_Path_Pattern;
class Evaluate_Coverage_Pattern;

/*! \brief TODO
*/
class CNF_Conversion : public ::bmct
{
	/*! \copydoc doc_self
	*/
	typedef CNF_Conversion Self;

	public:
	typedef ::std::set< ::literalt > test_goals_t;
	
	CNF_Conversion(::language_uit & manager, ::optionst const& opts);

	virtual ~CNF_Conversion();

	void convert(::goto_functionst const& gf); 

	void mark_as_test_goal(::literalt const& lit);
	inline test_goals_t const& get_test_goal_literals() const;

	inline ::cnf_clause_list_assignmentt & get_cnf();

	inline ::boolbvt const& get_bv() const;
	inline ::symex_target_equationt const& get_equation() const;

	inline ::namespacet const& get_ns() const;

	private:
	virtual bool decide_default();
	
	::cnf_clause_list_assignmentt m_cnf;
	::bv_cbmct m_bv;
	test_goals_t m_test_goals;

	/*! \copydoc copy_constructor
	*/
	CNF_Conversion( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};
	
inline CNF_Conversion::test_goals_t const& CNF_Conversion::get_test_goal_literals() const {
	return m_test_goals;
}
	
inline ::cnf_clause_list_assignmentt & CNF_Conversion::get_cnf() {
	return m_cnf;
}
	
inline ::boolbvt const& CNF_Conversion::get_bv() const {
	return m_bv;
}
	
inline ::symex_target_equationt const& CNF_Conversion::get_equation() const {
	return this->_equation;
}

inline ::namespacet const& CNF_Conversion::get_ns() const {
	return this->ns;
}


/*! \brief TODO
*/
class Compute_Test_Goals_From_Instrumentation : private Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Compute_Test_Goals_From_Instrumentation Self;

	public:
	Compute_Test_Goals_From_Instrumentation(::goto_functionst const& gf,
			::language_uit & manager, ::optionst const& opts);

	virtual ~Compute_Test_Goals_From_Instrumentation();

	CNF_Conversion & do_query(Query const& query);

	private:
	typedef ::std::map< ::goto_programt::const_targett,
				::std::map< ::goto_programt::const_targett, 
					::std::set< ::literalt > > > pc_to_context_and_guards_t;
	typedef ::std::map< ::goto_programt::const_targett,
				::std::set< ::goto_programt::const_targett > > context_to_pcs_t;
	
	::goto_functionst const& m_gf;
	::language_uit & m_manager;
	::optionst const& m_opts;
	Automaton_Inserter m_aut_insert;
	CNF_Conversion m_equation;
	Evaluate_Coverage_Pattern::Test_Goal_States const* m_test_goal_states;
	pc_to_context_and_guards_t m_pc_to_guard;
	CNF_Conversion::test_goals_t m_test_goals;

	bool find_all_contexts(context_to_pcs_t & context_to_pcs);

	void context_to_literals(::goto_programt::const_targett const& context,
		::std::set< ::goto_programt::const_targett > const& pcs, ::bvt & test_goal_literals) const;

	void do_atom(Coverage_Pattern_Expr const* n, bool epsilon_permitted, bool make_single);
	
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
	Compute_Test_Goals_From_Instrumentation( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

/*! \brief TODO
*/
class Compute_Test_Goals_Boolean
{
	/*! \copydoc doc_self
	*/
	typedef Compute_Test_Goals_Boolean Self;

	public:
	Compute_Test_Goals_Boolean(CNF_Conversion & equation,
			Evaluate_Path_Pattern const& pp_eval,
			Evaluate_Coverage_Pattern const& cp_eval);

	virtual ~Compute_Test_Goals_Boolean();

	void compute(Query const& query);

	private:
	CNF_Conversion & m_equation;
	Evaluate_Path_Pattern const& m_pp_eval;
	Evaluate_Coverage_Pattern const& m_cp_eval;
	/*typedef ::std::map< ta_state_t, 
		::std::map< ::goto_programt::const_targett, test_goal_t > > state_context_tg_t;
	state_context_tg_t m_state_context_tg_map;*/
	typedef ::std::map< ::goto_programt::const_targett,
				::std::map< ::goto_programt::const_targett, 
					::std::set< ::literalt > > > pc_to_context_and_guards_t;
	pc_to_context_and_guards_t m_pc_to_guard;

	/*! \copydoc copy_constructor
	*/
	Compute_Test_Goals_Boolean( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__COMPUTE_TEST_GOALS_HPP */
