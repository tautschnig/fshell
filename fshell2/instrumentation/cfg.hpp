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

#ifndef FSHELL2__INSTRUMENTATION__CFG_HPP
#define FSHELL2__INSTRUMENTATION__CFG_HPP

/*! \file fshell2/instrumentation/cfg.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug 21 12:59:45 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <cbmc/src/goto-programs/goto_functions.h>

#include <list>
#include <map>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

/*! \brief TODO
 *
 * Based on cbmc/src/goto-programs/cfg.h
*/
class CFG
{
	/*! \copydoc doc_self
	*/
	typedef CFG Self;

	public:
	typedef ::std::list< ::std::pair< goto_programt*, goto_programt::targett > > successors_t;
	typedef ::std::list< ::std::pair< goto_programt*, goto_programt::targett > > predecessors_t;

	struct entry_t {
		successors_t successors;
		predecessors_t predecessors;
	};

	typedef ::std::map<goto_programt::targett, entry_t> entries_t;

	void compute_edges(
			goto_functionst &goto_functions);

	CFG();

	inline entries_t::iterator find(::goto_programt::targett const& node);
	inline entries_t::const_iterator find(::goto_programt::targett const& node) const;
	inline entries_t::iterator end();
	inline entries_t::const_iterator end() const;

	private:
	entries_t m_entries;

	void compute_edges(
			goto_functionst &goto_functions,
			goto_programt &goto_program,
			goto_programt::targett PC);

	void compute_edges(
			goto_functionst &goto_functions,
			goto_programt &goto_program);

	/*! \copydoc copy_constructor
	*/
	CFG( Self const& rhs );

	/*! \copydoc assignment_op
	*/
	Self& operator=( Self const& rhs );
};
	
CFG::entries_t::iterator CFG::find(::goto_programt::targett const& node) {
	return m_entries.find(node);
}

CFG::entries_t::const_iterator CFG::find(::goto_programt::targett const& node) const {
	return m_entries.find(node);
}

CFG::entries_t::iterator CFG::end() {
	return m_entries.end();
}

CFG::entries_t::const_iterator CFG::end() const {
	return m_entries.end();
}

FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__INSTRUMENTATION__CFG_HPP */
