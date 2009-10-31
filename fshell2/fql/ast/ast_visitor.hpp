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

#ifndef FSHELL2__FQL__AST__AST_VISITOR_HPP
#define FSHELL2__FQL__AST__AST_VISITOR_HPP

/*! \file fshell2/fql/ast/ast_visitor.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Apr 20 23:53:39 CEST 2009 
*/

#include <fshell2/config/config.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Edgecov;
class FQL_Node;
class Filter_Expr;
class Filter_Complement;
class Filter_Compose;
class Filter_Enclosing_Scopes;
class Filter_Function;
class Filter_Intersection;
class Filter_Setminus;
class Filter_Union;
class PM_Alternative;
class PM_Concat;
class PM_Filter_Adapter;
class PM_Repeat;
class Path_Monitor_Expr;
class Pathcov;
class Predicate;
class Query;
class Statecov;
class TGS_Intersection;
class TGS_Setminus;
class TGS_Union;
class Test_Goal_Sequence;
class Test_Goal_Set;

/*! \brief TODO
*/
class AST_Visitor
{
	public:
		/*! \brief Destructor
		*/
		virtual ~AST_Visitor();

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Edgecov
		 * \param  n Edgecov
		 */
		virtual void visit(Edgecov const* n) = 0;
		virtual void visit(Edgecov const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::FQL_Node
		 * \param  n FQL_Node
		 */
		virtual void visit(FQL_Node const* n) = 0;
		virtual void visit(FQL_Node const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Expr
		 * \param  n Filter
		 */
		virtual void visit(Filter_Expr const* n) = 0;
		virtual void visit(Filter_Expr const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Compose
		 * \param  n Filter_Compose
		 */
		virtual void visit(Filter_Compose const* n) = 0;
		virtual void visit(Filter_Compose const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Complement
		 * \param  n Filter_Complement
		 */
		virtual void visit(Filter_Complement const* n) = 0;
		virtual void visit(Filter_Complement const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Enclosing_Scopes
		 * \param  n Filter_Enclosing_Scopes
		 */
		virtual void visit(Filter_Enclosing_Scopes const* n) = 0;
		virtual void visit(Filter_Enclosing_Scopes const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Function
		 * \param  n Filter_Function
		 */
		virtual void visit(Filter_Function const* n) = 0;
		virtual void visit(Filter_Function const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Intersection
		 * \param  n Filter_Intersection
		 */
		virtual void visit(Filter_Intersection const* n) = 0;
		virtual void visit(Filter_Intersection const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Setminus
		 * \param  n Filter_Setminus
		 */
		virtual void visit(Filter_Setminus const* n) = 0;
		virtual void visit(Filter_Setminus const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Union
		 * \param  n Filter_Union
		 */
		virtual void visit(Filter_Union const* n) = 0;
		virtual void visit(Filter_Union const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PM_Alternative
		 * \param  n PM_Alternative
		 */
		virtual void visit(PM_Alternative const* n) = 0;
		virtual void visit(PM_Alternative const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PM_Concat
		 * \param  n PM_Concat
		 */
		virtual void visit(PM_Concat const* n) = 0;
		virtual void visit(PM_Concat const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PM_Filter_Adapter
		 * \param  n PM_Filter_Adapter
		 */
		virtual void visit(PM_Filter_Adapter const* n) = 0;
		virtual void visit(PM_Filter_Adapter const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PM_Repeat
		 * \param  n PM_Repeat
		 */
		virtual void visit(PM_Repeat const* n) = 0;
		virtual void visit(PM_Repeat const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Path_Monitor_Expr
		 * \param  n Path_Monitor
		 */
		virtual void visit(Path_Monitor_Expr const* n) = 0;
		virtual void visit(Path_Monitor_Expr const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Pathcov
		 * \param  n Pathcov
		 */
		virtual void visit(Pathcov const* n) = 0;
		virtual void visit(Pathcov const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Predicate
		 * \param  n Predicate
		 */
		virtual void visit(Predicate const* n) = 0;
		virtual void visit(Predicate const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Query
		 * \param  n Query
		 */
		virtual void visit(Query const* n) = 0;
		virtual void visit(Query const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Statecov
		 * \param  n Statecov
		 */
		virtual void visit(Statecov const* n) = 0;
		virtual void visit(Statecov const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::TGS_Intersection
		 * \param  n TGS_Intersection
		 */
		virtual void visit(TGS_Intersection const* n) = 0;
		virtual void visit(TGS_Intersection const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::TGS_Setminus
		 * \param  n TGS_Setminus
		 */
		virtual void visit(TGS_Setminus const* n) = 0;
		virtual void visit(TGS_Setminus const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::TGS_Union
		 * \param  n TGS_Union
		 */
		virtual void visit(TGS_Union const* n) = 0;
		virtual void visit(TGS_Union const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Test_Goal_Sequence
		 * \param  n Test_Goal_Sequence
		 */
		virtual void visit(Test_Goal_Sequence const* n) = 0;
		virtual void visit(Test_Goal_Sequence const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Test_Goal_Set
		 * \param  n Test_Goal_Set
		 */
		virtual void visit(Test_Goal_Set const* n) = 0;
		virtual void visit(Test_Goal_Set const* n) const = 0;
		/*! \} */
};

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__AST_VISITOR_HPP */

