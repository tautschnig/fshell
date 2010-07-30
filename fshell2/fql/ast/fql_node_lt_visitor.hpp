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
#include <fshell2/fql/ast/coverage_pattern_expr.hpp>
#include <fshell2/fql/ast/cp_alternative.hpp>
#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/depcov.hpp>
#include <fshell2/fql/ast/ecp_atom.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_expr.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/fql_node.hpp>
#include <fshell2/fql/ast/nodecov.hpp>
#include <fshell2/fql/ast/path_pattern_expr.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pp_alternative.hpp>
#include <fshell2/fql/ast/pp_concat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>
#include <fshell2/fql/ast/repeat.hpp>
#include <fshell2/fql/ast/transform_pred.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
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
				virtual void visit(CP_Alternative const* n);
				virtual void visit(CP_Concat const* n);
				virtual void visit(Depcov const* n);
				virtual void visit(Edgecov const* n);
				virtual void visit(Filter_Compose const* n);
				virtual void visit(Filter_Function const* n);
				virtual void visit(Filter_Intersection const* n);
				virtual void visit(Filter_Setminus const* n);
				virtual void visit(Filter_Union const* n);
				virtual void visit(Nodecov const* n);
				virtual void visit(PP_Alternative const* n);
				virtual void visit(PP_Concat const* n);
				virtual void visit(Pathcov const* n);
				virtual void visit(Predicate const* n);
				virtual void visit(Quote const* n);
				virtual void visit(Query const* n);
				virtual void visit(Repeat const* n);
				virtual void visit(Transform_Pred const* n);
				/*! \} */

			private:
				virtual void visit(CP_Alternative const* n) const;
				virtual void visit(CP_Concat const* n) const;
				virtual void visit(Coverage_Pattern_Expr const* n);
				virtual void visit(Coverage_Pattern_Expr const* n) const;
				virtual void visit(Depcov const* n) const;
				virtual void visit(ECP_Atom const* n);
				virtual void visit(ECP_Atom const* n) const;
				virtual void visit(Edgecov const* n) const;
				virtual void visit(FQL_Node const* n);
				virtual void visit(FQL_Node const* n) const;
				virtual void visit(Filter_Compose const* n) const;
				virtual void visit(Filter_Expr const* n);
				virtual void visit(Filter_Expr const* n) const;
				virtual void visit(Filter_Function const* n) const;
				virtual void visit(Filter_Intersection const* n) const;
				virtual void visit(Filter_Setminus const* n) const;
				virtual void visit(Filter_Union const* n) const;
				virtual void visit(Nodecov const* n) const;
				virtual void visit(PP_Alternative const* n) const;
				virtual void visit(PP_Concat const* n) const;
				virtual void visit(Path_Pattern_Expr const* n);
				virtual void visit(Path_Pattern_Expr const* n) const;
				virtual void visit(Pathcov const* n) const;
				virtual void visit(Predicate const* n) const;
				virtual void visit(Quote const* n) const;
				virtual void visit(Query const* n) const;
				virtual void visit(Repeat const* n) const;
				virtual void visit(Transform_Pred const* n) const;

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

		virtual void visit(CP_Alternative const* n) const;
		virtual void visit(CP_Concat const* n) const;
		virtual void visit(Coverage_Pattern_Expr const* n);
		virtual void visit(Coverage_Pattern_Expr const* n) const;
		virtual void visit(Depcov const* n) const;
		virtual void visit(ECP_Atom const* n);
		virtual void visit(ECP_Atom const* n) const;
		virtual void visit(Edgecov const* n) const;
		virtual void visit(FQL_Node const* n);
		virtual void visit(FQL_Node const* n) const;
		virtual void visit(Filter_Compose const* n) const;
		virtual void visit(Filter_Expr const* n);
		virtual void visit(Filter_Expr const* n) const;
		virtual void visit(Filter_Function const* n) const;
		virtual void visit(Filter_Intersection const* n) const;
		virtual void visit(Filter_Setminus const* n) const;
		virtual void visit(Filter_Union const* n) const;
		virtual void visit(Nodecov const* n) const;
		virtual void visit(PP_Alternative const* n) const;
		virtual void visit(PP_Concat const* n) const;
		virtual void visit(Path_Pattern_Expr const* n);
		virtual void visit(Path_Pattern_Expr const* n) const;
		virtual void visit(Pathcov const* n) const;
		virtual void visit(Predicate const* n) const;
		virtual void visit(Quote const* n) const;
		virtual void visit(Query const* n) const;
		virtual void visit(Repeat const* n) const;
		virtual void visit(Transform_Pred const* n) const;

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
inline bool FQL_Node_Lt_Visitor::cmp_lt(Path_Pattern_Expr const* a, Path_Pattern_Expr const* b)
{
	if (a == b) return false;
	m_other = b;
	a->accept(this);
	return m_lt;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Coverage_Pattern_Expr const* a, Coverage_Pattern_Expr const* b)
{
	if (a == b) return false;
	m_other = b;
	a->accept(this);
	return m_lt;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(ECP_Atom const* a, ECP_Atom const* b)
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
CP_Alternative
CP_Concat
Depcov
Edgecov
Filter_Compose
Filter_Function
Filter_Intersection
Filter_Setminus
Filter_Union
Nodecov
PP_Alternative
PP_Concat
Pathcov
Predicate
Quote
Query
Repeat
Transform_Pred
*/

/*
for c in CP_Alternative CP_Concat Depcov Edgecov Filter_Compose Filter_Function Filter_Intersection Filter_Setminus Filter_Union Nodecov PP_Alternative PP_Concat Pathcov Predicate Quote Query Repeat Transform_Pred ; do res="FALSE" ; for c2 in CP_Alternative CP_Concat Depcov Edgecov Filter_Compose Filter_Function Filter_Intersection Filter_Setminus Filter_Union Nodecov PP_Alternative PP_Concat Pathcov Predicate Quote Query Repeat Transform_Pred ; do if [ "$c" = "$c2" ] ; then res="TRUE" ; echo "XXX" ; else echo "LT_COMPARISON_$res($c, $c2);" ; fi ; done ; echo "LT_COMPARISON_BUILDER($c);" ; echo ; done
*/

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(CP_Alternative const* a, CP_Alternative const* b)
{
	if (cmp_lt(a->get_cp_a(), b->get_cp_a())) return true;
	if (cmp_lt(b->get_cp_a(), a->get_cp_a())) return false;

	return cmp_lt(a->get_cp_b(), b->get_cp_b());
}
LT_COMPARISON_TRUE(CP_Alternative, CP_Concat);
LT_COMPARISON_TRUE(CP_Alternative, Depcov);
LT_COMPARISON_TRUE(CP_Alternative, Edgecov);
LT_COMPARISON_TRUE(CP_Alternative, Filter_Compose);
LT_COMPARISON_TRUE(CP_Alternative, Filter_Function);
LT_COMPARISON_TRUE(CP_Alternative, Filter_Intersection);
LT_COMPARISON_TRUE(CP_Alternative, Filter_Setminus);
LT_COMPARISON_TRUE(CP_Alternative, Filter_Union);
LT_COMPARISON_TRUE(CP_Alternative, Nodecov);
LT_COMPARISON_TRUE(CP_Alternative, PP_Alternative);
LT_COMPARISON_TRUE(CP_Alternative, PP_Concat);
LT_COMPARISON_TRUE(CP_Alternative, Pathcov);
LT_COMPARISON_TRUE(CP_Alternative, Predicate);
LT_COMPARISON_TRUE(CP_Alternative, Quote);
LT_COMPARISON_TRUE(CP_Alternative, Query);
LT_COMPARISON_TRUE(CP_Alternative, Repeat);
LT_COMPARISON_TRUE(CP_Alternative, Transform_Pred);
LT_COMPARISON_BUILDER(CP_Alternative);

LT_COMPARISON_FALSE(CP_Concat, CP_Alternative);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(CP_Concat const* a, CP_Concat const* b)
{
	if (cmp_lt(a->get_cp_a(), b->get_cp_a())) return true;
	if (cmp_lt(b->get_cp_a(), a->get_cp_a())) return false;

	return cmp_lt(a->get_cp_b(), b->get_cp_b());
}
LT_COMPARISON_TRUE(CP_Concat, Depcov);
LT_COMPARISON_TRUE(CP_Concat, Edgecov);
LT_COMPARISON_TRUE(CP_Concat, Filter_Compose);
LT_COMPARISON_TRUE(CP_Concat, Filter_Function);
LT_COMPARISON_TRUE(CP_Concat, Filter_Intersection);
LT_COMPARISON_TRUE(CP_Concat, Filter_Setminus);
LT_COMPARISON_TRUE(CP_Concat, Filter_Union);
LT_COMPARISON_TRUE(CP_Concat, Nodecov);
LT_COMPARISON_TRUE(CP_Concat, PP_Alternative);
LT_COMPARISON_TRUE(CP_Concat, PP_Concat);
LT_COMPARISON_TRUE(CP_Concat, Pathcov);
LT_COMPARISON_TRUE(CP_Concat, Predicate);
LT_COMPARISON_TRUE(CP_Concat, Quote);
LT_COMPARISON_TRUE(CP_Concat, Query);
LT_COMPARISON_TRUE(CP_Concat, Repeat);
LT_COMPARISON_TRUE(CP_Concat, Transform_Pred);
LT_COMPARISON_BUILDER(CP_Concat);

LT_COMPARISON_FALSE(Depcov, CP_Alternative);
LT_COMPARISON_FALSE(Depcov, CP_Concat);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Depcov const* a, Depcov const* b)
{
	if (cmp_lt(a->get_filter_a(), b->get_filter_a())) return true;
	if (cmp_lt(b->get_filter_a(), a->get_filter_a())) return false;

	if (cmp_lt(a->get_filter_b(), b->get_filter_b())) return true;
	if (cmp_lt(b->get_filter_b(), a->get_filter_b())) return false;

	return !cmp_lt(a->get_filter_c(), b->get_filter_c());
}
LT_COMPARISON_TRUE(Depcov, Edgecov);
LT_COMPARISON_TRUE(Depcov, Filter_Compose);
LT_COMPARISON_TRUE(Depcov, Filter_Function);
LT_COMPARISON_TRUE(Depcov, Filter_Intersection);
LT_COMPARISON_TRUE(Depcov, Filter_Setminus);
LT_COMPARISON_TRUE(Depcov, Filter_Union);
LT_COMPARISON_TRUE(Depcov, Nodecov);
LT_COMPARISON_TRUE(Depcov, PP_Alternative);
LT_COMPARISON_TRUE(Depcov, PP_Concat);
LT_COMPARISON_TRUE(Depcov, Pathcov);
LT_COMPARISON_TRUE(Depcov, Predicate);
LT_COMPARISON_TRUE(Depcov, Quote);
LT_COMPARISON_TRUE(Depcov, Query);
LT_COMPARISON_TRUE(Depcov, Repeat);
LT_COMPARISON_TRUE(Depcov, Transform_Pred);
LT_COMPARISON_BUILDER(Depcov);

LT_COMPARISON_FALSE(Edgecov, CP_Alternative);
LT_COMPARISON_FALSE(Edgecov, CP_Concat);
LT_COMPARISON_FALSE(Edgecov, Depcov);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Edgecov const* b)
{
	return cmp_lt(a->get_filter_expr(), b->get_filter_expr());
}
LT_COMPARISON_TRUE(Edgecov, Filter_Compose);
LT_COMPARISON_TRUE(Edgecov, Filter_Function);
LT_COMPARISON_TRUE(Edgecov, Filter_Intersection);
LT_COMPARISON_TRUE(Edgecov, Filter_Setminus);
LT_COMPARISON_TRUE(Edgecov, Filter_Union);
LT_COMPARISON_TRUE(Edgecov, Nodecov);
LT_COMPARISON_TRUE(Edgecov, PP_Alternative);
LT_COMPARISON_TRUE(Edgecov, PP_Concat);
LT_COMPARISON_TRUE(Edgecov, Pathcov);
LT_COMPARISON_TRUE(Edgecov, Predicate);
LT_COMPARISON_TRUE(Edgecov, Quote);
LT_COMPARISON_TRUE(Edgecov, Query);
LT_COMPARISON_TRUE(Edgecov, Repeat);
LT_COMPARISON_TRUE(Edgecov, Transform_Pred);
LT_COMPARISON_BUILDER(Edgecov);

LT_COMPARISON_FALSE(Filter_Compose, CP_Alternative);
LT_COMPARISON_FALSE(Filter_Compose, CP_Concat);
LT_COMPARISON_FALSE(Filter_Compose, Depcov);
LT_COMPARISON_FALSE(Filter_Compose, Edgecov);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Compose const* a, Filter_Compose const* b)
{
	if (cmp_lt(a->get_filter_expr_a(), b->get_filter_expr_a())) return true;
	if (cmp_lt(b->get_filter_expr_a(), a->get_filter_expr_a())) return false;

	return !cmp_lt(a->get_filter_expr_b(), b->get_filter_expr_b());
}
LT_COMPARISON_TRUE(Filter_Compose, Filter_Function);
LT_COMPARISON_TRUE(Filter_Compose, Filter_Intersection);
LT_COMPARISON_TRUE(Filter_Compose, Filter_Setminus);
LT_COMPARISON_TRUE(Filter_Compose, Filter_Union);
LT_COMPARISON_TRUE(Filter_Compose, Nodecov);
LT_COMPARISON_TRUE(Filter_Compose, PP_Alternative);
LT_COMPARISON_TRUE(Filter_Compose, PP_Concat);
LT_COMPARISON_TRUE(Filter_Compose, Pathcov);
LT_COMPARISON_TRUE(Filter_Compose, Predicate);
LT_COMPARISON_TRUE(Filter_Compose, Quote);
LT_COMPARISON_TRUE(Filter_Compose, Query);
LT_COMPARISON_TRUE(Filter_Compose, Repeat);
LT_COMPARISON_TRUE(Filter_Compose, Transform_Pred);
LT_COMPARISON_BUILDER(Filter_Compose);

LT_COMPARISON_FALSE(Filter_Function, CP_Alternative);
LT_COMPARISON_FALSE(Filter_Function, CP_Concat);
LT_COMPARISON_FALSE(Filter_Function, Depcov);
LT_COMPARISON_FALSE(Filter_Function, Edgecov);
LT_COMPARISON_FALSE(Filter_Function, Filter_Compose);
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
LT_COMPARISON_TRUE(Filter_Function, Nodecov);
LT_COMPARISON_TRUE(Filter_Function, PP_Alternative);
LT_COMPARISON_TRUE(Filter_Function, PP_Concat);
LT_COMPARISON_TRUE(Filter_Function, Pathcov);
LT_COMPARISON_TRUE(Filter_Function, Predicate);
LT_COMPARISON_TRUE(Filter_Function, Quote);
LT_COMPARISON_TRUE(Filter_Function, Query);
LT_COMPARISON_TRUE(Filter_Function, Repeat);
LT_COMPARISON_TRUE(Filter_Function, Transform_Pred);
LT_COMPARISON_BUILDER(Filter_Function);

LT_COMPARISON_FALSE(Filter_Intersection, CP_Alternative);
LT_COMPARISON_FALSE(Filter_Intersection, CP_Concat);
LT_COMPARISON_FALSE(Filter_Intersection, Depcov);
LT_COMPARISON_FALSE(Filter_Intersection, Edgecov);
LT_COMPARISON_FALSE(Filter_Intersection, Filter_Compose);
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
LT_COMPARISON_TRUE(Filter_Intersection, Nodecov);
LT_COMPARISON_TRUE(Filter_Intersection, PP_Alternative);
LT_COMPARISON_TRUE(Filter_Intersection, PP_Concat);
LT_COMPARISON_TRUE(Filter_Intersection, Pathcov);
LT_COMPARISON_TRUE(Filter_Intersection, Predicate);
LT_COMPARISON_TRUE(Filter_Intersection, Quote);
LT_COMPARISON_TRUE(Filter_Intersection, Query);
LT_COMPARISON_TRUE(Filter_Intersection, Repeat);
LT_COMPARISON_TRUE(Filter_Intersection, Transform_Pred);
LT_COMPARISON_BUILDER(Filter_Intersection);

LT_COMPARISON_FALSE(Filter_Setminus, CP_Alternative);
LT_COMPARISON_FALSE(Filter_Setminus, CP_Concat);
LT_COMPARISON_FALSE(Filter_Setminus, Depcov);
LT_COMPARISON_FALSE(Filter_Setminus, Edgecov);
LT_COMPARISON_FALSE(Filter_Setminus, Filter_Compose);
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
LT_COMPARISON_TRUE(Filter_Setminus, Nodecov);
LT_COMPARISON_TRUE(Filter_Setminus, PP_Alternative);
LT_COMPARISON_TRUE(Filter_Setminus, PP_Concat);
LT_COMPARISON_TRUE(Filter_Setminus, Pathcov);
LT_COMPARISON_TRUE(Filter_Setminus, Predicate);
LT_COMPARISON_TRUE(Filter_Setminus, Quote);
LT_COMPARISON_TRUE(Filter_Setminus, Query);
LT_COMPARISON_TRUE(Filter_Setminus, Repeat);
LT_COMPARISON_TRUE(Filter_Setminus, Transform_Pred);
LT_COMPARISON_BUILDER(Filter_Setminus);

LT_COMPARISON_FALSE(Filter_Union, CP_Alternative);
LT_COMPARISON_FALSE(Filter_Union, CP_Concat);
LT_COMPARISON_FALSE(Filter_Union, Depcov);
LT_COMPARISON_FALSE(Filter_Union, Edgecov);
LT_COMPARISON_FALSE(Filter_Union, Filter_Compose);
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
LT_COMPARISON_TRUE(Filter_Union, Nodecov);
LT_COMPARISON_TRUE(Filter_Union, PP_Alternative);
LT_COMPARISON_TRUE(Filter_Union, PP_Concat);
LT_COMPARISON_TRUE(Filter_Union, Pathcov);
LT_COMPARISON_TRUE(Filter_Union, Predicate);
LT_COMPARISON_TRUE(Filter_Union, Quote);
LT_COMPARISON_TRUE(Filter_Union, Query);
LT_COMPARISON_TRUE(Filter_Union, Repeat);
LT_COMPARISON_TRUE(Filter_Union, Transform_Pred);
LT_COMPARISON_BUILDER(Filter_Union);

LT_COMPARISON_FALSE(Nodecov, CP_Alternative);
LT_COMPARISON_FALSE(Nodecov, CP_Concat);
LT_COMPARISON_FALSE(Nodecov, Depcov);
LT_COMPARISON_FALSE(Nodecov, Edgecov);
LT_COMPARISON_FALSE(Nodecov, Filter_Compose);
LT_COMPARISON_FALSE(Nodecov, Filter_Function);
LT_COMPARISON_FALSE(Nodecov, Filter_Intersection);
LT_COMPARISON_FALSE(Nodecov, Filter_Setminus);
LT_COMPARISON_FALSE(Nodecov, Filter_Union);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Nodecov const* a, Nodecov const* b)
{
	return cmp_lt(a->get_filter_expr(), b->get_filter_expr());
}
LT_COMPARISON_TRUE(Nodecov, PP_Alternative);
LT_COMPARISON_TRUE(Nodecov, PP_Concat);
LT_COMPARISON_TRUE(Nodecov, Pathcov);
LT_COMPARISON_TRUE(Nodecov, Predicate);
LT_COMPARISON_TRUE(Nodecov, Quote);
LT_COMPARISON_TRUE(Nodecov, Query);
LT_COMPARISON_TRUE(Nodecov, Repeat);
LT_COMPARISON_TRUE(Nodecov, Transform_Pred);
LT_COMPARISON_BUILDER(Nodecov);

LT_COMPARISON_FALSE(PP_Alternative, CP_Alternative);
LT_COMPARISON_FALSE(PP_Alternative, CP_Concat);
LT_COMPARISON_FALSE(PP_Alternative, Depcov);
LT_COMPARISON_FALSE(PP_Alternative, Edgecov);
LT_COMPARISON_FALSE(PP_Alternative, Filter_Compose);
LT_COMPARISON_FALSE(PP_Alternative, Filter_Function);
LT_COMPARISON_FALSE(PP_Alternative, Filter_Intersection);
LT_COMPARISON_FALSE(PP_Alternative, Filter_Setminus);
LT_COMPARISON_FALSE(PP_Alternative, Filter_Union);
LT_COMPARISON_FALSE(PP_Alternative, Nodecov);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(PP_Alternative const* a, PP_Alternative const* b)
{
	if (cmp_lt(a->get_pp_a(), b->get_pp_a())) return true;
	if (cmp_lt(b->get_pp_a(), a->get_pp_a())) return false;

	return cmp_lt(a->get_pp_b(), b->get_pp_b());
}
LT_COMPARISON_TRUE(PP_Alternative, PP_Concat);
LT_COMPARISON_TRUE(PP_Alternative, Pathcov);
LT_COMPARISON_TRUE(PP_Alternative, Predicate);
LT_COMPARISON_TRUE(PP_Alternative, Quote);
LT_COMPARISON_TRUE(PP_Alternative, Query);
LT_COMPARISON_TRUE(PP_Alternative, Repeat);
LT_COMPARISON_TRUE(PP_Alternative, Transform_Pred);
LT_COMPARISON_BUILDER(PP_Alternative);

LT_COMPARISON_FALSE(PP_Concat, CP_Alternative);
LT_COMPARISON_FALSE(PP_Concat, CP_Concat);
LT_COMPARISON_FALSE(PP_Concat, Depcov);
LT_COMPARISON_FALSE(PP_Concat, Edgecov);
LT_COMPARISON_FALSE(PP_Concat, Filter_Compose);
LT_COMPARISON_FALSE(PP_Concat, Filter_Function);
LT_COMPARISON_FALSE(PP_Concat, Filter_Intersection);
LT_COMPARISON_FALSE(PP_Concat, Filter_Setminus);
LT_COMPARISON_FALSE(PP_Concat, Filter_Union);
LT_COMPARISON_FALSE(PP_Concat, Nodecov);
LT_COMPARISON_FALSE(PP_Concat, PP_Alternative);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(PP_Concat const* a, PP_Concat const* b)
{
	if (cmp_lt(a->get_pp_a(), b->get_pp_a())) return true;
	if (cmp_lt(b->get_pp_a(), a->get_pp_a())) return false;

	return cmp_lt(a->get_pp_b(), b->get_pp_b());
}
LT_COMPARISON_TRUE(PP_Concat, Pathcov);
LT_COMPARISON_TRUE(PP_Concat, Predicate);
LT_COMPARISON_TRUE(PP_Concat, Quote);
LT_COMPARISON_TRUE(PP_Concat, Query);
LT_COMPARISON_TRUE(PP_Concat, Repeat);
LT_COMPARISON_TRUE(PP_Concat, Transform_Pred);
LT_COMPARISON_BUILDER(PP_Concat);

LT_COMPARISON_FALSE(Pathcov, CP_Alternative);
LT_COMPARISON_FALSE(Pathcov, CP_Concat);
LT_COMPARISON_FALSE(Pathcov, Depcov);
LT_COMPARISON_FALSE(Pathcov, Edgecov);
LT_COMPARISON_FALSE(Pathcov, Filter_Compose);
LT_COMPARISON_FALSE(Pathcov, Filter_Function);
LT_COMPARISON_FALSE(Pathcov, Filter_Intersection);
LT_COMPARISON_FALSE(Pathcov, Filter_Setminus);
LT_COMPARISON_FALSE(Pathcov, Filter_Union);
LT_COMPARISON_FALSE(Pathcov, Nodecov);
LT_COMPARISON_FALSE(Pathcov, PP_Alternative);
LT_COMPARISON_FALSE(Pathcov, PP_Concat);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Pathcov const* b)
{
	if (cmp_lt(a->get_filter_expr(), b->get_filter_expr())) return true;
	if (cmp_lt(b->get_filter_expr(), a->get_filter_expr())) return false;
	
	return a->get_bound() < b->get_bound();
}
LT_COMPARISON_TRUE(Pathcov, Predicate);
LT_COMPARISON_TRUE(Pathcov, Quote);
LT_COMPARISON_TRUE(Pathcov, Query);
LT_COMPARISON_TRUE(Pathcov, Repeat);
LT_COMPARISON_TRUE(Pathcov, Transform_Pred);
LT_COMPARISON_BUILDER(Pathcov);

LT_COMPARISON_FALSE(Predicate, CP_Alternative);
LT_COMPARISON_FALSE(Predicate, CP_Concat);
LT_COMPARISON_FALSE(Predicate, Depcov);
LT_COMPARISON_FALSE(Predicate, Edgecov);
LT_COMPARISON_FALSE(Predicate, Filter_Compose);
LT_COMPARISON_FALSE(Predicate, Filter_Function);
LT_COMPARISON_FALSE(Predicate, Filter_Intersection);
LT_COMPARISON_FALSE(Predicate, Filter_Setminus);
LT_COMPARISON_FALSE(Predicate, Filter_Union);
LT_COMPARISON_FALSE(Predicate, Nodecov);
LT_COMPARISON_FALSE(Predicate, PP_Alternative);
LT_COMPARISON_FALSE(Predicate, PP_Concat);
LT_COMPARISON_FALSE(Predicate, Pathcov);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Predicate const* b)
{
	if (*(a->get_expr()) == *(b->get_expr())) return false;
	// simple pointer comparison
	return a->get_expr() < b->get_expr();
}
LT_COMPARISON_TRUE(Predicate, Quote);
LT_COMPARISON_TRUE(Predicate, Query);
LT_COMPARISON_TRUE(Predicate, Repeat);
LT_COMPARISON_TRUE(Predicate, Transform_Pred);
LT_COMPARISON_BUILDER(Predicate);

LT_COMPARISON_FALSE(Quote, CP_Alternative);
LT_COMPARISON_FALSE(Quote, CP_Concat);
LT_COMPARISON_FALSE(Quote, Depcov);
LT_COMPARISON_FALSE(Quote, Edgecov);
LT_COMPARISON_FALSE(Quote, Filter_Compose);
LT_COMPARISON_FALSE(Quote, Filter_Function);
LT_COMPARISON_FALSE(Quote, Filter_Intersection);
LT_COMPARISON_FALSE(Quote, Filter_Setminus);
LT_COMPARISON_FALSE(Quote, Filter_Union);
LT_COMPARISON_FALSE(Quote, Nodecov);
LT_COMPARISON_FALSE(Quote, PP_Alternative);
LT_COMPARISON_FALSE(Quote, PP_Concat);
LT_COMPARISON_FALSE(Quote, Pathcov);
LT_COMPARISON_FALSE(Quote, Predicate);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Quote const* a, Quote const* b)
{
	return cmp_lt(b->get_pp(), a->get_pp());
}
LT_COMPARISON_TRUE(Quote, Query);
LT_COMPARISON_TRUE(Quote, Repeat);
LT_COMPARISON_TRUE(Quote, Transform_Pred);
LT_COMPARISON_BUILDER(Quote);

LT_COMPARISON_FALSE(Query, CP_Alternative);
LT_COMPARISON_FALSE(Query, CP_Concat);
LT_COMPARISON_FALSE(Query, Depcov);
LT_COMPARISON_FALSE(Query, Edgecov);
LT_COMPARISON_FALSE(Query, Filter_Compose);
LT_COMPARISON_FALSE(Query, Filter_Function);
LT_COMPARISON_FALSE(Query, Filter_Intersection);
LT_COMPARISON_FALSE(Query, Filter_Setminus);
LT_COMPARISON_FALSE(Query, Filter_Union);
LT_COMPARISON_FALSE(Query, Nodecov);
LT_COMPARISON_FALSE(Query, PP_Alternative);
LT_COMPARISON_FALSE(Query, PP_Concat);
LT_COMPARISON_FALSE(Query, Pathcov);
LT_COMPARISON_FALSE(Query, Predicate);
LT_COMPARISON_FALSE(Query, Quote);
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
LT_COMPARISON_TRUE(Query, Repeat);
LT_COMPARISON_TRUE(Query, Transform_Pred);
LT_COMPARISON_BUILDER(Query);

LT_COMPARISON_FALSE(Repeat, CP_Alternative);
LT_COMPARISON_FALSE(Repeat, CP_Concat);
LT_COMPARISON_FALSE(Repeat, Depcov);
LT_COMPARISON_FALSE(Repeat, Edgecov);
LT_COMPARISON_FALSE(Repeat, Filter_Compose);
LT_COMPARISON_FALSE(Repeat, Filter_Function);
LT_COMPARISON_FALSE(Repeat, Filter_Intersection);
LT_COMPARISON_FALSE(Repeat, Filter_Setminus);
LT_COMPARISON_FALSE(Repeat, Filter_Union);
LT_COMPARISON_FALSE(Repeat, Nodecov);
LT_COMPARISON_FALSE(Repeat, PP_Alternative);
LT_COMPARISON_FALSE(Repeat, PP_Concat);
LT_COMPARISON_FALSE(Repeat, Pathcov);
LT_COMPARISON_FALSE(Repeat, Predicate);
LT_COMPARISON_FALSE(Repeat, Quote);
LT_COMPARISON_FALSE(Repeat, Query);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Repeat const* a, Repeat const* b)
{
	if (cmp_lt(a->get_pp(), b->get_pp())) return true;
	if (cmp_lt(b->get_pp(), a->get_pp())) return false;

	if (a->get_lower_bound() != b->get_lower_bound())
		return a->get_lower_bound() < b->get_lower_bound();
	if (a->get_upper_bound() == -1) return false;
	if (b->get_upper_bound() == -1) return true;
	return a->get_upper_bound() < b->get_upper_bound();
}
LT_COMPARISON_TRUE(Repeat, Transform_Pred);
LT_COMPARISON_BUILDER(Repeat);

LT_COMPARISON_FALSE(Transform_Pred, CP_Alternative);
LT_COMPARISON_FALSE(Transform_Pred, CP_Concat);
LT_COMPARISON_FALSE(Transform_Pred, Depcov);
LT_COMPARISON_FALSE(Transform_Pred, Edgecov);
LT_COMPARISON_FALSE(Transform_Pred, Filter_Compose);
LT_COMPARISON_FALSE(Transform_Pred, Filter_Function);
LT_COMPARISON_FALSE(Transform_Pred, Filter_Intersection);
LT_COMPARISON_FALSE(Transform_Pred, Filter_Setminus);
LT_COMPARISON_FALSE(Transform_Pred, Filter_Union);
LT_COMPARISON_FALSE(Transform_Pred, Nodecov);
LT_COMPARISON_FALSE(Transform_Pred, PP_Alternative);
LT_COMPARISON_FALSE(Transform_Pred, PP_Concat);
LT_COMPARISON_FALSE(Transform_Pred, Pathcov);
LT_COMPARISON_FALSE(Transform_Pred, Predicate);
LT_COMPARISON_FALSE(Transform_Pred, Quote);
LT_COMPARISON_FALSE(Transform_Pred, Query);
LT_COMPARISON_FALSE(Transform_Pred, Repeat);
template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Transform_Pred const* a, Transform_Pred const* b)
{
	if (cmp_lt(a->get_filter_expr(), b->get_filter_expr())) return true;
	if (cmp_lt(b->get_filter_expr(), a->get_filter_expr())) return false;

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
LT_COMPARISON_BUILDER(Transform_Pred);


LT_VISIT_BUILDER_TPL(CP_Alternative);
LT_VISIT_BUILDER_TPL(CP_Concat);
LT_VISIT_BUILDER_TPL(Depcov);
LT_VISIT_BUILDER_TPL(Edgecov);
LT_VISIT_BUILDER_TPL(Filter_Compose);
LT_VISIT_BUILDER_TPL(Filter_Function);
LT_VISIT_BUILDER_TPL(Filter_Intersection);
LT_VISIT_BUILDER_TPL(Filter_Setminus);
LT_VISIT_BUILDER_TPL(Filter_Union);
LT_VISIT_BUILDER_TPL(Nodecov);
LT_VISIT_BUILDER_TPL(PP_Alternative);
LT_VISIT_BUILDER_TPL(PP_Concat);
LT_VISIT_BUILDER_TPL(Pathcov);
LT_VISIT_BUILDER_TPL(Predicate);
LT_VISIT_BUILDER_TPL(Quote);
LT_VISIT_BUILDER_TPL(Query);
LT_VISIT_BUILDER_TPL(Repeat);
LT_VISIT_BUILDER_TPL(Transform_Pred);

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__FQL_NODE_LT_VISITOR_HPP */
