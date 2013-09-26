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

#ifndef FSHELL2__FQL__AST__FILTER_UNION_HPP
#define FSHELL2__FQL__AST__FILTER_UNION_HPP

/*! \file fshell2/fql/ast/filter_union.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:55 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/filter_expr.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Filter_Union : public Filter_Expr
{
	/*! \copydoc doc_self
	*/
	typedef Filter_Union Self;

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

	inline Filter_Expr const * get_filter_expr_a() const;
	inline Filter_Expr const * get_filter_expr_b() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Filter_Expr *, Filter_Expr *);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory();

	Filter_Expr * m_filter_expr_a;
	Filter_Expr * m_filter_expr_b;

	/*! Constructor
	*/
	Filter_Union(Filter_Expr * a, Filter_Expr * b);

	/*! \copydoc copy_constructor
	*/
	Filter_Union( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Filter_Union();
};

inline Filter_Expr const * Filter_Union::get_filter_expr_a() const {
	return m_filter_expr_a;
}

inline Filter_Expr const * Filter_Union::get_filter_expr_b() const {
	return m_filter_expr_b;
}

template <>
inline Filter_Union * FQL_Node_Factory<Filter_Union>::create(Filter_Expr * filter_expr_a, Filter_Expr * filter_expr_b) {
	if (m_available.empty()) {
		m_available.push_back(new Filter_Union(filter_expr_a, filter_expr_b));
	}

	m_available.back()->m_filter_expr_a = filter_expr_a;
	m_available.back()->m_filter_expr_b = filter_expr_b;
	::std::pair< ::std::set<Filter_Union *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		filter_expr_a->incr_ref_count();
		filter_expr_b->incr_ref_count();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__FILTER_UNION_HPP */
