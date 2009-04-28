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

#ifndef FSHELL2__FQL__AST__FILTER_INTERSECTION_HPP
#define FSHELL2__FQL__AST__FILTER_INTERSECTION_HPP

/*! \file fshell2/fql/ast/filter_intersection.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:55 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/filter.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Filter_Intersection : public Filter
{
	/*! \copydoc doc_self
	*/
	typedef Filter_Intersection Self;

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

	inline Filter const * get_filter_a() const;
	inline Filter const * get_filter_b() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Filter *, Filter *);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	Filter * m_filter_a;
	Filter * m_filter_b;

	/*! Constructor
	*/
	Filter_Intersection(Filter * a, Filter * b);

	/*! \copydoc copy_constructor
	*/
	Filter_Intersection( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Filter_Intersection();
};

inline Filter const * Filter_Intersection::get_filter_a() const {
	return m_filter_a;
}

inline Filter const * Filter_Intersection::get_filter_b() const {
	return m_filter_b;
}

template <>
inline Filter_Intersection * FQL_Node_Factory<Filter_Intersection>::create(Filter * filter_a, Filter * filter_b) {
	if (m_available.empty()) {
		m_available.push_back(new Filter_Intersection(filter_a, filter_b));
	}

	m_available.back()->m_filter_a = filter_a;
	m_available.back()->m_filter_b = filter_b;
	::std::pair< ::std::set<Filter_Intersection *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		filter_a->incr_ref_count();
		filter_b->incr_ref_count();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__FILTER_INTERSECTION_HPP */
