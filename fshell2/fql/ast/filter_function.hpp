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

#ifndef FSHELL2__FQL__AST__FILTER_FUNCTION_HPP
#define FSHELL2__FQL__AST__FILTER_FUNCTION_HPP

/*! \file fshell2/fql/ast/filter_function.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:56 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/filter.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Filter_Function : public Filter
{
	/*! \copydoc doc_self
	*/
	typedef Filter_Function Self;

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

	inline filter_function_t const get_filter_type() const;
	template <filter_function_t Filter_Function>
	inline int const get_int_arg() const;
	template <filter_function_t Filter_Function>
	inline ::std::string const& get_string_arg() const;

	private:
	template <filter_function_t Filter_Function>
	friend Self * FQL_Node_Factory<Self>::create();
	template <filter_function_t Filter_Function>
	friend Self * FQL_Node_Factory<Self>::create(int val);
	template <filter_function_t Filter_Function>
	friend Self * FQL_Node_Factory<Self>::create(::std::string const& val);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	filter_function_t m_type;
	int m_int_value;
	::std::string m_string_value;


	/*! Constructor
	*/
	Filter_Function(filter_function_t type, int val, ::std::string const& str_val);

	/*! \copydoc copy_constructor
	*/
	Filter_Function( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Filter_Function();
};

inline filter_function_t const Filter_Function::get_filter_type() const {
	return m_type;
}

#define FILTER_NO_ARG(subT) \
template <> \
template <> \
inline Filter_Function * FQL_Node_Factory<Filter_Function>::create<subT>() { \
	if (m_available.empty()) { \
		m_available.push_back(new Filter_Function(subT, -1, "")); \
	} \
\
	m_available.back()->m_type = subT; \
	::std::pair< ::std::set<Filter_Function *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted( \
			m_used.insert(m_available.back())); \
	if (inserted.second) { \
		m_available.pop_back(); \
	} \
\
	return *(inserted.first); \
} DUMMY_FUNC

#define FILTER_INT_ARG(subT) \
template <> \
inline int const Filter_Function::get_int_arg<subT>() const { \
	return m_int_value; \
} \
\
template <> \
template <> \
inline Filter_Function * FQL_Node_Factory<Filter_Function>::create<subT>(int val) { \
	if (m_available.empty()) { \
		m_available.push_back(new Filter_Function(subT, val, "")); \
	} \
\
	m_available.back()->m_type = subT; \
	m_available.back()->m_int_value = val; \
	::std::pair< ::std::set<Filter_Function *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted( \
			m_used.insert(m_available.back())); \
	if (inserted.second) { \
		m_available.pop_back(); \
	} \
\
	return *(inserted.first); \
} DUMMY_FUNC

#define FILTER_STRING_ARG(subT) \
template <> \
inline ::std::string const& Filter_Function::get_string_arg<subT>() const { \
    return m_string_value; \
} \
\
template <> \
template <> \
inline Filter_Function * FQL_Node_Factory<Filter_Function>::create<subT>(::std::string const& val) { \
	if (m_available.empty()) { \
		m_available.push_back(new Filter_Function(subT, -1, val)); \
	} \
\
	m_available.back()->m_type = subT; \
	m_available.back()->m_string_value = val; \
	::std::pair< ::std::set<Filter_Function *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted( \
			m_used.insert(m_available.back())); \
	if (inserted.second) { \
		m_available.pop_back(); \
	} \
\
	return *(inserted.first); \
} DUMMY_FUNC
		
FILTER_NO_ARG(F_IDENTITY);
FILTER_STRING_ARG(F_FILE);
FILTER_INT_ARG(F_LINE);
FILTER_INT_ARG(F_COLUMN);
FILTER_STRING_ARG(F_FUNC);
FILTER_STRING_ARG(F_LABEL);
FILTER_STRING_ARG(F_CALL);
FILTER_NO_ARG(F_CALLS);
FILTER_STRING_ARG(F_ENTRY);
FILTER_STRING_ARG(F_EXIT);
FILTER_STRING_ARG(F_EXPR);
FILTER_STRING_ARG(F_REGEXP);
FILTER_NO_ARG(F_BASICBLOCKENTRY);
FILTER_NO_ARG(F_CONDITIONEDGE);
FILTER_NO_ARG(F_DECISIONEDGE);
FILTER_NO_ARG(F_CONDITIONGRAPH);

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__FILTER_FUNCTION_HPP */
