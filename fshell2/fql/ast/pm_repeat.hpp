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

#ifndef FSHELL2__FQL__AST__PM_REPEAT_HPP
#define FSHELL2__FQL__AST__PM_REPEAT_HPP

/*! \file fshell2/fql/ast/pm_repeat.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Sun Aug  2 19:02:36 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/path_monitor.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class PM_Repeat : public Path_Monitor
{
	/*! \copydoc doc_self
	*/
	typedef PM_Repeat Self;

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

	inline Path_Monitor const * get_path_monitor() const;
	inline int const get_lower_bound() const;
	inline int const get_upper_bound() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Path_Monitor *, int, int);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	Path_Monitor * m_path_monitor;
	int m_lower_bound, m_upper_bound;

	/*! Constructor
	*/
	PM_Repeat(Path_Monitor * pm, int lower, int upper);

	/*! \copydoc copy_constructor
	*/
	PM_Repeat( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~PM_Repeat();
};

inline Path_Monitor const * PM_Repeat::get_path_monitor() const {
	return m_path_monitor;
}

inline int const PM_Repeat::get_lower_bound() const {
	return m_lower_bound;
}

inline int const PM_Repeat::get_upper_bound() const {
	return m_upper_bound;
}

template <>
inline PM_Repeat * FQL_Node_Factory<PM_Repeat>::create(Path_Monitor * pm, int lower, int upper) {
	if (m_available.empty()) {
		m_available.push_back(new PM_Repeat(pm, lower, upper));
	}

	m_available.back()->m_path_monitor = pm;
	m_available.back()->m_lower_bound = lower;
	m_available.back()->m_upper_bound = upper;
	::std::pair< ::std::set<PM_Repeat *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		pm->incr_ref_count();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__PM_REPEAT_HPP */
