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

#include <fshell2/fql/ast/abstraction.hpp>
#include <fshell2/fql/ast/filter.hpp>

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

	inline Abstraction const * get_abstraction() const;
	inline Filter const * get_filter() const;
	inline int const get_bound() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Abstraction * abst, Filter * filter, int bound);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	Abstraction * m_abst;
	Filter * m_filter;
	int m_bound;

	/*! Constructor
	*/
	Pathcov(Abstraction * abst, Filter * filter, int bound);

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

inline Abstraction const * Pathcov::get_abstraction() const {
	return m_abst;
}

inline Filter const * Pathcov::get_filter() const {
	return m_filter;
}

inline int const Pathcov::get_bound() const {
	return m_bound;
}

template <>
inline Pathcov * FQL_Node_Factory<Pathcov>::create(Abstraction * abst, Filter * filter, int bound) {
	if (m_available.empty()) {
		m_available.push_back(new Pathcov(abst, filter, bound));
	}

	m_available.back()->m_abst = abst;
	m_available.back()->m_filter = filter;
	m_available.back()->m_bound = bound;
	::std::pair< ::std::set<Pathcov *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		abst->incr_ref_count();
		filter->incr_ref_count();
	}

	return *(inserted.first);
}


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__PATHCOV_HPP */
