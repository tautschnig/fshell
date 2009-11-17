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

#ifndef FSHELL2__FQL__AST__PM_PRECONDITION_HPP
#define FSHELL2__FQL__AST__PM_PRECONDITION_HPP

/*! \file fshell2/fql/ast/pm_precondition.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Aug  2 19:02:36 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/path_monitor_expr.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class PM_Precondition : public Path_Monitor_Expr
{
	/*! \copydoc doc_self
	*/
	typedef PM_Precondition Self;

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

	inline Path_Monitor_Expr const * get_path_monitor_expr() const;
	inline Predicate const * get_predicate() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Path_Monitor_Expr *, Predicate *);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	Path_Monitor_Expr * m_path_monitor_expr;
	Predicate * m_predicate;

	/*! Constructor
	*/
	PM_Precondition(Path_Monitor_Expr * pm, Predicate * pred);

	/*! \copydoc copy_constructor
	*/
	PM_Precondition( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~PM_Precondition();
};

inline Path_Monitor_Expr const * PM_Precondition::get_path_monitor_expr() const {
	return m_path_monitor_expr;
}

inline Predicate const * PM_Precondition::get_predicate() const {
	return m_predicate;
}

template <>
inline PM_Precondition * FQL_Node_Factory<PM_Precondition>::create(Path_Monitor_Expr * pm, Predicate * pred) {
	if (m_available.empty()) {
		m_available.push_back(new PM_Precondition(pm, pred));
	}

	m_available.back()->m_path_monitor_expr = pm;
	m_available.back()->m_predicate = pred;
	::std::pair< ::std::set<PM_Precondition *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		pm->incr_ref_count();
		pred->incr_ref_count();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__PM_PRECONDITION_HPP */
