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

#ifndef FSHELL2__FQL__EVALUATION__EVALUATE_PATH_MONITOR_HPP
#define FSHELL2__FQL__EVALUATION__EVALUATE_PATH_MONITOR_HPP

/*! \file fshell2/fql/evaluation/evaluate_path_monitor.hpp
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

#include <map>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Evaluate_Filter;

/*! \brief TODO
*/
class Evaluate_Path_Monitor : public Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Evaluate_Path_Monitor Self;

	public:

	typedef ::std::map< Path_Monitor_Expr const*, trace_automaton_t > pm_value_t;

	explicit Evaluate_Path_Monitor(Evaluate_Filter const& filter_eval);

	virtual ~Evaluate_Path_Monitor();

	trace_automaton_t const& get(Path_Monitor_Expr const* pm) const;

	inline target_graph_t const& lookup_index(int index) const;
	inline int lookup_target_graph(target_graph_t const& tgg) const;
	inline int to_index(target_graph_t const& tgg);
	inline int id_index() const;
	inline int epsilon_index() const;

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Alternative
	 * \param  n PM_Alternative
	 */
	virtual void visit(PM_Alternative const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Concat
	 * \param  n PM_Concat
	 */
	virtual void visit(PM_Concat const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Filter_Adapter
	 * \param  n PM_Filter_Adapter
	 */
	virtual void visit(PM_Filter_Adapter const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Repeat
	 * \param  n PM_Repeat
	 */
	virtual void visit(PM_Repeat const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Query
	 * \param  n Query
	 */
	virtual void visit(Query const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Predicate
	 * \param  n Predicate
	 */
	virtual void visit(Predicate const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Test_Goal_Sequence
	 * \param  n Test_Goal_Sequence
	 */
	virtual void visit(Test_Goal_Sequence const* n);
	/*! \} */

	private:
	Evaluate_Filter const& m_eval_filter;
	Target_Graph_Index m_target_graph_index;
	pm_value_t m_pm_map;
	::std::pair< pm_value_t::iterator, bool > m_entry;

	/*! \copydoc copy_constructor
	*/
	Evaluate_Path_Monitor( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};
	
target_graph_t const& Evaluate_Path_Monitor::lookup_index(int index) const {
	return *(m_target_graph_index.lookup_index(index));
}
	
int Evaluate_Path_Monitor::lookup_target_graph(target_graph_t const& tgg) const {
	return m_target_graph_index.lookup_target_graph(&tgg);
}
	
int Evaluate_Path_Monitor::to_index(target_graph_t const& tgg) {
	return m_target_graph_index.to_index(&tgg);
}
	
int Evaluate_Path_Monitor::id_index() const {
	return m_target_graph_index.id_index();
}

int Evaluate_Path_Monitor::epsilon_index() const {
	return m_target_graph_index.epsilon_index();
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__EVALUATE_PATH_MONITOR_HPP */
