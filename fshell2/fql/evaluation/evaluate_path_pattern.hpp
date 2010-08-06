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

#ifndef FSHELL2__FQL__EVALUATION__EVALUATE_PATH_PATTERN_HPP
#define FSHELL2__FQL__EVALUATION__EVALUATE_PATH_PATTERN_HPP

/*! \file fshell2/fql/evaluation/evaluate_path_pattern.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:29:44 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>
#include <fshell2/fql/ast/standard_ast_visitor_aspect.hpp>
#include <fshell2/fql/concepts/trace_automaton.hpp>
#include <fshell2/fql/evaluation/evaluate_filter.hpp>

#include <map>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

class CFG;

FSHELL2_INSTRUMENTATION_NAMESPACE_END;

FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Evaluate_Path_Pattern : private Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Evaluate_Path_Pattern Self;

	public:
	typedef ::std::map< Path_Pattern_Expr const*, trace_automaton_t > pp_value_t;

	explicit Evaluate_Path_Pattern(::language_uit & manager);

	virtual ~Evaluate_Path_Pattern();

	void do_query(::goto_functionst & gf, ::fshell2::instrumentation::CFG & cfg,
			Query const& query);

	trace_automaton_t const& get(Path_Pattern_Expr const* pp) const;

	inline target_graph_t const& lookup_index(int index) const;
	inline int lookup_target_graph(target_graph_t const& tgg) const;
	inline int to_index(target_graph_t const& tgg);
	inline int id_index() const;
	inline int epsilon_index() const;

	private:
	typedef ::std::map< target_graph_t::node_t, int > node_counts_t;

	Evaluate_Filter m_eval_filter;
	::fshell2::instrumentation::CFG const* m_cfg;
	pp_value_t m_pp_map;
	::std::pair< pp_value_t::iterator, bool > m_entry;
	Target_Graph_Index m_target_graph_index;
	::std::list< target_graph_t > m_more_target_graphs;
	
	void dfs_build(trace_automaton_t & ta, ta_state_t const& state,
			target_graph_t::node_t const& root, int const bound,
			node_counts_t const& nc, target_graph_t const& tgg);

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
	 * \brief Visit a @ref fshell2::fql::PP_Alternative
	 * \param  n PP_Alternative
	 */
	virtual void visit(PP_Alternative const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PP_Concat
	 * \param  n PP_Concat
	 */
	virtual void visit(PP_Concat const* n);
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
	 * \brief Visit a @ref fshell2::fql::Quote
	 * \param  n Quote
	 */
	virtual void visit(Quote const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Query
	 * \param  n Query
	 */
	virtual void visit(Query const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Repeat
	 * \param  n Repeat
	 */
	virtual void visit(Repeat const* n);
	/*! \} */

	/*! \copydoc copy_constructor
	*/
	Evaluate_Path_Pattern( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};
	
target_graph_t const& Evaluate_Path_Pattern::lookup_index(int index) const {
	return *(m_target_graph_index.lookup_index(index));
}
	
int Evaluate_Path_Pattern::lookup_target_graph(target_graph_t const& tgg) const {
	return m_target_graph_index.lookup_target_graph(&tgg);
}
	
int Evaluate_Path_Pattern::to_index(target_graph_t const& tgg) {
	return m_target_graph_index.to_index(&tgg);
}
	
int Evaluate_Path_Pattern::id_index() const {
	return m_target_graph_index.id_index();
}

int Evaluate_Path_Pattern::epsilon_index() const {
	return m_target_graph_index.epsilon_index();
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__EVALUATE_PATH_PATTERN_HPP */
