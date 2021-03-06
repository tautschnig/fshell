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

/*! \file fshell2/fql/parser/query_parser.cpp
 * \brief TODO
 *
 * $Id$
 * \author Michael Tautschnig <tautschnig@forsyte.de>
 * \date   Thu Apr  2 17:29:47 CEST 2009 
*/

#include <fshell2/fql/parser/query_parser.hpp>
#include <fshell2/config/annotations.hpp>
#include <fshell2/exception/query_processing_error.hpp>

#if FSHELL2_DEBUG__LEVEL__ > -1
#  include <diagnostics/basic_exceptions/violated_invariance.hpp>
#endif

#include <sstream>
#include <cerrno>

/* parser */
#define yyFlexLexer FQLFlexLexer
#include <FlexLexer.h>

extern int FQLparse(FQLFlexLexer *, ::std::ostream *, ::fshell2::fql::Query **);
// extern int yydebug;
/* end parser */

#include <fshell2/fql/parser/grammar.hpp>

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

Query_Parser::Query_Parser() {
}

::std::ostream & Query_Parser::help(::std::ostream & os) {
	os << "FQL:" << ::std::endl << FQL_HELP << ::std::endl;
	return os;
}
	
void Query_Parser::parse(::std::ostream & os, char const * query, Query ** query_ast) {
	// new lexer
	FQLFlexLexer lexer;
	// put the input into a stream
	::std::istringstream is(query);
	// set the stream as the input to the parser
	lexer.switch_streams(&is, &os);
	// reset errno, readline for some reason sets this to EINVAL
	errno = 0;
	// try to parse
	try {
		int parse(0);
		parse = FQLparse(&lexer, &os, query_ast);
		if(0 != parse)
			FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance, false);
	} catch (::fshell2::Query_Processing_Error & e) {
		FSHELL2_PROD_CHECK1(Query_Processing_Error, false,
				::std::string("Query parsing failed: ") + e.what());
	}
}

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

