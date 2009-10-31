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

#ifndef FSHELL2__FQL__AST__PATHCOV_HPP
#define FSHELL2__FQL__AST__PATHCOV_HPP

/*! \file fshell2/fql/ast/pathcov.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:55 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/test_goal_set.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

#include <fshell2/fql/ast/filter_expr.hpp>
#include <fshell2/fql/ast/predicate.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Pathcov : public Test_Goal_Set
{
	/*! \copydoc doc_self
	*/
	typedef Pathcov Self;

	public:
	typedef FQL_Node_Factory<Self> Factory;

	/*! \{
	 * \brief Accept a visitor 
	 * \param  v Visitor
	 */
	virtual void accept(AST_Visitor * v) const;
	virtual void accept(AST_Visitor const * v) const;
	/*! \} */

	virtual bool destroy();

	inline Filter_Expr const * get_filter_expr() const;
	inline int const get_bound() const;
	inline Predicate::preds_t const * get_predicates() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Filter_Expr * filter_expr, int bound, Predicate::preds_t * predicates);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	Filter_Expr * m_filter_expr;
	int m_bound;
	Predicate::preds_t * m_predicates;

	/*! Constructor
	*/
	Pathcov(Filter_Expr * filter_expr, int bound, Predicate::preds_t * predicates);

	/*! \copydoc copy_constructor
	*/
	Pathcov( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Pathcov();
};

inline Filter_Expr const * Pathcov::get_filter_expr() const {
	return m_filter_expr;
}

inline int const Pathcov::get_bound() const {
	return m_bound;
}

inline Predicate::preds_t const * Pathcov::get_predicates() const {
	return m_predicates;
}

template <>
inline Pathcov * FQL_Node_Factory<Pathcov>::create(Filter_Expr * filter_expr, int bound, Predicate::preds_t * predicates) {
	if (m_available.empty()) {
		m_available.push_back(new Pathcov(filter_expr, bound, predicates));
	}

	m_available.back()->m_filter_expr = filter_expr;
	m_available.back()->m_bound = bound;
	m_available.back()->m_predicates = predicates;
	::std::pair< ::std::set<Pathcov *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		filter_expr->incr_ref_count();
		if (predicates) {
			for (Predicate::preds_t::iterator iter((*inserted.first)->m_predicates->begin());
					iter != (*inserted.first)->m_predicates->end(); ++iter) {
				(*iter)->incr_ref_count();
			}
		}
	} else {
		if (predicates) {
			for (Predicate::preds_t::iterator iter(predicates->begin());
					iter != predicates->end(); ++iter) {
				(*iter)->destroy();
			}
			delete predicates;
		}
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__PATHCOV_HPP */
