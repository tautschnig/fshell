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

#ifndef FSHELL2__FQL__EVALUATION__EVALUATE_FILTER_HPP
#define FSHELL2__FQL__EVALUATION__EVALUATE_FILTER_HPP

/*! \file fshell2/fql/evaluation/evaluate_filter.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr 30 13:31:24 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>
#include <fshell2/fql/ast/standard_ast_visitor_aspect.hpp>
#include <fshell2/fql/concepts/target_graph.hpp>
#include <fshell2/fql/evaluation/predicate_instrumentation.hpp>

#include <map>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

class CFG;

FSHELL2_INSTRUMENTATION_NAMESPACE_END;

FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Evaluate_Filter : public Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Evaluate_Filter Self;

	public:
	typedef ::std::map< Filter_Expr const*, target_graph_t > filter_value_t;
	typedef ::std::map< Predicate const*, target_graph_t > pred_to_filter_value_t;

	Evaluate_Filter(::goto_functionst & ts,
			::fshell2::instrumentation::CFG & cfg,
			::language_uit & manager);

	virtual ~Evaluate_Filter();

	static bool ignore_function(::goto_functionst::goto_functiont const& fct);
	static bool ignore_instruction(::goto_programt::instructiont const& e);

	target_graph_t const& get(Filter_Expr const& f) const;
	target_graph_t const& get(Predicate const& p) const;

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
	 * \brief Visit a @ref fshell2::fql::Filter_Compose
	 * \param  n Filter_Compose
	 */
	virtual void visit(Filter_Compose const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Filter_Function
	 * \param  n Filter_Function
	 */
	virtual void visit(Filter_Function const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Filter_Intersection
	 * \param  n Filter_Intersection
	 */
	virtual void visit(Filter_Intersection const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Filter_Setminus
	 * \param  n Filter_Setminus
	 */
	virtual void visit(Filter_Setminus const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Filter_Union
	 * \param  n Filter_Union
	 */
	virtual void visit(Filter_Union const* n);
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

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Transform_Pred
	 * \param  n Transform_Pred
	 */
	virtual void visit(Transform_Pred const* n);
	/*! \} */
		
	private:
	::goto_functionst & m_gf;
	::fshell2::instrumentation::CFG & m_cfg;
	::fshell2::fql::Predicate_Instrumentation m_pred_instr;
	filter_value_t m_filter_map;
	pred_to_filter_value_t m_predicate_map;

	/*! \copydoc copy_constructor
	*/
	Evaluate_Filter( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};
	
FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__EVALUATE_FILTER_HPP */
