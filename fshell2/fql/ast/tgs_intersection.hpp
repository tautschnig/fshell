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

#ifndef FSHELL2__FQL__AST__TGS_INTERSECTION_HPP
#define FSHELL2__FQL__AST__TGS_INTERSECTION_HPP

/*! \file fshell2/fql/ast/tgs_intersection.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:55 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/test_goal_set.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class TGS_Intersection : public Test_Goal_Set
{
	/*! \copydoc doc_self
	*/
	typedef TGS_Intersection Self;

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

	inline Test_Goal_Set const * get_tgs_a() const;
	inline Test_Goal_Set const * get_tgs_b() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Test_Goal_Set * a, Test_Goal_Set * b);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	Test_Goal_Set * m_tgs_a;
	Test_Goal_Set * m_tgs_b;

	/*! Constructor
	*/
	TGS_Intersection(Test_Goal_Set * a, Test_Goal_Set * b);

	/*! \copydoc copy_constructor
	*/
	TGS_Intersection( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~TGS_Intersection();
};

inline Test_Goal_Set const * TGS_Intersection::get_tgs_a() const {
	return m_tgs_a;
}

inline Test_Goal_Set const * TGS_Intersection::get_tgs_b() const {
	return m_tgs_b;
}

template <>
inline TGS_Intersection * FQL_Node_Factory<TGS_Intersection>::create(Test_Goal_Set * a, Test_Goal_Set * b) {
	if (m_available.empty()) {
		m_available.push_back(new TGS_Intersection(a, b));
	}

	m_available.back()->m_tgs_a = a;
	m_available.back()->m_tgs_b = b;
	::std::pair< ::std::set<TGS_Intersection *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
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

#endif /* FSHELL2__FQL__AST__TGS_INTERSECTION_HPP */
