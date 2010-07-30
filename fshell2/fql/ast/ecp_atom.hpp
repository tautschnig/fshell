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

#ifndef FSHELL2__FQL__AST__ECP_ATOM_HPP
#define FSHELL2__FQL__AST__ECP_ATOM_HPP

/*! \file fshell2/fql/ast/ecp_atom.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:54 CEST 2009 
 */

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/coverage_pattern_expr.hpp>
#include <fshell2/fql/ast/path_pattern_expr.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class ECP_Atom : public Coverage_Pattern_Expr, public Path_Pattern_Expr
{
	/*! \copydoc doc_self
	*/
	typedef ECP_Atom Self;

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
	ECP_Atom();

	/*! \brief Destructor
	*/
	virtual ~ECP_Atom();

	private:
	/*! \copydoc copy_constructor
	*/
	ECP_Atom( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );

};


FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__AST__ECP_ATOM_HPP */
