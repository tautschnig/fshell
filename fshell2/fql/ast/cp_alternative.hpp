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

#ifndef FSHELL2__FQL__AST__CP_ALTERNATIVE_HPP
#define FSHELL2__FQL__AST__CP_ALTERNATIVE_HPP

/*! \file fshell2/fql/ast/cp_alternative.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:55 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/coverage_pattern_expr.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class CP_Alternative : public Coverage_Pattern_Expr
{
	/*! \copydoc doc_self
	*/
	typedef CP_Alternative Self;

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

	inline Coverage_Pattern_Expr const * get_cp_a() const;
	inline Coverage_Pattern_Expr const * get_cp_b() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Coverage_Pattern_Expr * a, Coverage_Pattern_Expr * b);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	Coverage_Pattern_Expr * m_cp_a;
	Coverage_Pattern_Expr * m_cp_b;

	/*! Constructor
	*/
	CP_Alternative(Coverage_Pattern_Expr * a, Coverage_Pattern_Expr * b);

	/*! \copydoc copy_constructor
	*/
	CP_Alternative( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~CP_Alternative();
};

inline Coverage_Pattern_Expr const * CP_Alternative::get_cp_a() const {
	return m_cp_a;
}

inline Coverage_Pattern_Expr const * CP_Alternative::get_cp_b() const {
	return m_cp_b;
}

template <>
inline CP_Alternative * FQL_Node_Factory<CP_Alternative>::create(Coverage_Pattern_Expr * a, Coverage_Pattern_Expr * b) {
	if (m_available.empty()) {
		m_available.push_back(new CP_Alternative(a, b));
	}

	m_available.back()->m_cp_a = a;
	m_available.back()->m_cp_b = b;
	::std::pair< ::std::set<CP_Alternative *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		a->incr_ref_count();
		b->incr_ref_count();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__CP_ALTERNATIVE_HPP */
