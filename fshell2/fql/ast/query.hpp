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

#ifndef FSHELL2__FQL__AST__QUERY_HPP
#define FSHELL2__FQL__AST__QUERY_HPP

/*! \file fshell2/fql/ast/query.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:54 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/fql_node.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

#include <fshell2/fql/ast/filter_expr.hpp>
#include <fshell2/fql/ast/coverage_pattern_expr.hpp>
#include <fshell2/fql/ast/path_pattern_expr.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Query : public FQL_Node
{
	/*! \copydoc doc_self
	*/
	typedef Query Self;

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

	inline Filter_Expr const * get_prefix() const;
	inline Coverage_Pattern_Expr const * get_cover() const;
	inline Path_Pattern_Expr const * get_passing() const;

	private:
	friend Self * FQL_Node_Factory<Self>::create(Filter_Expr * prefix, Coverage_Pattern_Expr * cover,
			Path_Pattern_Expr * passing);
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory();

	Filter_Expr * m_prefix;
	Coverage_Pattern_Expr * m_cover;
	Path_Pattern_Expr * m_passing;

	/*! Constructor
	*/
	Query(Filter_Expr * prefix, Coverage_Pattern_Expr * cover, Path_Pattern_Expr * passing);

	/*! \copydoc copy_constructor
	*/
	Query( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Query();
};

inline Filter_Expr const * Query::get_prefix() const {
	return m_prefix;
}

inline Coverage_Pattern_Expr const * Query::get_cover() const {
	return m_cover;
}

inline Path_Pattern_Expr const * Query::get_passing() const {
	return m_passing;
}

template <>
inline Query * FQL_Node_Factory<Query>::create(Filter_Expr * prefix, Coverage_Pattern_Expr * cover,
		Path_Pattern_Expr * passing) {
	if (m_available.empty()) {
		m_available.push_back(new Query(prefix, cover, passing));
	}

	m_available.back()->m_prefix = prefix;
	m_available.back()->m_cover = cover;
	m_available.back()->m_passing = passing;
	::std::pair< ::std::set<Query *, FQL_Node_Lt_Compare>::const_iterator, bool > inserted(
			m_used.insert(m_available.back()));
	if (inserted.second) {
		m_available.pop_back();
		if (prefix) prefix->incr_ref_count();
		cover->incr_ref_count();
		if (passing) passing->incr_ref_count();
	}

	return *(inserted.first);
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__QUERY_HPP */
