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

#ifndef FSHELL2__FQL__EVALUATION__EVALUATE_PATH_MONITOR_HPP
#define FSHELL2__FQL__EVALUATION__EVALUATE_PATH_MONITOR_HPP

/*! \file fshell2/fql/evaluation/evaluate_path_monitor.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug  7 10:29:44 CEST 2009 
*/

#include <fshell2/config/config.hpp>

#include <fshell2/fql/ast/ast_visitor.hpp>
#include <fshell2/fql/ast/standard_ast_visitor_aspect.hpp>

#include <map>

#include <astl/include/nfa_mmap_backedge.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;


/*! \brief TODO
*/
class Evaluate_Path_Monitor : public Standard_AST_Visitor_Aspect<AST_Visitor>
{
	/*! \copydoc doc_self
	*/
	typedef Evaluate_Path_Monitor Self;

	public:

	class Filter_Index {
		typedef Filter_Index Self;
		public:
		typedef int char_type;

		Filter_Index();

		int to_index(Filter const* f);

		static bool lt(const char_type x, const char_type y) {
			return x < y;
		}

		int id_index() const {
			return 0;
		}
	
		Filter const* lookup_index(int index) const;

		private:
		::std::map< Filter const*, int > m_filter_to_int;
		::std::map< int, Filter const* > m_int_to_filter;
		int m_next_index;

		Filter_Index(Self const& rhs);
		Self& operator=(Self const& rhs);
	};

	typedef ::astl::NFA_mmap_backedge< Filter_Index > trace_automaton_t;

	Evaluate_Path_Monitor();

	virtual ~Evaluate_Path_Monitor();

	trace_automaton_t const& get_cov_seq_aut() const;
	trace_automaton_t const& get_passing_aut() const;

	inline Filter const* lookup_index(int index) const;
	inline int id_index() const;

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Edgecov
	 * \param  n Edgecov
	 */
	virtual void visit(Edgecov const* n);
	/*! \} */
	
	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Alternative
	 * \param  n PM_Alternative
	 */
	virtual void visit(PM_Alternative const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Concat
	 * \param  n PM_Concat
	 */
	virtual void visit(PM_Concat const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Filter_Adapter
	 * \param  n PM_Filter_Adapter
	 */
	virtual void visit(PM_Filter_Adapter const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Next
	 * \param  n PM_Next
	 */
	virtual void visit(PM_Next const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::PM_Repeat
	 * \param  n PM_Repeat
	 */
	virtual void visit(PM_Repeat const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Pathcov
	 * \param  n Pathcov
	 */
	virtual void visit(Pathcov const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Query
	 * \param  n Query
	 */
	virtual void visit(Query const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Predicate
	 * \param  n Predicate
	 */
	virtual void visit(Predicate const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Statecov
	 * \param  n Statecov
	 */
	virtual void visit(Statecov const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::TGS_Intersection
	 * \param  n TGS_Intersection
	 */
	virtual void visit(TGS_Intersection const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::TGS_Setminus
	 * \param  n TGS_Setminus
	 */
	virtual void visit(TGS_Setminus const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::TGS_Union
	 * \param  n TGS_Union
	 */
	virtual void visit(TGS_Union const* n);
	/*! \} */

	/*! \{
	 * \brief Visit a @ref fshell2::fql::Test_Goal_Sequence
	 * \param  n Test_Goal_Sequence
	 */
	virtual void visit(Test_Goal_Sequence const* n);
	/*! \} */

	private:
	Filter_Index m_filter_index;
	trace_automaton_t m_cov_seq_aut;
	trace_automaton_t m_passing_aut;

	trace_automaton_t * m_current_aut;
	trace_automaton_t::state_type m_current_initial;
	trace_automaton_t::state_type m_current_final;

	void simplify(trace_automaton_t & aut);

	/*! \copydoc copy_constructor
	*/
	Evaluate_Path_Monitor( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

Filter const* Evaluate_Path_Monitor::lookup_index(int index) const {
	return m_filter_index.lookup_index(index);
}
	
int Evaluate_Path_Monitor::id_index() const {
	return m_filter_index.id_index();
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__EVALUATION__EVALUATE_PATH_MONITOR_HPP */
