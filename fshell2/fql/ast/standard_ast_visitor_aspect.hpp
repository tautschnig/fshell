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

class Edgecov;
class FQL_Node;
class Filter;
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
class PM_Next;
class PM_Repeat;
class Path_Monitor;
class Pathcov;
class Predicate;
class Query;
class Statecov;
class TGS_Intersection;
class TGS_Setminus;
class TGS_Union;
class Test_Goal_Sequence;
class Test_Goal_Set;

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
		 * \brief Visit a @ref fshell2::fql::Filter
		 * \param  n Filter
		 */
		virtual void visit(Filter const* n);
		virtual void visit(Filter const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Compose
		 * \param  n Filter_Compose
		 */
		virtual void visit(Filter_Compose const* n);
		virtual void visit(Filter_Compose const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Complement
		 * \param  n Filter_Complement
		 */
		virtual void visit(Filter_Complement const* n);
		virtual void visit(Filter_Complement const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Enclosing_Scopes
		 * \param  n Filter_Enclosing_Scopes
		 */
		virtual void visit(Filter_Enclosing_Scopes const* n);
		virtual void visit(Filter_Enclosing_Scopes const* n) const;
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
		 * \brief Visit a @ref fshell2::fql::PM_Alternative
		 * \param  n PM_Alternative
		 */
		virtual void visit(PM_Alternative const* n);
		virtual void visit(PM_Alternative const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PM_Concat
		 * \param  n PM_Concat
		 */
		virtual void visit(PM_Concat const* n);
		virtual void visit(PM_Concat const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PM_Filter_Adapter
		 * \param  n PM_Filter_Adapter
		 */
		virtual void visit(PM_Filter_Adapter const* n);
		virtual void visit(PM_Filter_Adapter const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PM_Next
		 * \param  n PM_Next
		 */
		virtual void visit(PM_Next const* n);
		virtual void visit(PM_Next const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::PM_Repeat
		 * \param  n PM_Repeat
		 */
		virtual void visit(PM_Repeat const* n);
		virtual void visit(PM_Repeat const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Path_Monitor
		 * \param  n Path_Monitor
		 */
		virtual void visit(Path_Monitor const* n);
		virtual void visit(Path_Monitor const* n) const;
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
		 * \brief Visit a @ref fshell2::fql::Query
		 * \param  n Query
		 */
		virtual void visit(Query const* n);
		virtual void visit(Query const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Statecov
		 * \param  n Statecov
		 */
		virtual void visit(Statecov const* n);
		virtual void visit(Statecov const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::TGS_Intersection
		 * \param  n TGS_Intersection
		 */
		virtual void visit(TGS_Intersection const* n);
		virtual void visit(TGS_Intersection const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::TGS_Setminus
		 * \param  n TGS_Setminus
		 */
		virtual void visit(TGS_Setminus const* n);
		virtual void visit(TGS_Setminus const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::TGS_Union
		 * \param  n TGS_Union
		 */
		virtual void visit(TGS_Union const* n);
		virtual void visit(TGS_Union const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Test_Goal_Sequence
		 * \param  n Test_Goal_Sequence
		 */
		virtual void visit(Test_Goal_Sequence const* n);
		virtual void visit(Test_Goal_Sequence const* n) const;
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Test_Goal_Set
		 * \param  n Test_Goal_Set
		 */
		virtual void visit(Test_Goal_Set const* n);
		virtual void visit(Test_Goal_Set const* n) const;
		/*! \} */

};

template <typename INTERFACE>
Standard_AST_Visitor_Aspect<INTERFACE>::~Standard_AST_Visitor_Aspect()
{
}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Edgecov const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Edgecov const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(FQL_Node const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(FQL_Node const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Complement const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Complement const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Compose const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Compose const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Enclosing_Scopes const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Enclosing_Scopes const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Function const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Function const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Intersection const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Intersection const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Setminus const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Setminus const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Union const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Filter_Union const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Alternative const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Alternative const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Concat const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Concat const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Filter_Adapter const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Filter_Adapter const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Next const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Next const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Repeat const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(PM_Repeat const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Path_Monitor const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Path_Monitor const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Pathcov const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Pathcov const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Predicate const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Predicate const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Query const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Query const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Statecov const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Statecov const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(TGS_Intersection const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(TGS_Intersection const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(TGS_Setminus const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(TGS_Setminus const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(TGS_Union const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(TGS_Union const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Test_Goal_Sequence const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Test_Goal_Sequence const* n) const
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Test_Goal_Set const* n)
{}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit(Test_Goal_Set const* n) const
{}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__STANDARD_AST_VISITOR_ASPECT_HPP */

