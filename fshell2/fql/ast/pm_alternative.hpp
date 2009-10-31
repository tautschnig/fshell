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

#ifndef FSHELL2__FQL__AST__PM_ALTERNATIVE_HPP
#define FSHELL2__FQL__AST__PM_ALTERNATIVE_HPP

/*! \file fshell2/fql/ast/pm_alternative.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Aug  2 19:01:03 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/path_monitor_expr.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class PM_Alternative : public Path_Monitor_Expr
{
	/*! \copydoc doc_self
	*/
	typedef PM_Alternative Self;

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

	inline Path_Monitor_Expr const * get_path_monitor_expr_a() const;
	inline Path_Monitor_Expr const * get_path_monitor_expr_b() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Path_Monitor_Expr *, Path_Monitor_Expr *);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	Path_Monitor_Expr * m_path_monitor_expr_a;
	Path_Monitor_Expr * m_path_monitor_expr_b;

	/*! Constructor
	*/
	PM_Alternative(Path_Monitor_Expr * a, Path_Monitor_Expr * b);

	/*! \copydoc copy_constructor
	*/
	PM_Alternative( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~PM_Alternative();
};

inline Path_Monitor_Expr const * PM_Alternative::get_path_monitor_expr_a() const {
	return m_path_monitor_expr_a;
}

inline Path_Monitor_Expr const * PM_Alternative::get_path_monitor_expr_b() const {
	return m_path_monitor_expr_b;
}

template <>
inline PM_Alternative * FQL_Node_Factory<PM_Alternative>::create(Path_Monitor_Expr * path_monitor_expr_a, Path_Monitor_Expr * path_monitor_expr_b) {
	if (m_available.empty()) {
		m_available.push_back(new PM_Alternative(path_monitor_expr_a, path_monitor_expr_b));
	}

	m_available.back()->m_path_monitor_expr_a = path_monitor_expr_a;
	m_available.back()->m_path_monitor_expr_b = path_monitor_expr_b;
	::std::pair< ::std::set<PM_Alternative *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		path_monitor_expr_a->incr_ref_count();
		path_monitor_expr_b->incr_ref_count();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__PM_ALTERNATIVE_HPP */
