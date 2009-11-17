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

#ifndef FSHELL2__FQL__EVALUATION__PREDICATE_INSTRUMENTATION_HPP
#define FSHELL2__FQL__EVALUATION__PREDICATE_INSTRUMENTATION_HPP

/*! \file fshell2/fql/evaluation/predicate_instrumentation.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sat Oct 31 17:33:15 CET 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/fql/concepts/target_graph.hpp>
#include <fshell2/fql/ast/ast_visitor.hpp>
#include <fshell2/fql/ast/standard_ast_visitor_aspect.hpp>

#include <cbmc/src/goto-programs/goto_functions.h>

#include <map>
#include <set>

class language_uit;

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Evaluate_Filter;

/*! \brief TODO
*/
class Predicate_Instrumentation : public Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Predicate_Instrumentation Self;

	public:
	typedef ::std::map< Predicate const*, target_graph_t::edge_t > pred_instrumentation_map_t;
	typedef ::std::map< target_graph_t::node_t, pred_instrumentation_map_t > node_to_pred_instrumentation_map_t;

	Predicate_Instrumentation(Evaluate_Filter const& eval_filter, ::goto_functionst & gf, ::language_uit & manager);

	virtual ~Predicate_Instrumentation();

	target_graph_t::edge_t const& get(target_graph_t::node_t const& node, Predicate const* pred) const;

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Edgecov
	 * \param  n Edgecov
	 */
	virtual void visit(Edgecov const* n);
	/*! \} */

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
	 * \brief Visit a @ref fshell2::fql::PM_Postcondition
	 * \param  n PM_Postcondition
	 */
	virtual void visit(PM_Postcondition const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Precondition
	 * \param  n PM_Precondition
	 */
	virtual void visit(PM_Precondition const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Repeat
	 * \param  n PM_Repeat
	 */
	virtual void visit(PM_Repeat const* n);
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

	private:
	Evaluate_Filter const& m_eval_filter;
	::goto_functionst & m_gf;
	::language_uit & m_manager;
	::fshell2::instrumentation::GOTO_Transformation m_inserter;
	::std::set< target_graph_t::node_t > m_current_node_set;
	node_to_pred_instrumentation_map_t m_node_to_pred_instr;

	void insert_predicate(Predicate const* pred);

	/*! \copydoc copy_constructor
	*/
	Predicate_Instrumentation( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__PREDICATE_INSTRUMENTATION_HPP */
