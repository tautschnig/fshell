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
#include <fshell2/fql/evaluation/cfa.hpp>

#include <map>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

class CFG;

FSHELL2_INSTRUMENTATION_NAMESPACE_END;

FSHELL2_FQL_NAMESPACE_BEGIN;

typedef CFA target_graph_t;

/*! \brief TODO
*/
class Evaluate_Filter : public Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Evaluate_Filter Self;

	public:
	typedef ::std::map< Filter const*, target_graph_t > filter_value_t;
	typedef ::std::map< target_graph_t::edge_t, ::std::set< Filter const* > > edge_to_filters_t;
	typedef ::std::map< ::goto_programt::const_targett, edge_to_filters_t > node_to_filters_t; 

	Evaluate_Filter(::goto_functionst & ts,
			::fshell2::instrumentation::CFG & cfg);

	virtual ~Evaluate_Filter();

	static bool skip_function(::goto_functionst::goto_functiont const& fct);

	target_graph_t const& get(Filter const& f) const;
	edge_to_filters_t const& get(::goto_programt::const_targett const& n) const;

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
	 * \brief Visit a @ref fshell2::fql::Filter_Complement
	 * \param  n Filter_Complement
	 */
	virtual void visit(Filter_Complement const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Filter_Enclosing_Scopes
	 * \param  n Filter_Enclosing_Scopes
	 */
	virtual void visit(Filter_Enclosing_Scopes const* n);
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
	 * \brief Visit a @ref fshell2::fql::PM_Next
	 * \param  n PM_Next
	 */
	virtual void visit(PM_Next const* n);
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
	::goto_functionst & m_gf;
	::fshell2::instrumentation::CFG & m_cfg;
	filter_value_t m_filter_map;
	node_to_filters_t m_node_to_filters_map;

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
