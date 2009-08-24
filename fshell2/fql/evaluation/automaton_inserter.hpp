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

#ifndef FSHELL2__FQL__EVALUATION__AUTOMATON_INSERTER_HPP
#define FSHELL2__FQL__EVALUATION__AUTOMATON_INSERTER_HPP

/*! \file fshell2/fql/evaluation/automaton_inserter.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:32:01 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/instrumentation/goto_transformation.hpp>
#include <fshell2/fql/evaluation/evaluate_path_monitor.hpp>

#include <list>
#include <map>

#include <cbmc/src/goto-programs/goto_functions.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

class CFG;

FSHELL2_INSTRUMENTATION_NAMESPACE_END;

FSHELL2_FQL_NAMESPACE_BEGIN;

class Evaluate_Filter;

/*! \brief TODO
*/
class Automaton_Inserter
{
	/*! \copydoc doc_self
	*/
	typedef Automaton_Inserter Self;

	public:
	typedef ::std::list< ::fshell2::instrumentation::GOTO_Transformation::goto_node_t > instrumentation_points_t;
	typedef ::std::map< Evaluate_Path_Monitor::trace_automaton_t::state_type,
			instrumentation_points_t > instrumentation_map_t;

	Automaton_Inserter(Evaluate_Path_Monitor const& pm_eval,
			Evaluate_Filter const& filter_eval,
			::goto_functionst & gf,
			::fshell2::instrumentation::CFG & cfg, ::contextt & context);

	void insert();

	instrumentation_points_t const& get_test_goal_instrumentation(
			Evaluate_Path_Monitor::trace_automaton_t::state_type const& state) const;

	private:
	Evaluate_Path_Monitor const& m_pm_eval;
	Evaluate_Filter const& m_filter_eval;
	::goto_functionst & m_gf;
	::contextt & m_context;
	::fshell2::instrumentation::CFG & m_cfg;
	::fshell2::instrumentation::GOTO_Transformation m_inserter;
	instrumentation_map_t m_tg_instrumentation_map;

	void insert(char const * suffix, Evaluate_Path_Monitor::trace_automaton_t const& aut,
			::exprt & final_cond, bool map_tg);

	/*! \copydoc copy_constructor
	*/
	Automaton_Inserter( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__AUTOMATON_INSERTER_HPP */
