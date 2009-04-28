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
#include <fshell2/fql/ast/abstraction.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter.hpp>
#include <fshell2/fql/ast/filter_complement.hpp>
#include <fshell2/fql/ast/filter_enclosing_scopes.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/primitive_filter.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/restriction_automaton.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/test_goal_set.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>

#if AUTOMATA_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/not_implemented.hpp>
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
		 * \brief Visit a @ref fshell2::fql::Query
		 * \param  n Query
		 */
		virtual void visit(Query const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Test_Goal_Sequence
		 * \param  n Test_Goal_Sequence
		 */
		virtual void visit(Test_Goal_Sequence const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Restriction_Automaton
		 * \param  n Restriction_Automaton
		 */
		virtual void visit(Restriction_Automaton const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Abstraction
		 * \param  n Abstraction
		 */
		virtual void visit(Abstraction const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Identity
		 * \param  n Filter_Identity
		 */
		virtual void visit(Filter_Identity const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Predicate
		 * \param  n Predicate
		 */
		virtual void visit(Predicate const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::TGS_Union
		 * \param  n TGS_Union
		 */
		virtual void visit(TGS_Union const* n);
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
		 * \brief Visit a @ref fshell2::fql::Edgecov
		 * \param  n Edgecov
		 */
		virtual void visit(Edgecov const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Pathcov
		 * \param  n Pathcov
		 */
		virtual void visit(Pathcov const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Complement
		 * \param  n Filter_Complement
		 */
		virtual void visit(Filter_Complement const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Filter_Union
		 * \param  n Filter_Union
		 */
		virtual void visit(Filter_Union const* n);
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
		 * \brief Visit a @ref fshell2::fql::Filter_Enclosing_Scopes
		 * \param  n Filter_Enclosing_Scopes
		 */
		virtual void visit(Filter_Enclosing_Scopes const* n);
		/*! \} */

		/*! \{
		 * \brief Visit a @ref fshell2::fql::Primitive_Filter
		 * \param  n Primitive_Filter
		 */
		virtual void visit(Primitive_Filter const* n);
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
				virtual void visit(Query const* n);
				virtual void visit(Test_Goal_Sequence const* n);
				virtual void visit(Restriction_Automaton const* n);
				virtual void visit(Abstraction const* n);
				virtual void visit(Predicate const* n);
				virtual void visit(TGS_Union const* n);
				virtual void visit(TGS_Intersection const* n);
				virtual void visit(TGS_Setminus const* n);
				virtual void visit(Edgecov const* n);
				virtual void visit(Pathcov const* n);
				virtual void visit(Filter_Identity const* n);
				virtual void visit(Filter_Complement const* n);
				virtual void visit(Filter_Union const* n);
				virtual void visit(Filter_Intersection const* n);
				virtual void visit(Filter_Setminus const* n);
				virtual void visit(Filter_Enclosing_Scopes const* n);
				virtual void visit(Primitive_Filter const* n);
				/*! \} */

			private:
				virtual void visit(FQL_Node const* n);
				virtual void visit(FQL_Node const* n) const;
				virtual void visit(Query const* n) const;
				virtual void visit(Test_Goal_Sequence const* n) const;
				virtual void visit(Test_Goal_Set const* n);
				virtual void visit(Test_Goal_Set const* n) const;
				virtual void visit(Restriction_Automaton const* n) const;
				virtual void visit(Abstraction const* n) const;
				virtual void visit(Predicate const* n) const;
				virtual void visit(TGS_Union const* n) const;
				virtual void visit(TGS_Intersection const* n) const;
				virtual void visit(TGS_Setminus const* n) const;
				virtual void visit(Edgecov const* n) const;
				virtual void visit(Pathcov const* n) const;
				virtual void visit(Filter const* n);
				virtual void visit(Filter const* n) const;
				virtual void visit(Filter_Identity const* n) const;
				virtual void visit(Filter_Complement const* n) const;
				virtual void visit(Filter_Union const* n) const;
				virtual void visit(Filter_Intersection const* n) const;
				virtual void visit(Filter_Setminus const* n) const;
				virtual void visit(Filter_Enclosing_Scopes const* n) const;
				virtual void visit(Primitive_Filter const* n) const;

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

		virtual void visit(FQL_Node const* n);
		virtual void visit(FQL_Node const* n) const;
		virtual void visit(Query const* n) const;
		virtual void visit(Test_Goal_Sequence const* n) const;
		virtual void visit(Test_Goal_Set const* n);
		virtual void visit(Test_Goal_Set const* n) const;
		virtual void visit(Restriction_Automaton const* n) const;
		virtual void visit(Abstraction const* n) const;
		virtual void visit(Predicate const* n) const;
		virtual void visit(TGS_Union const* n) const;
		virtual void visit(TGS_Intersection const* n) const;
		virtual void visit(TGS_Setminus const* n) const;
		virtual void visit(Edgecov const* n) const;
		virtual void visit(Pathcov const* n) const;
		virtual void visit(Filter const* n);
		virtual void visit(Filter const* n) const;
		virtual void visit(Filter_Identity const* n) const;
		virtual void visit(Filter_Complement const* n) const;
		virtual void visit(Filter_Union const* n) const;
		virtual void visit(Filter_Intersection const* n) const;
		virtual void visit(Filter_Setminus const* n) const;
		virtual void visit(Filter_Enclosing_Scopes const* n) const;
		virtual void visit(Primitive_Filter const* n) const;

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
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter const* a, Filter const* b)
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

#define LT_VISIT_BUILDER_CPP_BASE( CL, CODE ) \
void FQL_Node_Lt_Visitor::visit( CL const* a ) \
{ \
	CODE; \
} \
\
void FQL_Node_Lt_Visitor::visit( CL const* a ) const \
{ \
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false); \
} \
\
template <typename T> \
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit( CL const* a ) const \
{ \
	FSHELL2_AUDIT_ASSERT(::diagnostics::Not_Implemented, false); \
} DUMMY_FUNC

#define LT_VISIT_BUILDER_1( CL ) \
	LT_VISIT_BUILDER_CPP_BASE(CL, m_lt = cmp_lt( a, m_other ))

#define LT_VISIT_BUILDER_TPL( CL ) \
template <typename T> \
void FQL_Node_Lt_Visitor::Compare_To_Visitor<T>::visit( CL const* n ) \
{ \
	m_inner_lt = m_comparison.cmp_lt< T, CL >( m_other_node, n ); \
} DUMMY_FUNC

/*
this is the order we use (smallest element is Query), idea is set inclusion over test goal sets
Query
Test_Goal_Sequence
Restriction_Automaton
TGS_Setminus
TGS_Intersection
TGS_Union
Pathcov
Edgecov
Abstraction
Predicate
Filter_Identity
Filter_Setminus
Filter_Intersection
Filter_Union
Filter_Enclosing_Scopes
Filter_Complement
Primitive_Filter
*/

/*
Scripted build:
for c in Query Test_Goal_Sequence Restriction_Automaton TGS_Setminus TGS_Intersection TGS_Union Pathcov Edgecov Abstraction Predicate Filter_Identity Filter_Setminus Filter_Intersection Filter_Union Filter_Enclosing_Scopes Filter_Complement Primitive_Filter ; do res="false" ; for c2 in Query Test_Goal_Sequence Restriction_Automaton TGS_Setminus TGS_Intersection TGS_Union Pathcov Edgecov Abstraction Predicate Filter_Identity Filter_Setminus Filter_Intersection Filter_Union Filter_Enclosing_Scopes Filter_Complement Primitive_Filter ; do echo -e "template<>\ninline bool FQL_Node_Lt_Visitor::cmp_lt($c const* a, $c2 const* b)\n{"; if [ "$c" = "$c2" ] ; then res="true" ; else echo -e "\treturn $res;" ; fi ; echo -e "}\n" ; done ; echo -e "LT_COMPARISON_BUILDER($c);\n" ; done
*/

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Restriction_Automaton const* b)
{
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, TGS_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, TGS_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, TGS_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Pathcov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Edgecov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Abstraction const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Predicate const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Restriction_Automaton const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Restriction_Automaton);


template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Query const* b)
{
	return false;
}

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
		
	if (a->get_suffix_automaton() != b->get_suffix_automaton()) {
		if (a->get_suffix_automaton() && !b->get_suffix_automaton()) return true;
		if (!a->get_suffix_automaton() && b->get_suffix_automaton()) return false;

		if (cmp_lt(a->get_suffix_automaton(), b->get_suffix_automaton())) return true;
		if (cmp_lt(b->get_suffix_automaton(), a->get_suffix_automaton())) return false;
	}

	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Restriction_Automaton const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, TGS_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, TGS_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, TGS_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Pathcov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Edgecov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Abstraction const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Predicate const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Test_Goal_Sequence const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Test_Goal_Sequence);

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

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Test_Goal_Sequence const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Restriction_Automaton const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, TGS_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, TGS_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, TGS_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Pathcov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Edgecov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Abstraction const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Predicate const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Query const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Query);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, TGS_Setminus const* b)
{
	if (cmp_lt(a->get_tgs_a(), b->get_tgs_a())) return true;
	if (cmp_lt(b->get_tgs_a(), a->get_tgs_a())) return false;

	return !cmp_lt(a->get_tgs_b(), b->get_tgs_b());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, TGS_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, TGS_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Pathcov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Edgecov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Abstraction const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Predicate const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Setminus const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(TGS_Setminus);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, TGS_Intersection const* b)
{
	if (cmp_lt(a->get_tgs_a(), b->get_tgs_a())) return true;
	if (cmp_lt(b->get_tgs_a(), a->get_tgs_a())) return false;

	return cmp_lt(a->get_tgs_b(), b->get_tgs_b());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, TGS_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Pathcov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Edgecov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Abstraction const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Predicate const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Intersection const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(TGS_Intersection);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, TGS_Union const* b)
{
	if (cmp_lt(a->get_tgs_a(), b->get_tgs_a())) return true;
	if (cmp_lt(b->get_tgs_a(), a->get_tgs_a())) return false;

	return cmp_lt(a->get_tgs_b(), b->get_tgs_b());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Pathcov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Edgecov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Abstraction const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Predicate const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(TGS_Union const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(TGS_Union);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Edgecov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Abstraction const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Predicate const* b)
{
	FSHELL2_PROD_CHECK(::diagnostics::Not_Implemented, false);
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Predicate const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Predicate);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Edgecov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Abstraction const* b)
{
	if (a->get_predicates().size() != b->get_predicates().size())
		return a->get_predicates().size() < b->get_predicates().size();
	for (Abstraction::preds_t::const_iterator itera(a->get_predicates().begin()),
			iterb(b->get_predicates().begin()); itera != a->get_predicates().end();
			++itera, ++iterb) {
		if (*itera == *iterb) continue;
		if (cmp_lt(*itera, *iterb)) return true;
		if (cmp_lt(*iterb, *itera)) return false;
	}
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Predicate const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Abstraction const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Abstraction);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Pathcov const* b)
{
	if (cmp_lt(a->get_abstraction(), b->get_abstraction())) return true;
	if (cmp_lt(b->get_abstraction(), a->get_abstraction())) return false;

	if (cmp_lt(a->get_filter(), b->get_filter())) return true;
	if (cmp_lt(b->get_filter(), a->get_filter())) return false;
	
	return (a->get_bound() < b->get_bound());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Edgecov const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Abstraction const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Predicate const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Pathcov const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Pathcov);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Edgecov const* b)
{
	if (cmp_lt(a->get_abstraction(), b->get_abstraction())) return true;
	if (cmp_lt(b->get_abstraction(), a->get_abstraction())) return false;

	return cmp_lt(a->get_filter(), b->get_filter());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Abstraction const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Predicate const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Filter_Identity const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Edgecov const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Edgecov);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Edgecov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Abstraction const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Predicate const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Filter_Identity const* b)
{
	// kind of a singleton
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Filter_Setminus const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Identity const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Filter_Identity);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Edgecov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Abstraction const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Predicate const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Filter_Identity const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Filter_Setminus const* b)
{
	if (cmp_lt(a->get_filter_a(), b->get_filter_a())) return true;
	if (cmp_lt(b->get_filter_a(), a->get_filter_a())) return false;

	return !cmp_lt(a->get_filter_b(), b->get_filter_b());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Filter_Intersection const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Setminus const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Filter_Setminus);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Edgecov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Abstraction const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Predicate const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Filter_Identity const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Filter_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Filter_Intersection const* b)
{
	if (cmp_lt(a->get_filter_a(), b->get_filter_a())) return true;
	if (cmp_lt(b->get_filter_a(), a->get_filter_a())) return false;

	return cmp_lt(a->get_filter_b(), b->get_filter_b());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Filter_Union const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Intersection const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Filter_Intersection);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Edgecov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Abstraction const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Predicate const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Filter_Identity const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Filter_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Filter_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Filter_Union const* b)
{
	if (cmp_lt(a->get_filter_a(), b->get_filter_a())) return true;
	if (cmp_lt(b->get_filter_a(), a->get_filter_a())) return false;

	return cmp_lt(a->get_filter_b(), b->get_filter_b());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Filter_Enclosing_Scopes const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Union const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Filter_Union);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Edgecov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Abstraction const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Predicate const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Filter_Identity const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Filter_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Filter_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Filter_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Filter_Enclosing_Scopes const* b)
{
	return cmp_lt(a->get_filter(), b->get_filter());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Filter_Complement const* b)
{
	return true;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Enclosing_Scopes const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Filter_Enclosing_Scopes);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Edgecov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Abstraction const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Predicate const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Filter_Identity const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Filter_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Filter_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Filter_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Filter_Enclosing_Scopes const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Filter_Complement const* b)
{
	return cmp_lt(b->get_filter(), a->get_filter());
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Filter_Complement const* a, Primitive_Filter const* b)
{
	return true;
}

LT_COMPARISON_BUILDER(Filter_Complement);

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Query const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Test_Goal_Sequence const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Restriction_Automaton const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, TGS_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, TGS_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, TGS_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Pathcov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Edgecov const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Abstraction const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Predicate const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Filter_Identity const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Filter_Setminus const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Filter_Intersection const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Filter_Union const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Filter_Enclosing_Scopes const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Filter_Complement const* b)
{
	return false;
}

template<>
inline bool FQL_Node_Lt_Visitor::cmp_lt(Primitive_Filter const* a, Primitive_Filter const* b)
{
	if (a->get_filter_type() != b->get_filter_type())
		return a->get_filter_type() < b->get_filter_type();

	switch (a->get_filter_type()) {
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
	}

	return false;
}

LT_COMPARISON_BUILDER(Primitive_Filter);


LT_VISIT_BUILDER_TPL(Query);
LT_VISIT_BUILDER_TPL(Test_Goal_Sequence);
LT_VISIT_BUILDER_TPL(Restriction_Automaton);
LT_VISIT_BUILDER_TPL(TGS_Setminus);
LT_VISIT_BUILDER_TPL(TGS_Intersection);
LT_VISIT_BUILDER_TPL(TGS_Union);
LT_VISIT_BUILDER_TPL(Pathcov);
LT_VISIT_BUILDER_TPL(Edgecov);
LT_VISIT_BUILDER_TPL(Abstraction);
LT_VISIT_BUILDER_TPL(Predicate);
LT_VISIT_BUILDER_TPL(Filter_Identity);
LT_VISIT_BUILDER_TPL(Filter_Setminus);
LT_VISIT_BUILDER_TPL(Filter_Intersection);
LT_VISIT_BUILDER_TPL(Filter_Union);
LT_VISIT_BUILDER_TPL(Filter_Enclosing_Scopes);
LT_VISIT_BUILDER_TPL(Filter_Complement);
LT_VISIT_BUILDER_TPL(Primitive_Filter);

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__FQL_NODE_LT_VISITOR_HPP */
