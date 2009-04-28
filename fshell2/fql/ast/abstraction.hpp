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

#ifndef FSHELL2__FQL__AST__ABSTRACTION_HPP
#define FSHELL2__FQL__AST__ABSTRACTION_HPP

/*! \file fshell2/fql/ast/abstraction.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:55 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>
#include <fshell2/fql/ast/fql_node.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

#include <fshell2/fql/ast/predicate.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <set>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Predicate;

/*! \brief TODO
*/
class Abstraction : public FQL_Node
{
	/*! \copydoc doc_self
	*/
	typedef Abstraction Self;

	public:
	typedef FQL_Node_Factory<Self> Factory;

	typedef ::std::set< Predicate *, FQL_Node_Lt_Compare > preds_t;

	/*! \{
	 * \brief Accept a visitor 
	 * \param  v Visitor
	 */
	virtual void accept(AST_Visitor * v) const;
	virtual void accept(AST_Visitor const * v) const;
	/*! \} */

	virtual bool destroy();

	inline preds_t const & get_predicates() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(preds_t & predicates);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	preds_t m_predicates;

	/*! Constructor
	*/
	Abstraction(preds_t const& predicates);

	/*! \copydoc copy_constructor
	*/
	Abstraction( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Abstraction();
};
	
inline Abstraction::preds_t const & Abstraction::get_predicates() const {
	return m_predicates;
}

template <>
inline Abstraction * FQL_Node_Factory<Abstraction>::create(Abstraction::preds_t & predicates) {
	if (m_available.empty()) {
		m_available.push_back(new Abstraction(predicates));
	} else {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				m_available.back()->m_predicates.empty());
		m_available.back()->m_predicates.swap(predicates);
	}

	::std::pair< ::std::set< Abstraction *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		for (Abstraction::preds_t::iterator iter((*inserted.first)->m_predicates.begin());
				iter != (*inserted.first)->m_predicates.end(); ++iter) {
			(*iter)->incr_ref_count();
		}
	} else {
		for (Abstraction::preds_t::iterator iter(m_available.back()->m_predicates.begin());
				iter != m_available.back()->m_predicates.end(); ++iter) {
			(*iter)->destroy();
		}
		m_available.back()->m_predicates.clear();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;
      
#endif /* FSHELL2__FQL__AST__ABSTRACTION_HPP */
