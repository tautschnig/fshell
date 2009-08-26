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

/*! \file fshell2/instrumentation/cfg.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Fri Aug 21 12:59:48 CEST 2009 
*/

#include <fshell2/instrumentation/cfg.hpp>

#include <cbmc/src/util/std_expr.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

CFG::CFG() {
}

void CFG::compute_edges(
  goto_functionst &goto_functions,
  goto_programt &goto_program,
  goto_programt::targett PC)
{
  goto_programt::instructiont &instruction=*PC;
  entry_t &entry=m_entries[PC];
  goto_programt::targett next_PC(PC);
  next_PC++;

  // compute forward edges first
  if(instruction.is_goto())
  {
    if(next_PC!=goto_program.instructions.end() &&
       !instruction.guard.is_true())
      entry.successors.push_back(::std::make_pair(&goto_program, next_PC));

    for(goto_programt::instructiont::targetst::iterator
        t_it=instruction.targets.begin();
        t_it!=instruction.targets.end();
        t_it++)
    {
      goto_programt::targett t=*t_it;
      if(t!=goto_program.instructions.end())
        entry.successors.push_back(::std::make_pair(&goto_program, t));
    }
  }
  else if(instruction.is_start_thread())
  {
    if(next_PC!=goto_program.instructions.end())
      entry.successors.push_back(::std::make_pair(&goto_program, PC));
      
    for(goto_programt::instructiont::targetst::iterator
        t_it=instruction.targets.begin();
        t_it!=instruction.targets.end();
        t_it++)
    {
      goto_programt::targett t=*t_it;
      if(t!=goto_program.instructions.end())
        entry.successors.push_back(::std::make_pair(&goto_program, t));
    }
  }
  else if(instruction.is_function_call())
  {
    const exprt &function=
      to_code_function_call(instruction.code).function();

    if(function.id()=="symbol")
    {
      const irep_idt &identifier=
        to_symbol_expr(function).get_identifier();

      goto_functionst::function_mapt::iterator f_it=
        goto_functions.function_map.find(identifier);

      if(f_it!=goto_functions.function_map.end() &&
         f_it->second.body_available)
      {
        // get the first instruction
        goto_programt::targett i_it=
          f_it->second.body.instructions.begin();

        goto_programt::targett e_it=
          f_it->second.body.instructions.end();

        if(i_it!=e_it)
          entry.successors.push_back(::std::make_pair(&(f_it->second.body), i_it));

        // add the last instruction as predecessor of the return location
        if(next_PC!=goto_program.instructions.end())
        {
          m_entries[--e_it].successors.push_back(::std::make_pair(&goto_program, next_PC));
          m_entries[next_PC].predecessors.push_back(::std::make_pair(&(f_it->second.body), --e_it));
        }
      }
      else if(next_PC!=goto_program.instructions.end())
        entry.successors.push_back(::std::make_pair(&goto_program, next_PC));
    }
  }
  else
  {
    if(next_PC!=goto_program.instructions.end())
      entry.successors.push_back(::std::make_pair(&goto_program, next_PC));
  }

  // now do backward edges
  for(successors_t::iterator
      s_it=entry.successors.begin();
      s_it!=entry.successors.end();
      s_it++)
  {
    m_entries[s_it->second].predecessors.push_back(::std::make_pair(&goto_program, PC));
  }
}

void CFG::compute_edges(
  goto_functionst &goto_functions,
  goto_programt &goto_program)
{
  Forall_goto_program_instructions(it, goto_program)
    compute_edges(goto_functions, goto_program, it);
}

void CFG::compute_edges(
  goto_functionst &goto_functions)
{
  Forall_goto_functions(it, goto_functions)
    if(it->second.body_available)
      compute_edges(goto_functions, it->second.body);
}

FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;
