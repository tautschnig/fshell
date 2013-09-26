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

#ifndef FSHELL2__FQL__AST__PP_CONCAT_HPP
#define FSHELL2__FQL__AST__PP_CONCAT_HPP

/*! \file fshell2/fql/ast/pp_concat.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Aug  2 19:01:50 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/path_pattern_expr.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class PP_Concat : public Path_Pattern_Expr
{
	/*! \copydoc doc_self
	*/
	typedef PP_Concat Self;

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

	inline Path_Pattern_Expr const * get_pp_a() const;
	inline Path_Pattern_Expr const * get_pp_b() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Path_Pattern_Expr *, Path_Pattern_Expr *);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory();

	Path_Pattern_Expr * m_pp_a;
	Path_Pattern_Expr * m_pp_b;

	/*! Constructor
	*/
	PP_Concat(Path_Pattern_Expr * a, Path_Pattern_Expr * b);

	/*! \copydoc copy_constructor
	*/
	PP_Concat( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~PP_Concat();
};

inline Path_Pattern_Expr const * PP_Concat::get_pp_a() const {
	return m_pp_a;
}

inline Path_Pattern_Expr const * PP_Concat::get_pp_b() const {
	return m_pp_b;
}

template <>
inline PP_Concat * FQL_Node_Factory<PP_Concat>::create(Path_Pattern_Expr * pp_a, Path_Pattern_Expr * pp_b) {
	if (m_available.empty()) {
		m_available.push_back(new PP_Concat(pp_a, pp_b));
	}

	m_available.back()->m_pp_a = pp_a;
	m_available.back()->m_pp_b = pp_b;
	::std::pair< ::std::set<PP_Concat *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		pp_a->incr_ref_count();
		pp_b->incr_ref_count();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__PP_CONCAT_HPP */
