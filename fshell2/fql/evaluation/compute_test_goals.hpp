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

#include <vector>
#include <set>
#include <map>

FSHELL2_NAMESPACE_BEGIN;

FSHELL2_STATISTICS_NAMESPACE_BEGIN;

class Statistics;

FSHELL2_STATISTICS_NAMESPACE_END;

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
	inline ::cnf_clause_list_assignmentt const& get_cnf() const;

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
	
inline ::cnf_clause_list_assignmentt const& CNF_Conversion::get_cnf() const {
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
class Compute_Test_Goals : protected Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Compute_Test_Goals Self;

	public:
	Compute_Test_Goals(::language_uit & manager,
			::optionst const& opts);

	virtual ~Compute_Test_Goals();

	virtual CNF_Conversion & do_query(::goto_functionst & gf, Query const& query) = 0;

	virtual bool get_satisfied_test_goals(::cnf_clause_list_assignmentt const& cnf,
			CNF_Conversion::test_goals_t & tgs) const = 0;

	protected:
	typedef ::std::map< ::literalt, ::std::pair< ::literalt, ::literalt > > and_map_t;
	typedef ::std::map< ::literalt, ::std::set< ::literalt > > reverse_or_lit_map_t;
	typedef ::std::map< ::literalt, ::std::set< ::std::pair< ::literalt, ::literalt > > > reverse_and_lit_map_t;
	typedef ::std::vector< ::std::pair< ::goto_programt::const_targett,
				::goto_programt::const_targett > > ctx_coll_t;
	typedef ::std::map< ::literalt, ctx_coll_t > tg_to_ctx_map_t;
	
	::language_uit & m_manager;
	::optionst const& m_opts;
	CNF_Conversion m_equation;
	Evaluate_Coverage_Pattern::Test_Goal_States const* m_test_goal_states;
	CNF_Conversion::test_goals_t m_test_goals;
	and_map_t m_and_map;
	tg_to_ctx_map_t m_tg_to_ctx_map;
	reverse_and_lit_map_t m_reverse_and_map;
	reverse_or_lit_map_t m_reverse_or_map;

	void print_test_goal(::literalt const& tg, ::std::ostream & os) const;
	void store_mapping(::literalt const& tg, ::bvt const& or_set,
			::goto_programt::const_targett const& src_context,
			::goto_programt::const_targett const& dest_context);
	void store_mapping(::literalt const& tg, ::bvt const& or_set,
			ctx_coll_t const& contexts);

	virtual void do_atom(Coverage_Pattern_Expr const* n, bool epsilon_permitted, bool make_single) = 0;
	
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
	Compute_Test_Goals( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};


/*! \brief TODO
*/
class Compute_Test_Goals_From_Instrumentation : public Compute_Test_Goals
{
	/*! \copydoc doc_self
	*/
	typedef Compute_Test_Goals_From_Instrumentation Self;

	public:
	Compute_Test_Goals_From_Instrumentation(::language_uit & manager,
			::optionst const& opts);

	virtual ~Compute_Test_Goals_From_Instrumentation();

	CNF_Conversion & do_query(::goto_functionst & gf, Query const& query);

	virtual bool get_satisfied_test_goals(::cnf_clause_list_assignmentt const& cnf,
			CNF_Conversion::test_goals_t & tgs) const;

	private:
	typedef ::std::map< ::goto_programt::const_targett,
				::std::map< ::goto_programt::const_targett, 
					::std::set< ::literalt > > > pc_to_context_and_guards_t;
	typedef ::std::map< ::goto_programt::const_targett,
				::std::set< ::goto_programt::const_targett > > context_to_pcs_t;
	
	Automaton_Inserter m_aut_insert;
	pc_to_context_and_guards_t m_pc_to_guard;

	bool find_all_contexts(context_to_pcs_t & context_to_pcs);

	void context_to_literals(::goto_programt::const_targett const& context,
		::std::set< ::goto_programt::const_targett > const& pcs, ::bvt & test_goal_literals) const;

	virtual void do_atom(Coverage_Pattern_Expr const* n, bool epsilon_permitted, bool make_single);
	
	/*! \copydoc copy_constructor
	*/
	Compute_Test_Goals_From_Instrumentation( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

class Run_Tree;

/*! \brief TODO
*/
class Compute_Test_Goals_Boolean : public Compute_Test_Goals
{
	/*! \copydoc doc_self
	*/
	typedef Compute_Test_Goals_Boolean Self;

	public:
	Compute_Test_Goals_Boolean(::language_uit & manager, ::optionst const& opts,
			::fshell2::statistics::Statistics & stats);

	virtual ~Compute_Test_Goals_Boolean();

	CNF_Conversion & do_query(::goto_functionst & gf, Query const& query);

	virtual bool get_satisfied_test_goals(::cnf_clause_list_assignmentt const& cnf,
			CNF_Conversion::test_goals_t & tgs) const;

	private:
	typedef ::std::map< target_graph_t::edge_t, ::std::set< target_graph_t const* > > edge_to_target_graphs_t;
	typedef ::std::map< target_graph_t::node_t, ::std::set< target_graph_t const* > > node_to_target_graphs_t;
	typedef ::std::map< ::goto_programt::const_targett, edge_to_target_graphs_t > loc_to_edge_target_graphs_t; 
	typedef ::std::map< ::goto_programt::const_targett, node_to_target_graphs_t > loc_to_node_target_graphs_t; 
	typedef ::std::map< ta_state_t, ta_state_set_t > symbol_transition_map_t;
	typedef ::std::map< int, symbol_transition_map_t > transition_map_t;
	typedef ::std::map< target_graph_t const*, int > target_graph_to_int_t;
	typedef ::std::map< ta_state_t, ::bvt > bv_cache_t;
	typedef ::std::map< ta_state_t, ::literalt > eq_cache_t;
	typedef ::std::map< target_graph_t::edge_t, ::std::set< ::literalt > > edge_to_literal_map_t; 
	typedef ::std::map< ta_state_t, edge_to_literal_map_t > state_edge_lit_map_t;

	class Edge_Goal {
		public:
			Edge_Goal(::literalt const& edge_guard, ::literalt const& tg,
					ta_state_t const& src, ta_state_t const& dest) :
				m_edge_guard(edge_guard), m_tg(tg), m_src(src), m_dest(dest) {}

			Edge_Goal(Edge_Goal const& other) :
				m_edge_guard(other.m_edge_guard), m_tg(other.m_tg),
				m_src(other.m_src), m_dest(other.m_dest) {}

			::literalt const m_edge_guard;
			::literalt const m_tg;
			ta_state_t const m_src;
			ta_state_t const m_dest;
	};
	typedef	::std::multimap< ::symex_target_equationt::SSA_stepst::const_iterator,
				Edge_Goal > step_to_trans_mmap_t;
	typedef ::std::map< ::symex_target_equationt::SSA_stepst::const_iterator,
				unsigned > node_rep_map_t;

	::fshell2::statistics::Statistics & m_stats;
	Evaluate_Coverage_Pattern m_cp_eval;
	loc_to_node_target_graphs_t m_loc_to_node_target_graphs_map;
	loc_to_edge_target_graphs_t m_loc_to_edge_target_graphs_map;
	state_edge_lit_map_t m_tg_to_lit_map;
	step_to_trans_mmap_t m_step_to_trans;
	step_to_trans_mmap_t m_node_step_to_trans;
	node_rep_map_t m_node_rep_map;
	
	::bvt const& state_to_bvt_lookup(::bv_utilst & bv_utils, bv_cache_t & bv_cache,
			ta_state_t const& state, unsigned const width);
	::literalt const& make_equals(::bv_utilst & bv_utils, eq_cache_t & eq_cache,
			::bvt const& state_vec, bv_cache_t & bv_cache, ta_state_t const&
			state, unsigned const width);
	void build(trace_automaton_t const& aut, bool map_tg);
	void do_step(::cnf_clause_list_assignmentt & cnf, ::bv_utilst & bv_utils,
			bv_cache_t & bv_cache, eq_cache_t & prev_eq_cache, eq_cache_t & next_eq_cache,
			::bvt & prev_state, ta_state_set_t & possibly_reached_states,
			unsigned const width,
			::symex_target_equationt::SSA_stepst::const_iterator const step,
			target_graph_to_int_t const& local_target_graph_map, bool map_tg,
			transition_map_t const& transitions, ta_state_map_t const&
			reverse_state_map);
	
	virtual void do_atom(Coverage_Pattern_Expr const* n, bool epsilon_permitted, bool make_single);

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
