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

#ifndef FSHELL2__FQL__AST__FQL_NODE_FACTORY_HPP
#define FSHELL2__FQL__AST__FQL_NODE_FACTORY_HPP

/*! \file fshell2/fql/ast/fql_node_factory.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Mon Apr 20 23:54:15 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <diagnostics/basic_exceptions/invalid_argument.hpp>

#include <set>
#include <list>
#include <vector>

class exprt;

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Filter;
class Path_Monitor;
class Predicate;
class Test_Goal_Set;
class Test_Goal_Sequence;

typedef enum {
	F_IDENTITY = 0,
	F_FILE = 1,
	F_LINE = 2,
	F_COLUMN = 3,
	F_FUNC = 4,
	F_LABEL = 5,
	F_CALL = 6,
	F_CALLS = 7,
	F_ENTRY = 8,
	F_EXIT = 9,
	F_EXPR = 10,
	F_REGEXP = 11,
	F_BASICBLOCKENTRY = 12,
	F_CONDITIONEDGE = 13,
	F_DECISIONEDGE = 14,
	F_CONDITIONGRAPH = 15
} filter_function_t;

/*! \brief TODO
*/
template <typename Element>
class FQL_Node_Factory
{
	/*! \copydoc doc_self
	*/
	typedef FQL_Node_Factory<Element> Self;

	public:
	static Self & get_instance();

	// Edgecov, Statecov
	Element * create(Filter * filter, ::std::set<Predicate *, FQL_Node_Lt_Compare> * predicates);
	// Filter_Complement, Filter_Enclosing_Scopes, PM_Filter_Adapter
	Element * create(Filter * filter);
	// Filter_Compose, Filter_Intersection, Filter_Setminus, Filter_Union
	Element * create(Filter * a, Filter * b);
	// Filter_Function
	template <filter_function_t Filter_Function>
	Element * create();
	template <filter_function_t Filter_Function>
	Element * create(int val);
	template <filter_function_t Filter_Function>
	Element * create(::std::string const& val);
	// Pathcov
	Element * create(Filter * filter, int bound, ::std::set<Predicate *, FQL_Node_Lt_Compare> * predicates);
	// PM_Alternative, PM_Concat, PM_Next
	Element * create(Path_Monitor * a, Path_Monitor * b);
	// PM_Repeat
	Element * create(Path_Monitor * pm, int lower, int upper);
	// Predicate
	Element * create(::exprt * expr);
	// Query
	Element * create(Filter * prefix, Test_Goal_Sequence * cover,
			Path_Monitor * passing);
	// Test_Goal_Sequence
	Element * create(::std::list< ::std::pair< Path_Monitor *, Test_Goal_Set * > > & seq,
			Path_Monitor * suffix_aut);
	// TGS_Intersection, TGS_Setminus, TGS_Union
	Element * create(Test_Goal_Set * a, Test_Goal_Set * b);
	
	void destroy(Element * e);

	/*! \brief Destructor
	*/
	virtual ~FQL_Node_Factory();

	private:
	typename ::std::set<Element *, FQL_Node_Lt_Compare> m_used;
	typename ::std::vector<Element *> m_available;

	FQL_Node_Factory();
	/*! \copydoc copy_constructor
	*/
	FQL_Node_Factory( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

template <typename Element>
FQL_Node_Factory<Element>& FQL_Node_Factory<Element>::get_instance() {
	static Self instance;
	return instance;
}

template <typename Element>
FQL_Node_Factory<Element>::FQL_Node_Factory() {
}

template <typename Element>
FQL_Node_Factory<Element>::~FQL_Node_Factory() {
	for (typename ::std::set<Element *, FQL_Node_Lt_Compare>::const_iterator iter(m_used.begin());
			iter != m_used.end(); ++iter)
		delete *iter;
	
	for (typename ::std::vector<Element *>::const_iterator iter(m_available.begin());
			iter != m_available.end(); ++iter)
		delete *iter;
}

template <typename Element>
void FQL_Node_Factory<Element>::destroy(Element * e) {
	typename ::std::set<Element *>::const_iterator entry(m_used.find(e));
	FSHELL2_DEBUG_ASSERT(::diagnostics::Invalid_Argument, entry != m_used.end());
	m_used.erase(entry);
	m_available.push_back(e);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__FQL_NODE_FACTORY_HPP */
