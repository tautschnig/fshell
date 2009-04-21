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

#ifndef FSHELL2__FQL__AST__FILTER_ENCLOSING_SCOPES_HPP
#define FSHELL2__FQL__AST__FILTER_ENCLOSING_SCOPES_HPP

/*! \file fshell2/fql/ast/filter_enclosing_scopes.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Apr 21 23:48:56 CEST 2009 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/fql_node.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

FSHELL2_NAMESPACE_BEGIN;
      FSHELL2_FQL_NAMESPACE_BEGIN;
      
/*! \brief TODO
*/
class Filter_Enclosing_Scopes : public FQL_Node
{
	/*! \copydoc doc_self
	*/
	typedef Filter_Enclosing_Scopes Self;

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
  Filter_Enclosing_Scopes();

	/*! \copydoc copy_constructor
	*/
	Filter_Enclosing_Scopes( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
		
  /*! \brief Destructor
  */
  virtual ~Filter_Enclosing_Scopes();
};

template <>
inline Filter_Enclosing_Scopes * FQL_Node_Factory<Filter_Enclosing_Scopes>::create() {
}

FSHELL2_FQL_NAMESPACE_END;
      FSHELL2_NAMESPACE_END;
      
#endif /* FSHELL2__FQL__AST__FILTER_ENCLOSING_SCOPES_HPP */
