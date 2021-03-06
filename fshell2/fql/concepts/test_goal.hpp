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

#ifndef FSHELL2__FQL__CONCEPTS__TEST_GOAL_HPP
#define FSHELL2__FQL__CONCEPTS__TEST_GOAL_HPP

/*! \file fshell2/fql/concepts/test_goal.hpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Tue Aug 31 17:01:01 CEST 2010 
*/

#include <fshell2/config/config.hpp>

#include <set>
#include <list>
#include <vector>

class literalt;

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

typedef ::std::set< ::literalt > test_goals_t;
typedef ::std::list< test_goals_t > test_goal_groups_t;
typedef ::std::vector< ::std::vector< ::literalt > > test_goal_id_map_t;
typedef test_goal_id_map_t::size_type test_goal_id_t;
typedef ::std::set< test_goal_id_t > test_goal_ids_t;

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#endif /* FSHELL2__FQL__CONCEPTS__TEST_GOAL_HPP */
