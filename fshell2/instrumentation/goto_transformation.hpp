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

#ifndef FSHELL2__INSTRUMENTATION__GOTO_TRANSFORMATION_HPP
#define FSHELL2__INSTRUMENTATION__GOTO_TRANSFORMATION_HPP

/*! \file fshell2/instrumentation/goto_transformation.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr 30 13:50:45 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <cbmc/src/goto-programs/goto_functions.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

/*! \brief TODO
*/
class GOTO_Transformation
{
	/*! \copydoc doc_self
	*/
	typedef GOTO_Transformation Self;

	public:
	typedef enum {
		BEFORE,
		AFTER
	} position_t;
	
	typedef ::std::pair< ::goto_programt *, ::goto_programt::targett > goto_node_t;
	typedef ::std::list< goto_node_t > inserted_t;
	typedef ::std::pair< goto_node_t, goto_node_t > goto_edge_t;

	GOTO_Transformation(::goto_functionst & gf);

	inserted_t const& insert(::std::string const& f, position_t const pos,
			::goto_program_instruction_typet const stmt_type, ::goto_programt const& prg);

	inserted_t const& insert(position_t const pos, goto_edge_t const& edge,
			::goto_programt const& prg);

	inserted_t & make_nondet_choice(::goto_programt & dest, int const num, ::contextt context);

	static void set_annotations(::goto_programt::const_targett src, ::goto_programt & target);
	static void mark_instrumented(::goto_programt & target);
	static void mark_instrumented(::goto_programt::targett inst);
	static bool is_instrumented(::goto_programt::const_targett inst);

	private:
	::goto_functionst & m_goto;
	inserted_t m_inserted;
	int m_nondet_var_count;

	/*! \copydoc copy_constructor
	*/
	GOTO_Transformation( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__INSTRUMENTATION__GOTO_TRANSFORMATION_HPP */
