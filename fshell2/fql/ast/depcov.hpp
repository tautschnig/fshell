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

#ifndef FSHELL2__FQL__AST__DEPCOV_HPP
#define FSHELL2__FQL__AST__DEPCOV_HPP

/*! \file fshell2/fql/ast/depcov.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:55 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/ecp_atom.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

#include <fshell2/fql/ast/filter_expr.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Depcov : public ECP_Atom
{
	/*! \copydoc doc_self
	*/
	typedef Depcov Self;

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

	inline Filter_Expr const * get_filter_a() const;
	inline Filter_Expr const * get_filter_b() const;
	inline Filter_Expr const * get_filter_c() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Filter_Expr * a, Filter_Expr * b, Filter_Expr * c);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory();

	Filter_Expr * m_filter_a;
	Filter_Expr * m_filter_b;
	Filter_Expr * m_filter_c;

	/*! Constructor
	*/
	Depcov(Filter_Expr * a, Filter_Expr * b, Filter_Expr * c);

	/*! \copydoc copy_constructor
	*/
	Depcov( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Depcov();
};

inline Filter_Expr const * Depcov::get_filter_a() const {
	return m_filter_a;
}

inline Filter_Expr const * Depcov::get_filter_b() const {
	return m_filter_b;
}

inline Filter_Expr const * Depcov::get_filter_c() const {
	return m_filter_c;
}

template <>
inline Depcov * FQL_Node_Factory<Depcov>::create(Filter_Expr * a, Filter_Expr * b, Filter_Expr * c) {
	if (m_available.empty()) {
		m_available.push_back(new Depcov(a, b, c));
	}

	m_available.back()->m_filter_a = a;
	m_available.back()->m_filter_b = b;
	m_available.back()->m_filter_c = c;
	::std::pair< ::std::set<Depcov *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		a->incr_ref_count();
		b->incr_ref_count();
		c->incr_ref_count();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__DEPCOV_HPP */
