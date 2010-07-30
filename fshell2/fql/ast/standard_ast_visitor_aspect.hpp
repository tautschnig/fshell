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

#ifndef FSHELL2__FQL__AST__STANDARD_AST_VISITOR_ASPECT_HPP
#define FSHELL2__FQL__AST__STANDARD_AST_VISITOR_ASPECT_HPP

/*! \file fshell2/fql/ast/standard_ast_visitor_aspect.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Apr 22 00:19:14 CEST 2009 
 */

#include <fshell2/config/config.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class CP_Alternative;
class CP_Concat;
class Coverage_Pattern_Expr;
class Depcov;
class ECP_Atom;
class Edgecov;
class FQL_Node;
class Filter_Compose;
class Filter_Expr;
class Filter_Function;
class Filter_Intersection;
class Filter_Setminus;
class Filter_Union;
class Nodecov;
class PP_Alternative;
class PP_Concat;
class Path_Pattern_Expr;
class Pathcov;
class Predicate;
class Quote;
class Query;
class Repeat;
class Transform_Pred;

/*! \brief Default implementation of an AST visitor
 *
 * May be put in the inheritance chain of an AST visitor to provide (empty)
 * default implementations of all methods
 */
template <typename INTERFACE>
class Standard_AST_Visitor_Aspect : public INTERFACE
{
	public:
		/*! \brief Destructor
		*/
		virtual ~Standard_AST_Visitor_Aspect();

		/*! \{
		 * \brief Visit a @ref fshell2::fql::CP_Alternative
		 * \param  n CP_Alternative
		 */
		virtual void visit(CP_Alternative const* n);
		virtual void visit(CP_Alternative const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::CP_Concat
		 * \param  n CP_Concat
		 */
		virtual void visit(CP_Concat const* n);
		virtual void visit(CP_Concat const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Coverage_Pattern_Expr
		 * \param  n Coverage_Pattern_Expr
		 */
		virtual void visit(Coverage_Pattern_Expr const* n);
		virtual void visit(Coverage_Pattern_Expr const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Depcov
		 * \param  n Depcov
		 */
		virtual void visit(Depcov const* n);
		virtual void visit(Depcov const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::ECP_Atom
		 * \param  n ECP_Atom
		 */
		virtual void visit(ECP_Atom const* n);
		virtual void visit(ECP_Atom const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Edgecov
		 * \param  n Edgecov
		 */
		virtual void visit(Edgecov const* n);
		virtual void visit(Edgecov const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::FQL_Node
		 * \param  n FQL_Node
		 */
		virtual void visit(FQL_Node const* n);
		virtual void visit(FQL_Node const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Compose
		 * \param  n Filter_Compose
		 */
		virtual void visit(Filter_Compose const* n);
		virtual void visit(Filter_Compose const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Expr
		 * \param  n Filter_Expr
		 */
		virtual void visit(Filter_Expr const* n);
		virtual void visit(Filter_Expr const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Function
		 * \param  n Filter_Function
		 */
		virtual void visit(Filter_Function const* n);
		virtual void visit(Filter_Function const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Intersection
		 * \param  n Filter_Intersection
		 */
		virtual void visit(Filter_Intersection const* n);
		virtual void visit(Filter_Intersection const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Setminus
		 * \param  n Filter_Setminus
		 */
		virtual void visit(Filter_Setminus const* n);
		virtual void visit(Filter_Setminus const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Union
		 * \param  n Filter_Union
		 */
		virtual void visit(Filter_Union const* n);
		virtual void visit(Filter_Union const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Nodecov
		 * \param  n Nodecov
		 */
		virtual void visit(Nodecov const* n);
		virtual void visit(Nodecov const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PP_Alternative
		 * \param  n PP_Alternative
		 */
		virtual void visit(PP_Alternative const* n);
		virtual void visit(PP_Alternative const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PP_Concat
		 * \param  n PP_Concat
		 */
		virtual void visit(PP_Concat const* n);
		virtual void visit(PP_Concat const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Path_Pattern_Expr
		 * \param  n Path_Pattern_Expr
		 */
		virtual void visit(Path_Pattern_Expr const* n);
		virtual void visit(Path_Pattern_Expr const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Pathcov
		 * \param  n Pathcov
		 */
		virtual void visit(Pathcov const* n);
		virtual void visit(Pathcov const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Predicate
		 * \param  n Predicate
		 */
		virtual void visit(Predicate const* n);
		virtual void visit(Predicate const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Quote
		 * \param  n Quote
		 */
		virtual void visit(Quote const* n);
		virtual void visit(Quote const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Query
		 * \param  n Query
		 */
		virtual void visit(Query const* n);
		virtual void visit(Query const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Repeat
		 * \param  n Repeat
		 */
		virtual void visit(Repeat const* n);
		virtual void visit(Repeat const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Transform_Pred
		 * \param  n Transform_Pred
		 */
		virtual void visit(Transform_Pred const* n);
		virtual void visit(Transform_Pred const* n) const;
		/*! \} */

};

template <typename INTERFACE>
Standard_AST_Visitor_Aspect<INTERFACE>::~Standard_AST_Visitor_Aspect()
{
}

#define NOOP_VISIT( CL ) \
template <typename INTERFACE> \
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(CL const* n) \
{} \
\
template <typename INTERFACE> \
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(CL const* n) const\
{} \
DUMMY_FUNC

NOOP_VISIT(CP_Alternative);
NOOP_VISIT(CP_Concat);
NOOP_VISIT(Coverage_Pattern_Expr);
NOOP_VISIT(Depcov);
NOOP_VISIT(ECP_Atom);
NOOP_VISIT(Edgecov);
NOOP_VISIT(FQL_Node);
NOOP_VISIT(Filter_Compose);
NOOP_VISIT(Filter_Expr);
NOOP_VISIT(Filter_Function);
NOOP_VISIT(Filter_Intersection);
NOOP_VISIT(Filter_Setminus);
NOOP_VISIT(Filter_Union);
NOOP_VISIT(Nodecov);
NOOP_VISIT(PP_Alternative);
NOOP_VISIT(PP_Concat);
NOOP_VISIT(Path_Pattern_Expr);
NOOP_VISIT(Pathcov);
NOOP_VISIT(Predicate);
NOOP_VISIT(Quote);
NOOP_VISIT(Query);
NOOP_VISIT(Repeat);
NOOP_VISIT(Transform_Pred);

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__STANDARD_AST_VISITOR_ASPECT_HPP */

