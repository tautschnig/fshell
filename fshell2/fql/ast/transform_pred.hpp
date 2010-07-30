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

#ifndef FSHELL2__FQL__AST__TRANSFORM_PRED_HPP
#define FSHELL2__FQL__AST__TRANSFORM_PRED_HPP

/*! \file fshell2/fql/ast/edgecov.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:55 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/filter_expr.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

#include <fshell2/fql/ast/predicate.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Transform_Pred : public Filter_Expr
{
	/*! \copydoc doc_self
	*/
	typedef Transform_Pred Self;

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
	inline Predicate::preds_t const * get_predicates() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Filter_Expr * filter_expr, Predicate::preds_t * predicates);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	Filter_Expr * m_filter_expr;
	Predicate::preds_t * m_predicates;

	/*! Constructor
	*/
	Transform_Pred(Filter_Expr * filter_expr, Predicate::preds_t * predicates);

	/*! \copydoc copy_constructor
	*/
	Transform_Pred( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Transform_Pred();
};

inline Filter_Expr const * Transform_Pred::get_filter_expr() const {
	return m_filter_expr;
}

inline Predicate::preds_t const * Transform_Pred::get_predicates() const {
	return m_predicates;
}

template <>
inline Transform_Pred * FQL_Node_Factory<Transform_Pred>::create(Filter_Expr * filter_expr, Predicate::preds_t * predicates) {
	if (m_available.empty()) {
		m_available.push_back(new Transform_Pred(filter_expr, predicates));
	}

	m_available.back()->m_filter_expr = filter_expr;
	m_available.back()->m_predicates = predicates;
	::std::pair< ::std::set<Transform_Pred *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
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

#endif /* FSHELL2__FQL__AST__EDGECOV_HPP */
