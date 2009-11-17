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

#ifndef FSHELL2__FQL__AST__FQL_NODE_LT_VISITOR_HPP
#define FSHELL2__FQL__AST__FQL_NODE_LT_VISITOR_HPP

/*! \file fshell2/fql/ast/fql_node_lt_visitor.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Apr 26 18:15:23 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_complement.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_enclosing_scopes.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pm_alternative.hpp>
#include <fshell2/fql/ast/pm_concat.hpp>
#include <fshell2/fql/ast/pm_filter_adapter.hpp>
#include <fshell2/fql/ast/pm_postcondition.hpp>
#include <fshell2/fql/ast/pm_precondition.hpp>
#include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_postcondition.hpp>
#include <fshell2/fql/ast/tgs_precondition.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>

#if AUTOMATA_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#endif

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class FQL_Node_Lt_Visitor : public AST_Visitor
{
	/*! \copydoc doc_self
	*/
	typedef FQL_Node_Lt_Visitor Self;

	public:
		/*! \brief Destructor
		*/
		virtual ~FQL_Node_Lt_Visitor();
		
		/*! \brief Constructor
		*/
		FQL_Node_Lt_Visitor();

		template <typename A, typename B>
		inline bool cmp_lt(A const* a, B const* b);

		/*! \brief Compare @a a and @a b as a functor
		 *
		 * \return cmp_lt( a, b )
		*/
		bool operator()(FQL_Node const* a, FQL_Node const* b);
		
		/*! \brief Compare @a a and @a b as a functor
		 *
		 * \return cmp_lt( &a, &b )
		*/
		bool operator()(FQL_Node const& a, FQL_Node const& b);
	
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

	protected:
		/*! \brief Compare an unidentified AST node to some other node of an AST
		*/
		template <typename T>
		class Compare_To_Visitor : public AST_Visitor
		{
			private:
				/*! \copydoc doc_self
				*/
				typedef Compare_To_Visitor< T > Self;
				/*! \brief The real compare object
				*/
				FQL_Node_Lt_Visitor & m_comparison;
				/*! \brief The other node
				*/
				T const * m_other_node;
				/*! \brief Store the result of the comparison
				*/
				bool m_inner_lt;

			public:
				/*! \brief Constructor
				 * \param cmp Comparer
				 * \param other The node to compare to
				*/
				Compare_To_Visitor( FQL_Node_Lt_Visitor & cmp, T const * other );

				/*! \brief Destructor
				*/
				virtual ~Compare_To_Visitor();

				inline bool is_lt() const;

				/*! \{ 
				 * \brief Visit a node of the respective type and call the appropriate
				 * compare function
				 * \param n AST node
				*/
				virtual void visit(Edgecov const* n);
				virtual void visit(Filter_Complement const* n);
				virtual void visit(Filter_Compose const* n);
				virtual void visit(Filter_Enclosing_Scopes const* n);
				virtual void visit(Filter_Function const* n);
				virtual void visit(Filter_Intersection const* n);
				virtual void visit(Filter_Setminus const* n);
				virtual void visit(Filter_Union const* n);
				virtual void visit(PM_Alternative const* n);
				virtual void visit(PM_Concat const* n);
				virtual void visit(PM_Filter_Adapter const* n);
				virtual void visit(PM_Postcondition const* n);
				virtual void visit(PM_Precondition const* n);
				virtual void visit(PM_Repeat const* n);
				virtual void visit(Pathcov const* n);
				virtual void visit(Predicate const* n);
				virtual void visit(Query const* n);
				virtual void visit(Statecov const* n);
				virtual void visit(TGS_Intersection const* n);
				virtual void visit(TGS_Postcondition const* n);
				virtual void visit(TGS_Precondition const* n);
				virtual void visit(TGS_Setminus const* n);
				virtual void visit(TGS_Union const* n);
				virtual void visit(Test_Goal_Sequence const* n);
				/*! \} */

			private:
				virtual void visit(Edgecov const* n) const;
				virtual void visit(FQL_Node const* n);
				virtual void visit(FQL_Node const* n) const;
				virtual void visit(Filter_Expr const* n);
				virtual void visit(Filter_Expr const* n) const;
				virtual void visit(Filter_Complement const* n) const;
				virtual void visit(Filter_Compose const* n) const;
				virtual void visit(Filter_Enclosing_Scopes const* n) const;
				virtual void visit(Filter_Function const* n) const;
				virtual void visit(Filter_Intersection const* n) const;
				virtual void visit(Filter_Setminus const* n) const;
				virtual void visit(Filter_Union const* n) const;
				virtual void visit(PM_Alternative const* n) const;
				virtual void visit(PM_Concat const* n) const;
				virtual void visit(PM_Filter_Adapter const* n) const;
				virtual void visit(PM_Postcondition const* n) const;
				virtual void visit(PM_Precondition const* n) const;
				virtual void visit(PM_Repeat const* n) const;
				virtual void visit(Path_Monitor_Expr const* n);
				virtual void visit(Path_Monitor_Expr const* n) const;
				virtual void visit(Pathcov const* n) const;
				virtual void visit(Predicate const* n) const;
				virtual void visit(Query const* n) const;
				virtual void visit(Statecov const* n) const;
				virtual void visit(TGS_Intersection const* n) const;
				virtual void visit(TGS_Postcondition const* n) const;
				virtual void visit(TGS_Precondition const* n) const;
				virtual void visit(TGS_Setminus const* n) const;
				virtual void visit(TGS_Union const* n) const;
				virtual void visit(Test_Goal_Sequence const* n) const;
				virtual void visit(Test_Goal_Set const* n);
				virtual void visit(Test_Goal_Set const* n) const;

				/*! \copydoc copy_constructor
				*/
				Compare_To_Visitor( Self const& rhs );
				/*! \copydoc assignment_op
				*/
				Self& operator=( Self const& rhs );
		};


	private:
		FQL_Node const * m_other;
		bool m_lt;

		virtual void visit(Edgecov const* n) const;
		virtual void visit(FQL_Node const* n);
		virtual void visit(FQL_Node const* n) const;
		virtual void visit(Filter_Expr const* n);
		virtual void visit(Filter_Expr const* n) const;
		virtual void visit(Filter_Complement const* n) const;
		virtual void visit(Filter_Compose const* n) const;
		virtual void visit(Filter_Enclosing_Scopes const* n) const;
		virtual void visit(Filter_Function const* n) const;
		virtual void visit(Filter_Intersection const* n) const;
		virtual void visit(Filter_Setminus const* n) const;
		virtual void visit(Filter_Union const* n) const;
		virtual void visit(PM_Alternative const* n) const;
		virtual void visit(PM_Concat const* n) const;
		virtual void visit(PM_Filter_Adapter const* n) const;
		virtual void visit(PM_Postcondition const* n) const;
		virtual void visit(PM_Precondition const* n) const;
		virtual void visit(PM_Repeat const* n) const;
		virtual void visit(Path_Monitor_Expr const* n);
		virtual void visit(Path_Monitor_Expr const* n) const;
		virtual void visit(Pathcov const* n) const;
		virtual void visit(Predicate const* n) const;
		virtual void visit(Query const* n) const;
		virtual void visit(Statecov const* n) const;
		virtual void visit(TGS_Intersection const* n) const;
		virtual void visit(TGS_Postcondition const* n) const;
		virtual void visit(TGS_Precondition const* n) const;
		virtual void visit(TGS_Setminus const* n) const;
		virtual void visit(TGS_Union const* n) const;
		virtual void visit(Test_Goal_Sequence const* n) const;
		virtual void visit(Test_Goal_Set const* n);
		virtual void visit(Test_Goal_Set const* n) const;

		/*! \copydoc copy_constructor
		*/
		FQL_Node_Lt_Visitor( Self const& rhs );

		/*! \copydoc assignment_op
		*/
		Self& operator=( Self const& rhs );
};

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(FQL_Node const* a, FQL_Node const* b)
{
	if (a == b) return false;
	m_other = b;
	a->accept(this);
	return m_lt;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Expr const* a, Filter_Expr const* b)
{
	if (a == b) return false;
	m_other = b;
	a->accept(this);
	return m_lt;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Path_Monitor_Expr const* a, Path_Monitor_Expr const* b)
{
	if (a == b) return false;
	m_other = b;
	a->accept(this);
	return m_lt;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Set const* a, Test_Goal_Set const* b)
{
	if (a == b) return false;
	m_other = b;
	a->accept(this);
	return m_lt;
}

template <typename T>
FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::Compare_To_Visitor(
		FQL_Node_Lt_Visitor & cmp, T const * other )
: m_comparison( cmp ), m_other_node( other ), m_inner_lt( false )
{
}
				
template <typename T>
FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::~Compare_To_Visitor()
{
}
				
template <typename T>
inline bool FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::is_lt() const
{
	return m_inner_lt;
}


/*! \brief Common code for any type of comparison
*/
#define LT_COMPARISON_BUILDER( CL ) \
template<> \
inline bool FQL_Node_Lt_Visitor::cmp_lt( CL const* a, FQL_Node const* b ) \
{ \
	Compare_To_Visitor< CL > v( *this, a ); \
	b->accept( &v ); \
	return v.is_lt(); \
} DUMMY_FUNC

#define LT_COMPARISON_TRUE( CL1, CL2 ) \
template<> \
inline bool FQL_Node_Lt_Visitor::cmp_lt( CL1 const* a, CL2 const* b ) \
{ \
	return true; \
} DUMMY_FUNC

#define LT_COMPARISON_FALSE( CL1, CL2 ) \
template<> \
inline bool FQL_Node_Lt_Visitor::cmp_lt( CL1 const* a, CL2 const* b ) \
{ \
	return false; \
} DUMMY_FUNC

#define LT_VISIT_BUILDER_CPP_BASE( CL, CODE ) \
void FQL_Node_Lt_Visitor::visit( CL const* a ) \
{ \
	CODE; \
} \
\
void FQL_Node_Lt_Visitor::visit( CL const* a ) const \
{ \
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false); \
} \
\
template <typename T> \
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit( CL const* a ) const \
{ \
	FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false); \
} DUMMY_FUNC

#define LT_VISIT_BUILDER_1( CL ) \
	LT_VISIT_BUILDER_CPP_BASE(CL, m_lt = cmp_lt( a, m_other ))

#define LT_VISIT_BUILDER_TPL( CL ) \
template <typename T> \
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit( CL const* n ) \
{ \
	m_inner_lt = m_comparison.cmp_lt< T, CL >( m_other_node, n ); \
} DUMMY_FUNC

/* we use a simple lexicographic order (using class names)
Edgecov
Filter_Complement
Filter_Compose
Filter_Enclosing_Scopes
Filter_Function
Filter_Intersection
Filter_Setminus
Filter_Union
PM_Alternative
PM_Concat
PM_Filter_Adapter
PM_Postcondition
PM_Precondition
PM_Repeat
Pathcov
Predicate
Query
Statecov
TGS_Intersection
TGS_Postcondition
TGS_Precondition
TGS_Setminus
TGS_Union
Test_Goal_Sequence
*/

/*
Original scripted build:
for c in Edgecov Filter_Complement Filter_Compose Filter_Enclosing_Scopes Filter_Function Filter_Intersection Filter_Setminus Filter_Union PM_Alternative PM_Concat PM_Filter_Adapter PM_Repeat Pathcov Predicate Query Statecov TGS_Intersection TGS_Setminus TGS_Union Test_Goal_Sequence ; do res="false" ; for c2 in Edgecov Filter_Complement Filter_Compose Filter_Enclosing_Scopes Filter_Function Filter_Intersection Filter_Setminus Filter_Union PM_Alternative PM_Concat PM_Filter_Adapter PM_Repeat Pathcov Predicate Query Statecov TGS_Intersection TGS_Setminus TGS_Union Test_Goal_Sequence ; do echo -e "template<>\ninline bool FQL_Node_Lt_Visitor::cmp_lt($c const* a, $c2 const* b)\n{"; if [ "$c" = "$c2" ] ; then res="true" ; else echo -e "\treturn $res;" ; fi ; echo -e "}\n" ; done ; echo -e "LT_COMPARISON_BUILDER($c);\n" ; done
*/

LT_COMPARISON_FALSE(Predicate, Edgecov);
LT_COMPARISON_FALSE(Predicate, Filter_Complement);
LT_COMPARISON_FALSE(Predicate, Filter_Compose);
LT_COMPARISON_FALSE(Predicate, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(Predicate, Filter_Function);
LT_COMPARISON_FALSE(Predicate, Filter_Intersection);
LT_COMPARISON_FALSE(Predicate, Filter_Setminus);
LT_COMPARISON_FALSE(Predicate, Filter_Union);
LT_COMPARISON_FALSE(Predicate, PM_Alternative);
LT_COMPARISON_FALSE(Predicate, PM_Concat);
LT_COMPARISON_FALSE(Predicate, PM_Filter_Adapter);
LT_COMPARISON_FALSE(Predicate, PM_Postcondition);
LT_COMPARISON_FALSE(Predicate, PM_Precondition);
LT_COMPARISON_FALSE(Predicate, PM_Repeat);
LT_COMPARISON_FALSE(Predicate, Pathcov);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Predicate const* b)
{
	// simple pointer comparison
	return a->get_expr() < b->get_expr();
}

LT_COMPARISON_TRUE(Predicate, Query);
LT_COMPARISON_TRUE(Predicate, Statecov);
LT_COMPARISON_TRUE(Predicate, TGS_Intersection);
LT_COMPARISON_TRUE(Predicate, TGS_Postcondition);
LT_COMPARISON_TRUE(Predicate, TGS_Precondition);
LT_COMPARISON_TRUE(Predicate, TGS_Setminus);
LT_COMPARISON_TRUE(Predicate, TGS_Union);
LT_COMPARISON_TRUE(Predicate, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Predicate);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Edgecov const* b)
{
	if (cmp_lt(a->get_filter_expr(), b->get_filter_expr())) return true;
	if (cmp_lt(b->get_filter_expr(), a->get_filter_expr())) return false;

	if (!b->get_predicates()) return false;
	if (!a->get_predicates()) return true;
	
	if (a->get_predicates()->size() != b->get_predicates()->size())
		return a->get_predicates()->size() < b->get_predicates()->size();
	for (Predicate::preds_t::const_iterator itera(a->get_predicates()->begin()),
			iterb(b->get_predicates()->begin()); itera != a->get_predicates()->end();
			++itera, ++iterb) {
		if (*itera == *iterb) continue;
		if (cmp_lt(*itera, *iterb)) return true;
		if (cmp_lt(*iterb, *itera)) return false;
	}
	return false;
}

LT_COMPARISON_TRUE(Edgecov, Filter_Complement);
LT_COMPARISON_TRUE(Edgecov, Filter_Compose);
LT_COMPARISON_TRUE(Edgecov, Filter_Enclosing_Scopes);
LT_COMPARISON_TRUE(Edgecov, Filter_Function);
LT_COMPARISON_TRUE(Edgecov, Filter_Intersection);
LT_COMPARISON_TRUE(Edgecov, Filter_Setminus);
LT_COMPARISON_TRUE(Edgecov, Filter_Union);
LT_COMPARISON_TRUE(Edgecov, PM_Alternative);
LT_COMPARISON_TRUE(Edgecov, PM_Concat);
LT_COMPARISON_TRUE(Edgecov, PM_Filter_Adapter);
LT_COMPARISON_TRUE(Edgecov, PM_Postcondition);
LT_COMPARISON_TRUE(Edgecov, PM_Precondition);
LT_COMPARISON_TRUE(Edgecov, PM_Repeat);
LT_COMPARISON_TRUE(Edgecov, Pathcov);
LT_COMPARISON_TRUE(Edgecov, Predicate);
LT_COMPARISON_TRUE(Edgecov, Query);
LT_COMPARISON_TRUE(Edgecov, Statecov);
LT_COMPARISON_TRUE(Edgecov, TGS_Intersection);
LT_COMPARISON_TRUE(Edgecov, TGS_Postcondition);
LT_COMPARISON_TRUE(Edgecov, TGS_Precondition);
LT_COMPARISON_TRUE(Edgecov, TGS_Setminus);
LT_COMPARISON_TRUE(Edgecov, TGS_Union);
LT_COMPARISON_TRUE(Edgecov, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Edgecov);

LT_COMPARISON_FALSE(Filter_Complement, Edgecov);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Filter_Complement const* b)
{
	return cmp_lt(b->get_filter_expr(), a->get_filter_expr());
}

LT_COMPARISON_TRUE(Filter_Complement, Filter_Compose);
LT_COMPARISON_TRUE(Filter_Complement, Filter_Enclosing_Scopes);
LT_COMPARISON_TRUE(Filter_Complement, Filter_Function);
LT_COMPARISON_TRUE(Filter_Complement, Filter_Intersection);
LT_COMPARISON_TRUE(Filter_Complement, Filter_Setminus);
LT_COMPARISON_TRUE(Filter_Complement, Filter_Union);
LT_COMPARISON_TRUE(Filter_Complement, PM_Alternative);
LT_COMPARISON_TRUE(Filter_Complement, PM_Concat);
LT_COMPARISON_TRUE(Filter_Complement, PM_Filter_Adapter);
LT_COMPARISON_TRUE(Filter_Complement, PM_Postcondition);
LT_COMPARISON_TRUE(Filter_Complement, PM_Precondition);
LT_COMPARISON_TRUE(Filter_Complement, PM_Repeat);
LT_COMPARISON_TRUE(Filter_Complement, Pathcov);
LT_COMPARISON_TRUE(Filter_Complement, Predicate);
LT_COMPARISON_TRUE(Filter_Complement, Query);
LT_COMPARISON_TRUE(Filter_Complement, Statecov);
LT_COMPARISON_TRUE(Filter_Complement, TGS_Intersection);
LT_COMPARISON_TRUE(Filter_Complement, TGS_Postcondition);
LT_COMPARISON_TRUE(Filter_Complement, TGS_Precondition);
LT_COMPARISON_TRUE(Filter_Complement, TGS_Setminus);
LT_COMPARISON_TRUE(Filter_Complement, TGS_Union);
LT_COMPARISON_TRUE(Filter_Complement, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Filter_Complement);

LT_COMPARISON_FALSE(Filter_Compose, Edgecov);
LT_COMPARISON_FALSE(Filter_Compose, Filter_Complement);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Compose const* a, Filter_Compose const* b)
{
	if (cmp_lt(a->get_filter_expr_a(), b->get_filter_expr_a())) return true;
	if (cmp_lt(b->get_filter_expr_a(), a->get_filter_expr_a())) return false;

	return !cmp_lt(a->get_filter_expr_b(), b->get_filter_expr_b());
}

LT_COMPARISON_TRUE(Filter_Compose, Filter_Enclosing_Scopes);
LT_COMPARISON_TRUE(Filter_Compose, Filter_Function);
LT_COMPARISON_TRUE(Filter_Compose, Filter_Intersection);
LT_COMPARISON_TRUE(Filter_Compose, Filter_Setminus);
LT_COMPARISON_TRUE(Filter_Compose, Filter_Union);
LT_COMPARISON_TRUE(Filter_Compose, PM_Alternative);
LT_COMPARISON_TRUE(Filter_Compose, PM_Concat);
LT_COMPARISON_TRUE(Filter_Compose, PM_Filter_Adapter);
LT_COMPARISON_TRUE(Filter_Compose, PM_Postcondition);
LT_COMPARISON_TRUE(Filter_Compose, PM_Precondition);
LT_COMPARISON_TRUE(Filter_Compose, PM_Repeat);
LT_COMPARISON_TRUE(Filter_Compose, Pathcov);
LT_COMPARISON_TRUE(Filter_Compose, Predicate);
LT_COMPARISON_TRUE(Filter_Compose, Query);
LT_COMPARISON_TRUE(Filter_Compose, Statecov);
LT_COMPARISON_TRUE(Filter_Compose, TGS_Intersection);
LT_COMPARISON_TRUE(Filter_Compose, TGS_Postcondition);
LT_COMPARISON_TRUE(Filter_Compose, TGS_Precondition);
LT_COMPARISON_TRUE(Filter_Compose, TGS_Setminus);
LT_COMPARISON_TRUE(Filter_Compose, TGS_Union);
LT_COMPARISON_TRUE(Filter_Compose, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Filter_Compose);

LT_COMPARISON_FALSE(Filter_Enclosing_Scopes, Edgecov);
LT_COMPARISON_FALSE(Filter_Enclosing_Scopes, Filter_Complement);
LT_COMPARISON_FALSE(Filter_Enclosing_Scopes, Filter_Compose);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Filter_Enclosing_Scopes const* b)
{
	return cmp_lt(a->get_filter_expr(), b->get_filter_expr());
}

LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, Filter_Function);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, Filter_Intersection);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, Filter_Setminus);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, Filter_Union);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, PM_Alternative);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, PM_Concat);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, PM_Filter_Adapter);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, PM_Postcondition);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, PM_Precondition);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, PM_Repeat);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, Pathcov);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, Predicate);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, Query);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, Statecov);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, TGS_Intersection);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, TGS_Postcondition);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, TGS_Precondition);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, TGS_Setminus);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, TGS_Union);
LT_COMPARISON_TRUE(Filter_Enclosing_Scopes, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(Filter_Function, Edgecov);
LT_COMPARISON_FALSE(Filter_Function, Filter_Complement);
LT_COMPARISON_FALSE(Filter_Function, Filter_Compose);
LT_COMPARISON_FALSE(Filter_Function, Filter_Enclosing_Scopes);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Function const* a, Filter_Function const* b)
{
	if (a->get_filter_type() != b->get_filter_type())
		return a->get_filter_type() < b->get_filter_type();

	switch (a->get_filter_type()) {
		case F_IDENTITY:
		case F_CALLS:
		case F_BASICBLOCKENTRY:
		case F_CONDITIONEDGE:
		case F_DECISIONEDGE:
		case F_CONDITIONGRAPH:
			return false;
		case F_LINE:
			return a->get_int_arg<F_LINE>() < b->get_int_arg<F_LINE>();
		case F_COLUMN:
			return a->get_int_arg<F_COLUMN>() < b->get_int_arg<F_COLUMN>();
		case F_FILE:
			return a->get_string_arg<F_FILE>() < b->get_string_arg<F_FILE>();
		case F_FUNC:
			return a->get_string_arg<F_FUNC>() < b->get_string_arg<F_FUNC>();
		case F_LABEL:
			return a->get_string_arg<F_LABEL>() < b->get_string_arg<F_LABEL>();
		case F_CALL:
			return a->get_string_arg<F_CALL>() < b->get_string_arg<F_CALL>();
		case F_ENTRY:
			return a->get_string_arg<F_ENTRY>() < b->get_string_arg<F_ENTRY>();
		case F_EXIT:
			return a->get_string_arg<F_EXIT>() < b->get_string_arg<F_EXIT>();
		case F_EXPR:
			return a->get_string_arg<F_EXPR>() < b->get_string_arg<F_EXPR>();
		case F_REGEXP:
			return a->get_string_arg<F_REGEXP>() < b->get_string_arg<F_REGEXP>();
		case F_DEF:
			return a->get_string_arg<F_DEF>() < b->get_string_arg<F_DEF>();
		case F_USE:
			return a->get_string_arg<F_USE>() < b->get_string_arg<F_USE>();
		case F_STMTTYPE:
			return a->get_int_arg<F_STMTTYPE>() < b->get_int_arg<F_STMTTYPE>();
	}

	return false;
}

LT_COMPARISON_TRUE(Filter_Function, Filter_Intersection);
LT_COMPARISON_TRUE(Filter_Function, Filter_Setminus);
LT_COMPARISON_TRUE(Filter_Function, Filter_Union);
LT_COMPARISON_TRUE(Filter_Function, PM_Alternative);
LT_COMPARISON_TRUE(Filter_Function, PM_Concat);
LT_COMPARISON_TRUE(Filter_Function, PM_Filter_Adapter);
LT_COMPARISON_TRUE(Filter_Function, PM_Postcondition);
LT_COMPARISON_TRUE(Filter_Function, PM_Precondition);
LT_COMPARISON_TRUE(Filter_Function, PM_Repeat);
LT_COMPARISON_TRUE(Filter_Function, Pathcov);
LT_COMPARISON_TRUE(Filter_Function, Predicate);
LT_COMPARISON_TRUE(Filter_Function, Query);
LT_COMPARISON_TRUE(Filter_Function, Statecov);
LT_COMPARISON_TRUE(Filter_Function, TGS_Intersection);
LT_COMPARISON_TRUE(Filter_Function, TGS_Postcondition);
LT_COMPARISON_TRUE(Filter_Function, TGS_Precondition);
LT_COMPARISON_TRUE(Filter_Function, TGS_Setminus);
LT_COMPARISON_TRUE(Filter_Function, TGS_Union);
LT_COMPARISON_TRUE(Filter_Function, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Filter_Function);

LT_COMPARISON_FALSE(Filter_Intersection, Edgecov);
LT_COMPARISON_FALSE(Filter_Intersection, Filter_Complement);
LT_COMPARISON_FALSE(Filter_Intersection, Filter_Compose);
LT_COMPARISON_FALSE(Filter_Intersection, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(Filter_Intersection, Filter_Function);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Filter_Intersection const* b)
{
	if (cmp_lt(a->get_filter_expr_a(), b->get_filter_expr_a())) return true;
	if (cmp_lt(b->get_filter_expr_a(), a->get_filter_expr_a())) return false;

	return cmp_lt(a->get_filter_expr_b(), b->get_filter_expr_b());
}

LT_COMPARISON_TRUE(Filter_Intersection, Filter_Setminus);
LT_COMPARISON_TRUE(Filter_Intersection, Filter_Union);
LT_COMPARISON_TRUE(Filter_Intersection, PM_Alternative);
LT_COMPARISON_TRUE(Filter_Intersection, PM_Concat);
LT_COMPARISON_TRUE(Filter_Intersection, PM_Filter_Adapter);
LT_COMPARISON_TRUE(Filter_Intersection, PM_Postcondition);
LT_COMPARISON_TRUE(Filter_Intersection, PM_Precondition);
LT_COMPARISON_TRUE(Filter_Intersection, PM_Repeat);
LT_COMPARISON_TRUE(Filter_Intersection, Pathcov);
LT_COMPARISON_TRUE(Filter_Intersection, Predicate);
LT_COMPARISON_TRUE(Filter_Intersection, Query);
LT_COMPARISON_TRUE(Filter_Intersection, Statecov);
LT_COMPARISON_TRUE(Filter_Intersection, TGS_Intersection);
LT_COMPARISON_TRUE(Filter_Intersection, TGS_Postcondition);
LT_COMPARISON_TRUE(Filter_Intersection, TGS_Precondition);
LT_COMPARISON_TRUE(Filter_Intersection, TGS_Setminus);
LT_COMPARISON_TRUE(Filter_Intersection, TGS_Union);
LT_COMPARISON_TRUE(Filter_Intersection, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Filter_Intersection);

LT_COMPARISON_FALSE(Filter_Setminus, Edgecov);
LT_COMPARISON_FALSE(Filter_Setminus, Filter_Complement);
LT_COMPARISON_FALSE(Filter_Setminus, Filter_Compose);
LT_COMPARISON_FALSE(Filter_Setminus, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(Filter_Setminus, Filter_Function);
LT_COMPARISON_FALSE(Filter_Setminus, Filter_Intersection);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Filter_Setminus const* b)
{
	if (cmp_lt(a->get_filter_expr_a(), b->get_filter_expr_a())) return true;
	if (cmp_lt(b->get_filter_expr_a(), a->get_filter_expr_a())) return false;

	return !cmp_lt(a->get_filter_expr_b(), b->get_filter_expr_b());
}

LT_COMPARISON_TRUE(Filter_Setminus, Filter_Union);
LT_COMPARISON_TRUE(Filter_Setminus, PM_Alternative);
LT_COMPARISON_TRUE(Filter_Setminus, PM_Concat);
LT_COMPARISON_TRUE(Filter_Setminus, PM_Filter_Adapter);
LT_COMPARISON_TRUE(Filter_Setminus, PM_Postcondition);
LT_COMPARISON_TRUE(Filter_Setminus, PM_Precondition);
LT_COMPARISON_TRUE(Filter_Setminus, PM_Repeat);
LT_COMPARISON_TRUE(Filter_Setminus, Pathcov);
LT_COMPARISON_TRUE(Filter_Setminus, Predicate);
LT_COMPARISON_TRUE(Filter_Setminus, Query);
LT_COMPARISON_TRUE(Filter_Setminus, Statecov);
LT_COMPARISON_TRUE(Filter_Setminus, TGS_Intersection);
LT_COMPARISON_TRUE(Filter_Setminus, TGS_Postcondition);
LT_COMPARISON_TRUE(Filter_Setminus, TGS_Precondition);
LT_COMPARISON_TRUE(Filter_Setminus, TGS_Setminus);
LT_COMPARISON_TRUE(Filter_Setminus, TGS_Union);
LT_COMPARISON_TRUE(Filter_Setminus, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Filter_Setminus);

LT_COMPARISON_FALSE(Filter_Union, Edgecov);
LT_COMPARISON_FALSE(Filter_Union, Filter_Complement);
LT_COMPARISON_FALSE(Filter_Union, Filter_Compose);
LT_COMPARISON_FALSE(Filter_Union, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(Filter_Union, Filter_Function);
LT_COMPARISON_FALSE(Filter_Union, Filter_Intersection);
LT_COMPARISON_FALSE(Filter_Union, Filter_Setminus);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Filter_Union const* b)
{
	if (cmp_lt(a->get_filter_expr_a(), b->get_filter_expr_a())) return true;
	if (cmp_lt(b->get_filter_expr_a(), a->get_filter_expr_a())) return false;

	return cmp_lt(a->get_filter_expr_b(), b->get_filter_expr_b());
}

LT_COMPARISON_TRUE(Filter_Union, PM_Alternative);
LT_COMPARISON_TRUE(Filter_Union, PM_Concat);
LT_COMPARISON_TRUE(Filter_Union, PM_Filter_Adapter);
LT_COMPARISON_TRUE(Filter_Union, PM_Postcondition);
LT_COMPARISON_TRUE(Filter_Union, PM_Precondition);
LT_COMPARISON_TRUE(Filter_Union, PM_Repeat);
LT_COMPARISON_TRUE(Filter_Union, Pathcov);
LT_COMPARISON_TRUE(Filter_Union, Predicate);
LT_COMPARISON_TRUE(Filter_Union, Query);
LT_COMPARISON_TRUE(Filter_Union, Statecov);
LT_COMPARISON_TRUE(Filter_Union, TGS_Intersection);
LT_COMPARISON_TRUE(Filter_Union, TGS_Postcondition);
LT_COMPARISON_TRUE(Filter_Union, TGS_Precondition);
LT_COMPARISON_TRUE(Filter_Union, TGS_Setminus);
LT_COMPARISON_TRUE(Filter_Union, TGS_Union);
LT_COMPARISON_TRUE(Filter_Union, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Filter_Union);

LT_COMPARISON_FALSE(PM_Alternative, Edgecov);
LT_COMPARISON_FALSE(PM_Alternative, Filter_Complement);
LT_COMPARISON_FALSE(PM_Alternative, Filter_Compose);
LT_COMPARISON_FALSE(PM_Alternative, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(PM_Alternative, Filter_Function);
LT_COMPARISON_FALSE(PM_Alternative, Filter_Intersection);
LT_COMPARISON_FALSE(PM_Alternative, Filter_Setminus);
LT_COMPARISON_FALSE(PM_Alternative, Filter_Union);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(PM_Alternative const* a, PM_Alternative const* b)
{
	if (cmp_lt(a->get_path_monitor_expr_a(), b->get_path_monitor_expr_a())) return true;
	if (cmp_lt(b->get_path_monitor_expr_a(), a->get_path_monitor_expr_a())) return false;

	return cmp_lt(a->get_path_monitor_expr_b(), b->get_path_monitor_expr_b());
}

LT_COMPARISON_TRUE(PM_Alternative, PM_Concat);
LT_COMPARISON_TRUE(PM_Alternative, PM_Filter_Adapter);
LT_COMPARISON_TRUE(PM_Alternative, PM_Postcondition);
LT_COMPARISON_TRUE(PM_Alternative, PM_Precondition);
LT_COMPARISON_TRUE(PM_Alternative, PM_Repeat);
LT_COMPARISON_TRUE(PM_Alternative, Pathcov);
LT_COMPARISON_TRUE(PM_Alternative, Predicate);
LT_COMPARISON_TRUE(PM_Alternative, Query);
LT_COMPARISON_TRUE(PM_Alternative, Statecov);
LT_COMPARISON_TRUE(PM_Alternative, TGS_Intersection);
LT_COMPARISON_TRUE(PM_Alternative, TGS_Postcondition);
LT_COMPARISON_TRUE(PM_Alternative, TGS_Precondition);
LT_COMPARISON_TRUE(PM_Alternative, TGS_Setminus);
LT_COMPARISON_TRUE(PM_Alternative, TGS_Union);
LT_COMPARISON_TRUE(PM_Alternative, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(PM_Alternative);

LT_COMPARISON_FALSE(PM_Concat, Edgecov);
LT_COMPARISON_FALSE(PM_Concat, Filter_Complement);
LT_COMPARISON_FALSE(PM_Concat, Filter_Compose);
LT_COMPARISON_FALSE(PM_Concat, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(PM_Concat, Filter_Function);
LT_COMPARISON_FALSE(PM_Concat, Filter_Intersection);
LT_COMPARISON_FALSE(PM_Concat, Filter_Setminus);
LT_COMPARISON_FALSE(PM_Concat, Filter_Union);
LT_COMPARISON_FALSE(PM_Concat, PM_Alternative);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(PM_Concat const* a, PM_Concat const* b)
{
	if (cmp_lt(a->get_path_monitor_expr_a(), b->get_path_monitor_expr_a())) return true;
	if (cmp_lt(b->get_path_monitor_expr_a(), a->get_path_monitor_expr_a())) return false;

	return cmp_lt(a->get_path_monitor_expr_b(), b->get_path_monitor_expr_b());
}

LT_COMPARISON_TRUE(PM_Concat, PM_Filter_Adapter);
LT_COMPARISON_TRUE(PM_Concat, PM_Postcondition);
LT_COMPARISON_TRUE(PM_Concat, PM_Precondition);
LT_COMPARISON_TRUE(PM_Concat, PM_Repeat);
LT_COMPARISON_TRUE(PM_Concat, Pathcov);
LT_COMPARISON_TRUE(PM_Concat, Predicate);
LT_COMPARISON_TRUE(PM_Concat, Query);
LT_COMPARISON_TRUE(PM_Concat, Statecov);
LT_COMPARISON_TRUE(PM_Concat, TGS_Intersection);
LT_COMPARISON_TRUE(PM_Concat, TGS_Postcondition);
LT_COMPARISON_TRUE(PM_Concat, TGS_Precondition);
LT_COMPARISON_TRUE(PM_Concat, TGS_Setminus);
LT_COMPARISON_TRUE(PM_Concat, TGS_Union);
LT_COMPARISON_TRUE(PM_Concat, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(PM_Concat);

LT_COMPARISON_FALSE(PM_Filter_Adapter, Edgecov);
LT_COMPARISON_FALSE(PM_Filter_Adapter, Filter_Complement);
LT_COMPARISON_FALSE(PM_Filter_Adapter, Filter_Compose);
LT_COMPARISON_FALSE(PM_Filter_Adapter, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(PM_Filter_Adapter, Filter_Function);
LT_COMPARISON_FALSE(PM_Filter_Adapter, Filter_Intersection);
LT_COMPARISON_FALSE(PM_Filter_Adapter, Filter_Setminus);
LT_COMPARISON_FALSE(PM_Filter_Adapter, Filter_Union);
LT_COMPARISON_FALSE(PM_Filter_Adapter, PM_Alternative);
LT_COMPARISON_FALSE(PM_Filter_Adapter, PM_Concat);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(PM_Filter_Adapter const* a, PM_Filter_Adapter const* b)
{
	return cmp_lt(b->get_filter_expr(), a->get_filter_expr());
}

LT_COMPARISON_TRUE(PM_Filter_Adapter, PM_Postcondition);
LT_COMPARISON_TRUE(PM_Filter_Adapter, PM_Precondition);
LT_COMPARISON_TRUE(PM_Filter_Adapter, PM_Repeat);
LT_COMPARISON_TRUE(PM_Filter_Adapter, Pathcov);
LT_COMPARISON_TRUE(PM_Filter_Adapter, Predicate);
LT_COMPARISON_TRUE(PM_Filter_Adapter, Query);
LT_COMPARISON_TRUE(PM_Filter_Adapter, Statecov);
LT_COMPARISON_TRUE(PM_Filter_Adapter, TGS_Intersection);
LT_COMPARISON_TRUE(PM_Filter_Adapter, TGS_Postcondition);
LT_COMPARISON_TRUE(PM_Filter_Adapter, TGS_Precondition);
LT_COMPARISON_TRUE(PM_Filter_Adapter, TGS_Setminus);
LT_COMPARISON_TRUE(PM_Filter_Adapter, TGS_Union);
LT_COMPARISON_TRUE(PM_Filter_Adapter, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(PM_Filter_Adapter);

LT_COMPARISON_FALSE(PM_Postcondition, Edgecov);
LT_COMPARISON_FALSE(PM_Postcondition, Filter_Complement);
LT_COMPARISON_FALSE(PM_Postcondition, Filter_Compose);
LT_COMPARISON_FALSE(PM_Postcondition, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(PM_Postcondition, Filter_Function);
LT_COMPARISON_FALSE(PM_Postcondition, Filter_Intersection);
LT_COMPARISON_FALSE(PM_Postcondition, Filter_Setminus);
LT_COMPARISON_FALSE(PM_Postcondition, Filter_Union);
LT_COMPARISON_FALSE(PM_Postcondition, PM_Alternative);
LT_COMPARISON_FALSE(PM_Postcondition, PM_Concat);
LT_COMPARISON_FALSE(PM_Postcondition, PM_Filter_Adapter);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(PM_Postcondition const* a, PM_Postcondition const* b)
{
	if (cmp_lt(a->get_path_monitor_expr(), b->get_path_monitor_expr())) return true;
	if (cmp_lt(b->get_path_monitor_expr(), a->get_path_monitor_expr())) return false;

	return cmp_lt(b->get_predicate(), a->get_predicate());
}

LT_COMPARISON_TRUE(PM_Postcondition, PM_Precondition);
LT_COMPARISON_TRUE(PM_Postcondition, PM_Repeat);
LT_COMPARISON_TRUE(PM_Postcondition, Pathcov);
LT_COMPARISON_TRUE(PM_Postcondition, Predicate);
LT_COMPARISON_TRUE(PM_Postcondition, Query);
LT_COMPARISON_TRUE(PM_Postcondition, Statecov);
LT_COMPARISON_TRUE(PM_Postcondition, TGS_Intersection);
LT_COMPARISON_TRUE(PM_Postcondition, TGS_Postcondition);
LT_COMPARISON_TRUE(PM_Postcondition, TGS_Precondition);
LT_COMPARISON_TRUE(PM_Postcondition, TGS_Setminus);
LT_COMPARISON_TRUE(PM_Postcondition, TGS_Union);
LT_COMPARISON_TRUE(PM_Postcondition, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(PM_Postcondition);

LT_COMPARISON_FALSE(PM_Precondition, Edgecov);
LT_COMPARISON_FALSE(PM_Precondition, Filter_Complement);
LT_COMPARISON_FALSE(PM_Precondition, Filter_Compose);
LT_COMPARISON_FALSE(PM_Precondition, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(PM_Precondition, Filter_Function);
LT_COMPARISON_FALSE(PM_Precondition, Filter_Intersection);
LT_COMPARISON_FALSE(PM_Precondition, Filter_Setminus);
LT_COMPARISON_FALSE(PM_Precondition, Filter_Union);
LT_COMPARISON_FALSE(PM_Precondition, PM_Alternative);
LT_COMPARISON_FALSE(PM_Precondition, PM_Concat);
LT_COMPARISON_FALSE(PM_Precondition, PM_Filter_Adapter);
LT_COMPARISON_FALSE(PM_Precondition, PM_Postcondition);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(PM_Precondition const* a, PM_Precondition const* b)
{
	if (cmp_lt(a->get_predicate(), b->get_predicate())) return true;
	if (cmp_lt(b->get_predicate(), a->get_predicate())) return false;

	return cmp_lt(a->get_path_monitor_expr(), b->get_path_monitor_expr());;
}

LT_COMPARISON_TRUE(PM_Precondition, PM_Repeat);
LT_COMPARISON_TRUE(PM_Precondition, Pathcov);
LT_COMPARISON_TRUE(PM_Precondition, Predicate);
LT_COMPARISON_TRUE(PM_Precondition, Query);
LT_COMPARISON_TRUE(PM_Precondition, Statecov);
LT_COMPARISON_TRUE(PM_Precondition, TGS_Intersection);
LT_COMPARISON_TRUE(PM_Precondition, TGS_Postcondition);
LT_COMPARISON_TRUE(PM_Precondition, TGS_Precondition);
LT_COMPARISON_TRUE(PM_Precondition, TGS_Setminus);
LT_COMPARISON_TRUE(PM_Precondition, TGS_Union);
LT_COMPARISON_TRUE(PM_Precondition, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(PM_Precondition);

LT_COMPARISON_FALSE(PM_Repeat, Edgecov);
LT_COMPARISON_FALSE(PM_Repeat, Filter_Complement);
LT_COMPARISON_FALSE(PM_Repeat, Filter_Compose);
LT_COMPARISON_FALSE(PM_Repeat, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(PM_Repeat, Filter_Function);
LT_COMPARISON_FALSE(PM_Repeat, Filter_Intersection);
LT_COMPARISON_FALSE(PM_Repeat, Filter_Setminus);
LT_COMPARISON_FALSE(PM_Repeat, Filter_Union);
LT_COMPARISON_FALSE(PM_Repeat, PM_Alternative);
LT_COMPARISON_FALSE(PM_Repeat, PM_Concat);
LT_COMPARISON_FALSE(PM_Repeat, PM_Filter_Adapter);
LT_COMPARISON_FALSE(PM_Repeat, PM_Postcondition);
LT_COMPARISON_FALSE(PM_Repeat, PM_Precondition);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(PM_Repeat const* a, PM_Repeat const* b)
{
	if (cmp_lt(a->get_path_monitor_expr(), b->get_path_monitor_expr())) return true;
	if (cmp_lt(b->get_path_monitor_expr(), a->get_path_monitor_expr())) return false;

	if (a->get_lower_bound() != b->get_lower_bound())
		return a->get_lower_bound() < b->get_lower_bound();
	if (a->get_upper_bound() == -1) return false;
	if (b->get_upper_bound() == -1) return true;
	return a->get_upper_bound() < b->get_upper_bound();
}

LT_COMPARISON_TRUE(PM_Repeat, Pathcov);
LT_COMPARISON_TRUE(PM_Repeat, Predicate);
LT_COMPARISON_TRUE(PM_Repeat, Query);
LT_COMPARISON_TRUE(PM_Repeat, Statecov);
LT_COMPARISON_TRUE(PM_Repeat, TGS_Intersection);
LT_COMPARISON_TRUE(PM_Repeat, TGS_Postcondition);
LT_COMPARISON_TRUE(PM_Repeat, TGS_Precondition);
LT_COMPARISON_TRUE(PM_Repeat, TGS_Setminus);
LT_COMPARISON_TRUE(PM_Repeat, TGS_Union);
LT_COMPARISON_TRUE(PM_Repeat, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(PM_Repeat);

LT_COMPARISON_FALSE(Pathcov, Edgecov);
LT_COMPARISON_FALSE(Pathcov, Filter_Complement);
LT_COMPARISON_FALSE(Pathcov, Filter_Compose);
LT_COMPARISON_FALSE(Pathcov, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(Pathcov, Filter_Function);
LT_COMPARISON_FALSE(Pathcov, Filter_Intersection);
LT_COMPARISON_FALSE(Pathcov, Filter_Setminus);
LT_COMPARISON_FALSE(Pathcov, Filter_Union);
LT_COMPARISON_FALSE(Pathcov, PM_Alternative);
LT_COMPARISON_FALSE(Pathcov, PM_Concat);
LT_COMPARISON_FALSE(Pathcov, PM_Filter_Adapter);
LT_COMPARISON_FALSE(Pathcov, PM_Postcondition);
LT_COMPARISON_FALSE(Pathcov, PM_Precondition);
LT_COMPARISON_FALSE(Pathcov, PM_Repeat);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Pathcov const* b)
{
	if (cmp_lt(a->get_filter_expr(), b->get_filter_expr())) return true;
	if (cmp_lt(b->get_filter_expr(), a->get_filter_expr())) return false;
	
	if (a->get_bound() != b->get_bound()) return a->get_bound() < b->get_bound();

	if (!b->get_predicates()) return false;
	if (!a->get_predicates()) return true;
	
	if (a->get_predicates()->size() != b->get_predicates()->size())
		return a->get_predicates()->size() < b->get_predicates()->size();
	for (Predicate::preds_t::const_iterator itera(a->get_predicates()->begin()),
			iterb(b->get_predicates()->begin()); itera != a->get_predicates()->end();
			++itera, ++iterb) {
		if (*itera == *iterb) continue;
		if (cmp_lt(*itera, *iterb)) return true;
		if (cmp_lt(*iterb, *itera)) return false;
	}
	return false;
}

LT_COMPARISON_TRUE(Pathcov, Predicate);
LT_COMPARISON_TRUE(Pathcov, Query);
LT_COMPARISON_TRUE(Pathcov, Statecov);
LT_COMPARISON_TRUE(Pathcov, TGS_Intersection);
LT_COMPARISON_TRUE(Pathcov, TGS_Postcondition);
LT_COMPARISON_TRUE(Pathcov, TGS_Precondition);
LT_COMPARISON_TRUE(Pathcov, TGS_Setminus);
LT_COMPARISON_TRUE(Pathcov, TGS_Union);
LT_COMPARISON_TRUE(Pathcov, Test_Goal_Sequence);

LT_COMPARISON_BUILDER(Pathcov);

LT_COMPARISON_FALSE(Test_Goal_Sequence, Edgecov);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Filter_Complement);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Filter_Compose);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Filter_Function);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Filter_Intersection);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Filter_Setminus);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Filter_Union);
LT_COMPARISON_FALSE(Test_Goal_Sequence, PM_Alternative);
LT_COMPARISON_FALSE(Test_Goal_Sequence, PM_Concat);
LT_COMPARISON_FALSE(Test_Goal_Sequence, PM_Filter_Adapter);
LT_COMPARISON_FALSE(Test_Goal_Sequence, PM_Postcondition);
LT_COMPARISON_FALSE(Test_Goal_Sequence, PM_Precondition);
LT_COMPARISON_FALSE(Test_Goal_Sequence, PM_Repeat);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Pathcov);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Predicate);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Query);
LT_COMPARISON_FALSE(Test_Goal_Sequence, Statecov);
LT_COMPARISON_FALSE(Test_Goal_Sequence, TGS_Intersection);
LT_COMPARISON_FALSE(Test_Goal_Sequence, TGS_Postcondition);
LT_COMPARISON_FALSE(Test_Goal_Sequence, TGS_Precondition);
LT_COMPARISON_FALSE(Test_Goal_Sequence, TGS_Setminus);
LT_COMPARISON_FALSE(Test_Goal_Sequence, TGS_Union);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Test_Goal_Sequence const* b)
{
	if (a->get_sequence().size() != b->get_sequence().size())
		return a->get_sequence().size() < b->get_sequence().size();
	for (Test_Goal_Sequence::seq_t::const_iterator itera(a->get_sequence().begin()),
			iterb(b->get_sequence().begin()); itera != a->get_sequence().end();
			++itera, ++iterb) {
		if (itera->first != iterb->first) {
			if (itera->first && !iterb->first) return true;
			if (!itera->first && iterb->first) return false;

			if (cmp_lt(itera->first, iterb->first)) return true;
			if (cmp_lt(iterb->first, itera->first)) return false;
		}
			
		if (cmp_lt(itera->second, iterb->second)) return true;
		if (cmp_lt(iterb->second, itera->second)) return false;
	}
		
	if (a->get_suffix_monitor() != b->get_suffix_monitor()) {
		if (a->get_suffix_monitor() && !b->get_suffix_monitor()) return true;
		if (!a->get_suffix_monitor() && b->get_suffix_monitor()) return false;

		if (cmp_lt(a->get_suffix_monitor(), b->get_suffix_monitor())) return true;
		if (cmp_lt(b->get_suffix_monitor(), a->get_suffix_monitor())) return false;
	}

	return false;
}

LT_COMPARISON_FALSE(Query, Edgecov);
LT_COMPARISON_FALSE(Query, Filter_Complement);
LT_COMPARISON_FALSE(Query, Filter_Compose);
LT_COMPARISON_FALSE(Query, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(Query, Filter_Function);
LT_COMPARISON_FALSE(Query, Filter_Intersection);
LT_COMPARISON_FALSE(Query, Filter_Setminus);
LT_COMPARISON_FALSE(Query, Filter_Union);
LT_COMPARISON_FALSE(Query, PM_Alternative);
LT_COMPARISON_FALSE(Query, PM_Concat);
LT_COMPARISON_FALSE(Query, PM_Filter_Adapter);
LT_COMPARISON_FALSE(Query, PM_Postcondition);
LT_COMPARISON_FALSE(Query, PM_Precondition);
LT_COMPARISON_FALSE(Query, PM_Repeat);
LT_COMPARISON_FALSE(Query, Pathcov);
LT_COMPARISON_FALSE(Query, Predicate);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Query const* b)
{
	if (a->get_prefix() != b->get_prefix()) {
		if (a->get_prefix() && !b->get_prefix()) return true;
		if (!a->get_prefix() && b->get_prefix()) return false;

		if (cmp_lt(a->get_prefix(), b->get_prefix())) return true;
		if (cmp_lt(b->get_prefix(), a->get_prefix())) return false;
	}

	if (cmp_lt(a->get_cover(), b->get_cover())) return true;
	if (cmp_lt(b->get_cover(), a->get_cover())) return false;
	
	if (a->get_passing() != b->get_passing()) {
		if (a->get_passing() && !b->get_passing()) return true;
		if (!a->get_passing() && b->get_passing()) return false;

		if (cmp_lt(a->get_passing(), b->get_passing())) return true;
		if (cmp_lt(b->get_passing(), a->get_passing())) return false;
	}

	return false;
}

LT_COMPARISON_TRUE(Query, Statecov);
LT_COMPARISON_TRUE(Query, TGS_Intersection);
LT_COMPARISON_TRUE(Query, TGS_Postcondition);
LT_COMPARISON_TRUE(Query, TGS_Precondition);
LT_COMPARISON_TRUE(Query, TGS_Setminus);
LT_COMPARISON_TRUE(Query, TGS_Union);
LT_COMPARISON_TRUE(Query, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Query);

LT_COMPARISON_FALSE(Statecov, Edgecov);
LT_COMPARISON_FALSE(Statecov, Filter_Complement);
LT_COMPARISON_FALSE(Statecov, Filter_Compose);
LT_COMPARISON_FALSE(Statecov, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(Statecov, Filter_Function);
LT_COMPARISON_FALSE(Statecov, Filter_Intersection);
LT_COMPARISON_FALSE(Statecov, Filter_Setminus);
LT_COMPARISON_FALSE(Statecov, Filter_Union);
LT_COMPARISON_FALSE(Statecov, PM_Alternative);
LT_COMPARISON_FALSE(Statecov, PM_Concat);
LT_COMPARISON_FALSE(Statecov, PM_Filter_Adapter);
LT_COMPARISON_FALSE(Statecov, PM_Postcondition);
LT_COMPARISON_FALSE(Statecov, PM_Precondition);
LT_COMPARISON_FALSE(Statecov, PM_Repeat);
LT_COMPARISON_FALSE(Statecov, Pathcov);
LT_COMPARISON_FALSE(Statecov, Predicate);
LT_COMPARISON_FALSE(Statecov, Query);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Statecov const* a, Statecov const* b)
{
	if (cmp_lt(a->get_filter_expr(), b->get_filter_expr())) return true;
	if (cmp_lt(b->get_filter_expr(), a->get_filter_expr())) return false;

	if (!b->get_predicates()) return false;
	if (!a->get_predicates()) return true;
	
	if (a->get_predicates()->size() != b->get_predicates()->size())
		return a->get_predicates()->size() < b->get_predicates()->size();
	for (Predicate::preds_t::const_iterator itera(a->get_predicates()->begin()),
			iterb(b->get_predicates()->begin()); itera != a->get_predicates()->end();
			++itera, ++iterb) {
		if (*itera == *iterb) continue;
		if (cmp_lt(*itera, *iterb)) return true;
		if (cmp_lt(*iterb, *itera)) return false;
	}
	return false;
}

LT_COMPARISON_TRUE(Statecov, TGS_Intersection);
LT_COMPARISON_TRUE(Statecov, TGS_Postcondition);
LT_COMPARISON_TRUE(Statecov, TGS_Precondition);
LT_COMPARISON_TRUE(Statecov, TGS_Setminus);
LT_COMPARISON_TRUE(Statecov, TGS_Union);
LT_COMPARISON_TRUE(Statecov, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(Statecov);

LT_COMPARISON_FALSE(TGS_Intersection, Edgecov);
LT_COMPARISON_FALSE(TGS_Intersection, Filter_Complement);
LT_COMPARISON_FALSE(TGS_Intersection, Filter_Compose);
LT_COMPARISON_FALSE(TGS_Intersection, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(TGS_Intersection, Filter_Function);
LT_COMPARISON_FALSE(TGS_Intersection, Filter_Intersection);
LT_COMPARISON_FALSE(TGS_Intersection, Filter_Setminus);
LT_COMPARISON_FALSE(TGS_Intersection, Filter_Union);
LT_COMPARISON_FALSE(TGS_Intersection, PM_Alternative);
LT_COMPARISON_FALSE(TGS_Intersection, PM_Concat);
LT_COMPARISON_FALSE(TGS_Intersection, PM_Filter_Adapter);
LT_COMPARISON_FALSE(TGS_Intersection, PM_Postcondition);
LT_COMPARISON_FALSE(TGS_Intersection, PM_Precondition);
LT_COMPARISON_FALSE(TGS_Intersection, PM_Repeat);
LT_COMPARISON_FALSE(TGS_Intersection, Pathcov);
LT_COMPARISON_FALSE(TGS_Intersection, Predicate);
LT_COMPARISON_FALSE(TGS_Intersection, Query);
LT_COMPARISON_FALSE(TGS_Intersection, Statecov);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, TGS_Intersection const* b)
{
	if (cmp_lt(a->get_tgs_a(), b->get_tgs_a())) return true;
	if (cmp_lt(b->get_tgs_a(), a->get_tgs_a())) return false;

	return cmp_lt(a->get_tgs_b(), b->get_tgs_b());
}

LT_COMPARISON_TRUE(TGS_Intersection, TGS_Postcondition);
LT_COMPARISON_TRUE(TGS_Intersection, TGS_Precondition);
LT_COMPARISON_TRUE(TGS_Intersection, TGS_Setminus);
LT_COMPARISON_TRUE(TGS_Intersection, TGS_Union);
LT_COMPARISON_TRUE(TGS_Intersection, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(TGS_Intersection);

LT_COMPARISON_FALSE(TGS_Postcondition, Edgecov);
LT_COMPARISON_FALSE(TGS_Postcondition, Filter_Complement);
LT_COMPARISON_FALSE(TGS_Postcondition, Filter_Compose);
LT_COMPARISON_FALSE(TGS_Postcondition, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(TGS_Postcondition, Filter_Function);
LT_COMPARISON_FALSE(TGS_Postcondition, Filter_Intersection);
LT_COMPARISON_FALSE(TGS_Postcondition, Filter_Setminus);
LT_COMPARISON_FALSE(TGS_Postcondition, Filter_Union);
LT_COMPARISON_FALSE(TGS_Postcondition, PM_Alternative);
LT_COMPARISON_FALSE(TGS_Postcondition, PM_Concat);
LT_COMPARISON_FALSE(TGS_Postcondition, PM_Filter_Adapter);
LT_COMPARISON_FALSE(TGS_Postcondition, PM_Postcondition);
LT_COMPARISON_FALSE(TGS_Postcondition, PM_Precondition);
LT_COMPARISON_FALSE(TGS_Postcondition, PM_Repeat);
LT_COMPARISON_FALSE(TGS_Postcondition, Pathcov);
LT_COMPARISON_FALSE(TGS_Postcondition, Predicate);
LT_COMPARISON_FALSE(TGS_Postcondition, Query);
LT_COMPARISON_FALSE(TGS_Postcondition, Statecov);
LT_COMPARISON_FALSE(TGS_Postcondition, TGS_Intersection);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Postcondition const* a, TGS_Postcondition const* b)
{
	if (cmp_lt(a->get_tgs(), b->get_tgs())) return true;
	if (cmp_lt(b->get_tgs(), a->get_tgs())) return false;

	return !cmp_lt(a->get_predicate(), b->get_predicate());
}

LT_COMPARISON_TRUE(TGS_Postcondition, TGS_Precondition);
LT_COMPARISON_TRUE(TGS_Postcondition, TGS_Setminus);
LT_COMPARISON_TRUE(TGS_Postcondition, TGS_Union);
LT_COMPARISON_TRUE(TGS_Postcondition, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(TGS_Postcondition);

LT_COMPARISON_FALSE(TGS_Precondition, Edgecov);
LT_COMPARISON_FALSE(TGS_Precondition, Filter_Complement);
LT_COMPARISON_FALSE(TGS_Precondition, Filter_Compose);
LT_COMPARISON_FALSE(TGS_Precondition, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(TGS_Precondition, Filter_Function);
LT_COMPARISON_FALSE(TGS_Precondition, Filter_Intersection);
LT_COMPARISON_FALSE(TGS_Precondition, Filter_Setminus);
LT_COMPARISON_FALSE(TGS_Precondition, Filter_Union);
LT_COMPARISON_FALSE(TGS_Precondition, PM_Alternative);
LT_COMPARISON_FALSE(TGS_Precondition, PM_Concat);
LT_COMPARISON_FALSE(TGS_Precondition, PM_Filter_Adapter);
LT_COMPARISON_FALSE(TGS_Precondition, PM_Postcondition);
LT_COMPARISON_FALSE(TGS_Precondition, PM_Precondition);
LT_COMPARISON_FALSE(TGS_Precondition, PM_Repeat);
LT_COMPARISON_FALSE(TGS_Precondition, Pathcov);
LT_COMPARISON_FALSE(TGS_Precondition, Predicate);
LT_COMPARISON_FALSE(TGS_Precondition, Query);
LT_COMPARISON_FALSE(TGS_Precondition, Statecov);
LT_COMPARISON_FALSE(TGS_Precondition, TGS_Intersection);
LT_COMPARISON_FALSE(TGS_Precondition, TGS_Postcondition);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Precondition const* a, TGS_Precondition const* b)
{
	if (cmp_lt(a->get_predicate(), b->get_predicate())) return true;
	if (cmp_lt(b->get_predicate(), a->get_predicate())) return false;

	return !cmp_lt(a->get_tgs(), b->get_tgs());
}

LT_COMPARISON_TRUE(TGS_Precondition, TGS_Setminus);
LT_COMPARISON_TRUE(TGS_Precondition, TGS_Union);
LT_COMPARISON_TRUE(TGS_Precondition, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(TGS_Precondition);

LT_COMPARISON_FALSE(TGS_Setminus, Edgecov);
LT_COMPARISON_FALSE(TGS_Setminus, Filter_Complement);
LT_COMPARISON_FALSE(TGS_Setminus, Filter_Compose);
LT_COMPARISON_FALSE(TGS_Setminus, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(TGS_Setminus, Filter_Function);
LT_COMPARISON_FALSE(TGS_Setminus, Filter_Intersection);
LT_COMPARISON_FALSE(TGS_Setminus, Filter_Setminus);
LT_COMPARISON_FALSE(TGS_Setminus, Filter_Union);
LT_COMPARISON_FALSE(TGS_Setminus, PM_Alternative);
LT_COMPARISON_FALSE(TGS_Setminus, PM_Concat);
LT_COMPARISON_FALSE(TGS_Setminus, PM_Filter_Adapter);
LT_COMPARISON_FALSE(TGS_Setminus, PM_Postcondition);
LT_COMPARISON_FALSE(TGS_Setminus, PM_Precondition);
LT_COMPARISON_FALSE(TGS_Setminus, PM_Repeat);
LT_COMPARISON_FALSE(TGS_Setminus, Pathcov);
LT_COMPARISON_FALSE(TGS_Setminus, Predicate);
LT_COMPARISON_FALSE(TGS_Setminus, Query);
LT_COMPARISON_FALSE(TGS_Setminus, Statecov);
LT_COMPARISON_FALSE(TGS_Setminus, TGS_Intersection);
LT_COMPARISON_FALSE(TGS_Setminus, TGS_Postcondition);
LT_COMPARISON_FALSE(TGS_Setminus, TGS_Precondition);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, TGS_Setminus const* b)
{
	if (cmp_lt(a->get_tgs_a(), b->get_tgs_a())) return true;
	if (cmp_lt(b->get_tgs_a(), a->get_tgs_a())) return false;

	return !cmp_lt(a->get_tgs_b(), b->get_tgs_b());
}

LT_COMPARISON_TRUE(TGS_Setminus, TGS_Union);
LT_COMPARISON_TRUE(TGS_Setminus, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(TGS_Setminus);

LT_COMPARISON_FALSE(TGS_Union, Edgecov);
LT_COMPARISON_FALSE(TGS_Union, Filter_Complement);
LT_COMPARISON_FALSE(TGS_Union, Filter_Compose);
LT_COMPARISON_FALSE(TGS_Union, Filter_Enclosing_Scopes);
LT_COMPARISON_FALSE(TGS_Union, Filter_Function);
LT_COMPARISON_FALSE(TGS_Union, Filter_Intersection);
LT_COMPARISON_FALSE(TGS_Union, Filter_Setminus);
LT_COMPARISON_FALSE(TGS_Union, Filter_Union);
LT_COMPARISON_FALSE(TGS_Union, PM_Alternative);
LT_COMPARISON_FALSE(TGS_Union, PM_Concat);
LT_COMPARISON_FALSE(TGS_Union, PM_Filter_Adapter);
LT_COMPARISON_FALSE(TGS_Union, PM_Postcondition);
LT_COMPARISON_FALSE(TGS_Union, PM_Precondition);
LT_COMPARISON_FALSE(TGS_Union, PM_Repeat);
LT_COMPARISON_FALSE(TGS_Union, Pathcov);
LT_COMPARISON_FALSE(TGS_Union, Predicate);
LT_COMPARISON_FALSE(TGS_Union, Query);
LT_COMPARISON_FALSE(TGS_Union, Statecov);
LT_COMPARISON_FALSE(TGS_Union, TGS_Intersection);
LT_COMPARISON_FALSE(TGS_Union, TGS_Postcondition);
LT_COMPARISON_FALSE(TGS_Union, TGS_Precondition);
LT_COMPARISON_FALSE(TGS_Union, TGS_Setminus);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, TGS_Union const* b)
{
	if (cmp_lt(a->get_tgs_a(), b->get_tgs_a())) return true;
	if (cmp_lt(b->get_tgs_a(), a->get_tgs_a())) return false;

	return cmp_lt(a->get_tgs_b(), b->get_tgs_b());
}

LT_COMPARISON_TRUE(TGS_Union, Test_Goal_Sequence);
LT_COMPARISON_BUILDER(TGS_Union);

LT_COMPARISON_BUILDER(Test_Goal_Sequence);

LT_VISIT_BUILDER_TPL(Edgecov);
LT_VISIT_BUILDER_TPL(Filter_Complement);
LT_VISIT_BUILDER_TPL(Filter_Compose);
LT_VISIT_BUILDER_TPL(Filter_Enclosing_Scopes);
LT_VISIT_BUILDER_TPL(Filter_Function);
LT_VISIT_BUILDER_TPL(Filter_Intersection);
LT_VISIT_BUILDER_TPL(Filter_Setminus);
LT_VISIT_BUILDER_TPL(Filter_Union);
LT_VISIT_BUILDER_TPL(PM_Alternative);
LT_VISIT_BUILDER_TPL(PM_Concat);
LT_VISIT_BUILDER_TPL(PM_Filter_Adapter);
LT_VISIT_BUILDER_TPL(PM_Postcondition);
LT_VISIT_BUILDER_TPL(PM_Precondition);
LT_VISIT_BUILDER_TPL(PM_Repeat);
LT_VISIT_BUILDER_TPL(Pathcov);
LT_VISIT_BUILDER_TPL(Predicate);
LT_VISIT_BUILDER_TPL(Query);
LT_VISIT_BUILDER_TPL(Statecov);
LT_VISIT_BUILDER_TPL(TGS_Intersection);
LT_VISIT_BUILDER_TPL(TGS_Postcondition);
LT_VISIT_BUILDER_TPL(TGS_Precondition);
LT_VISIT_BUILDER_TPL(TGS_Setminus);
LT_VISIT_BUILDER_TPL(TGS_Union);
LT_VISIT_BUILDER_TPL(Test_Goal_Sequence);

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__FQL_NODE_LT_VISITOR_HPP */
