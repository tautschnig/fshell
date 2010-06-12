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

/*! \file fshell2/instrumentation/goto_utils.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Wed Nov 11 08:27:38 CET 2009 
*/

#include <fshell2/instrumentation/goto_utils.hpp>

#include <cbmc/src/util/expr.h>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_INSTRUMENTATION_NAMESPACE_BEGIN;

void find_symbols(::exprt & expr, ::std::list< ::exprt * > & symbols) {
	if (expr.id() == ID_symbol)
		symbols.push_back(&expr);
	else {
		Forall_operands(iter, expr) find_symbols(*iter, symbols);
	}
}

void find_symbols(::exprt const& expr, ::std::list< ::exprt const * > & symbols) {
	if (expr.id() == ID_symbol)
		symbols.push_back(&expr);
	else {
		forall_operands(iter, expr) find_symbols(*iter, symbols);
	}
}

void collect_expr_with_parents(::exprt const& expr, ::std::map< ::exprt const*, ::exprt const* > & exprs) {
	exprs.insert(::std::make_pair< ::exprt const*, ::exprt const*>(&expr, 0)); // will fail most of the time
	forall_operands(iter, expr) {
			exprs.insert(::std::make_pair(&(*iter), &expr));
			collect_expr_with_parents(*iter, exprs);
	}
}

FSHELL2_INSTRUMENTATION_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

