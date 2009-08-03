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

#ifndef FSHELL2__FQL__AST__TEST_GOAL_SEQUENCE_HPP
#define FSHELL2__FQL__AST__TEST_GOAL_SEQUENCE_HPP

/*! \file fshell2/fql/ast/test_goal_sequence.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:54 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>
#include <fshell2/fql/ast/test_goal_set.hpp>

#include <fshell2/fql/ast/path_monitor.hpp>

#include <diagnostics/basic_exceptions/violated_invariance.hpp>
#include <list>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Test_Goal_Sequence : public Test_Goal_Set
{
	/*! \copydoc doc_self
	*/
	typedef Test_Goal_Sequence Self;

	public:
	typedef FQL_Node_Factory<Self> Factory;

	typedef ::std::pair< Path_Monitor *, Test_Goal_Set * > seq_entry_t;
	typedef ::std::list< seq_entry_t > seq_t;

	/*! \{
	 * \brief Accept a visitor 
	 * \param  v Visitor
	 */
	virtual void accept(AST_Visitor * v) const;
	virtual void accept(AST_Visitor const * v) const;
	/*! \} */

	virtual bool destroy();

	inline seq_t const& get_sequence() const;
	inline Path_Monitor const * get_suffix_monitor() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(seq_t & seq, Path_Monitor * suffix_mon);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	seq_t m_seq;
	Path_Monitor * m_suffix_monitor;

	/*! Constructor
	*/
	Test_Goal_Sequence(seq_t const& seq, Path_Monitor * suffix_mon);

	/*! \copydoc copy_constructor
	*/
	Test_Goal_Sequence( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Test_Goal_Sequence();
};

inline Test_Goal_Sequence::seq_t const& Test_Goal_Sequence::get_sequence() const {
	return m_seq;
}

inline Path_Monitor const * Test_Goal_Sequence::get_suffix_monitor() const {
	return m_suffix_monitor;
}

template <>
inline Test_Goal_Sequence * FQL_Node_Factory<Test_Goal_Sequence>::create(Test_Goal_Sequence::seq_t & seq,
		Path_Monitor * suffix_mon) {
	if (m_available.empty()) {
		m_available.push_back(new Test_Goal_Sequence(seq, suffix_mon));
	} else {
		FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
				m_available.back()->m_seq.empty());
		m_available.back()->m_seq.swap(seq);
		m_available.back()->m_suffix_monitor = suffix_mon;
	}

	::std::pair< ::std::set< Test_Goal_Sequence *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		for (Test_Goal_Sequence::seq_t::iterator iter((*inserted.first)->m_seq.begin());
				iter != (*inserted.first)->m_seq.end(); ++iter) {
			if (iter->first) iter->first->incr_ref_count();
			iter->second->incr_ref_count();
		}
		if ((*inserted.first)->m_suffix_monitor) (*inserted.first)->m_suffix_monitor->incr_ref_count();
	} else {
		for (Test_Goal_Sequence::seq_t::iterator iter(m_available.back()->m_seq.begin());
				iter != m_available.back()->m_seq.end(); ++iter) {
			if (iter->first) iter->first->destroy();
			iter->second->destroy();
		}
		if (m_available.back()->m_suffix_monitor) m_available.back()->m_suffix_monitor->destroy();
		m_available.back()->m_seq.clear();
		m_available.back()->m_suffix_monitor = 0;
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__TEST_GOAL_SEQUENCE_HPP */
