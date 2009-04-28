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

class FQL_Node;
class Query;
class Test_Goal_Sequence;
class Test_Goal_Set;
class Restriction_Automaton;
class Abstraction;
class Filter;
class Predicate;
class TGS_Union;
class TGS_Intersection;
class TGS_Setminus;
class Edgecov;
class Pathcov;
class Filter_Identity;
class Filter_Complement;
class Filter_Union;
class Filter_Intersection;
class Filter_Setminus;
class Filter_Enclosing_Scopes;
class Primitive_Filter;

/*! \brief TODO
*/
class AST_Visitor
{
	public:
		/*! \brief Destructor
		*/
		virtual ~AST_Visitor();


		/*! \{
		 * \brief Visit a @ref fshell2::fql::FQL_Node
		 * \param  n FQL_Node
		 */
		virtual void visit(FQL_Node const* n) = 0;
		virtual void visit(FQL_Node const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Query
		 * \param  n Query
		 */
		virtual void visit(Query const* n) = 0;
		virtual void visit(Query const* n) const = 0;
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

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Restriction_Automaton
		 * \param  n Restriction_Automaton
		 */
		virtual void visit(Restriction_Automaton const* n) = 0;
		virtual void visit(Restriction_Automaton const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Abstraction
		 * \param  n Abstraction
		 */
		virtual void visit(Abstraction const* n) = 0;
		virtual void visit(Abstraction const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter
		 * \param  n Filter
		 */
		virtual void visit(Filter const* n) = 0;
		virtual void visit(Filter const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Predicate
		 * \param  n Predicate
		 */
		virtual void visit(Predicate const* n) = 0;
		virtual void visit(Predicate const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::TGS_Union
		 * \param  n TGS_Union
		 */
		virtual void visit(TGS_Union const* n) = 0;
		virtual void visit(TGS_Union const* n) const = 0;
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
		 * \brief Visit a @ref fshell2::fql::Edgecov
		 * \param  n Edgecov
		 */
		virtual void visit(Edgecov const* n) = 0;
		virtual void visit(Edgecov const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Pathcov
		 * \param  n Pathcov
		 */
		virtual void visit(Pathcov const* n) = 0;
		virtual void visit(Pathcov const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Identity
		 * \param  n Filter_Identity
		 */
		virtual void visit(Filter_Identity const* n) = 0;
		virtual void visit(Filter_Identity const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Complement
		 * \param  n Filter_Complement
		 */
		virtual void visit(Filter_Complement const* n) = 0;
		virtual void visit(Filter_Complement const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Union
		 * \param  n Filter_Union
		 */
		virtual void visit(Filter_Union const* n) = 0;
		virtual void visit(Filter_Union const* n) const = 0;
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
		 * \brief Visit a @ref fshell2::fql::Filter_Enclosing_Scopes
		 * \param  n Filter_Enclosing_Scopes
		 */
		virtual void visit(Filter_Enclosing_Scopes const* n) = 0;
		virtual void visit(Filter_Enclosing_Scopes const* n) const = 0;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Primitive_Filter
		 * \param  n Primitive_Filter
		 */
		virtual void visit(Primitive_Filter const* n) = 0;
		virtual void visit(Primitive_Filter const* n) const = 0;
		/*! \} */
};

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__AST_VISITOR_HPP */

