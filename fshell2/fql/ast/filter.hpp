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

#ifndef FSHELL2__FQL__AST__FILTER_HPP
#define FSHELL2__FQL__AST__FILTER_HPP

/*! \file fshell2/fql/ast/filter.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:55 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/fql_node.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class Filter : public FQL_Node
{
	/*! \copydoc doc_self
	*/
	typedef Filter Self;

	public:
	typedef FQL_Node_Factory<Self> Factory;

	/*! \{
	 * \brief Accept a visitor 
	 * \param  v Visitor
	 */
	virtual void accept(AST_Visitor * v) const = 0;
	virtual void accept(AST_Visitor const * v) const = 0;
	/*! \} */

	virtual bool destroy() = 0;

	/*! Constructor
	*/
	Filter();

	/*! \brief Destructor
	*/
	virtual ~Filter();

	private:
	/*! \copydoc copy_constructor
	*/
	Filter( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

};

/*! \brief TODO
*/
class Filter_Identity : public Filter
{
	/*! \copydoc doc_self
	*/
	typedef Filter_Identity Self;

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

	private:
	friend Self * FQL_Node_Factory<Self>::create();
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

	/*! Constructor
	*/
	Filter_Identity();

	/*! \copydoc copy_constructor
	*/
	Filter_Identity( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

	/*! \brief Destructor
	*/
	virtual ~Filter_Identity();
};

template <>
inline Filter_Identity * FQL_Node_Factory<Filter_Identity>::create() {
	if (m_used.empty()) {
		if (m_available.empty()) {
			m_used.insert(new Filter_Identity());
		} else {
			m_used.insert(m_available.back());
			m_available.pop_back();
		}
	}

	return *(m_used.begin());
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__FILTER_HPP */
