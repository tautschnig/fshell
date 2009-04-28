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

/**
 * @file fshell2/fql/parser/lexer.ll
 *
 * @brief Lex/flex based lexer
 * 
 * The implementation is set to be case-insensitive
 *
 * $Id$
 *
 * @author Michael Tautschnig <tautschnig@forsyte.de>
 * @date   Thu Feb 19 23:41:27 CET 2009
*/

/* in a query */
%start query_scope
%start query_cover
%start query_passing

%{

#ifdef __ICC
#  define YY_NO_UNISTD_H
#elif ! __APPLE__
/* flex redefines isatty without an exception specification, but unistd.h 
does; this breaks compilation with -pedantic */
#  define isatty( t ) isatty( t ) throw()
#endif

#ifdef WIN32
#  define YY_NO_UNISTD_H
#  include <errno.h>
#  include <iostream>
  using ::std::cerr;
  using ::std::cin;
  using ::std::cout;
  using ::std::istream;
  using ::std::ostream;
#endif

#define yyparse FQLparse
#define yyerror FQLerror
#define yynerrs FQLnerrs
#define yylval FQLlval
#define yychar FQLchar

#include <fshell2/fql/ast/fql_node.hpp>

namespace fshell2 {
  namespace fql {
    class Abstraction;
	class Filter;
	class Predicate;
	class Restriction_Automaton;
	class Test_Goal_Sequence;
	class Test_Goal_Set;
  }
}

#include <set>
#include <list>

class exprt;

#include <fshell2/fql/parser/parser.h>

#include <fshell2/config/annotations.hpp>
#include <diagnostics/basic_exceptions/parse_error.hpp>

extern ::std::set< ::fshell2::fql::FQL_Node * > intermediates;

%}

%option c++
%option noyywrap
%option case-insensitive
%option prefix="FQL"

/* general */
TOK_L_PARENTHESIS       \(
TOK_R_PARENTHESIS       \)
TOK_COMMA               ,
/* predicate generators */
TOK_FILE                @FILE
TOK_LINE                @LINE
TOK_COLUMN              @COLUMN
TOK_FUNC                @FUNC
TOK_LABEL               @LABEL
TOK_CALL                @CALL
TOK_CALLS               @CALLS
TOK_ENTRY               (@ENTRY|\^)
TOK_EXIT                (@EXIT|\$)
TOK_EXPR                @EXPR
TOK_REGEXP              @REGEXP
TOK_BASICBLOCKENTRY     @BASICBLOCKENTRY
TOK_CONDITIONEDGE       @CONDITIONEDGE
TOK_DECISIONEDGE        @DECISIONEDGE
TOK_CONDITIONGRAPH      @CONDITIONGRAPH
/* operations on pred generators */
TOK_COMPLEMENT          COMPLEMENT
TOK_UNION               UNION
TOK_INTERSECT           INTERSECT
TOK_SETMINUS            SETMINUS
TOK_ENCLOSING_SCOPES    ENCLOSING_SCOPES
TOK_IDENTITY            IDENTITY
/* abstraction builders */
TOK_CFG                 CFG
TOK_PREDICATE           @PRED
TOK_L_BRACE             \{
TOK_R_BRACE             \}
TOK_GREATER_OR_EQ       >=
TOK_GREATER             >
TOK_EQ                  ==
TOK_LESS_OR_EQ          <=
TOK_LESS                <
TOK_NEQ                 !=
/* coverage specification */
TOK_EDGECOV             EDGECOV
TOK_PATHCOV             PATHCOV
/* automaton construction */
TOK_NEXT                ->
TOK_NEXT_START          -\[
TOK_NEXT_END            \]>
TOK_SUCH_THAT           \.
/* query */
TOK_IN                  IN
TOK_COVER               COVER
TOK_PASSING             PASSING
/* C identifier */
TOK_C_IDENT             [_a-zA-Z][_a-zA-Z0-9]*
/* a quoted string (no newline) */
TOK_QUOTED_STRING       \"[^\"]*\"
/* a natural number */
TOK_NAT_NUMBER          [0-9]+

%%

[ \t\n]                               /* NOOP */
\/\/.*                                /* NOOP */
{TOK_L_PARENTHESIS}   { return TOK_L_PARENTHESIS; }
{TOK_R_PARENTHESIS}   { return TOK_R_PARENTHESIS; }
{TOK_COMMA}   { return TOK_COMMA; }

<query_scope,query_cover,query_passing>{TOK_FILE}   { return TOK_FILE; }
<query_scope,query_cover,query_passing>{TOK_LINE}   { return TOK_LINE; }
<query_scope,query_cover,query_passing>{TOK_COLUMN}   { return TOK_COLUMN; }
<query_scope,query_cover,query_passing>{TOK_FUNC}   { return TOK_FUNC; }
<query_scope,query_cover,query_passing>{TOK_LABEL}   { return TOK_LABEL; }
<query_scope,query_cover,query_passing>{TOK_CALL}   { return TOK_CALL; }
<query_scope,query_cover,query_passing>{TOK_CALLS}   { return TOK_CALLS; }
<query_scope,query_cover,query_passing>{TOK_ENTRY}   { return TOK_ENTRY; }
<query_scope,query_cover,query_passing>{TOK_EXIT}   { return TOK_EXIT; }
<query_scope,query_cover,query_passing>{TOK_EXPR}   { return TOK_EXPR; }
<query_scope,query_cover,query_passing>{TOK_REGEXP}   { return TOK_REGEXP; }
<query_cover>{TOK_BASICBLOCKENTRY}   { return TOK_BASICBLOCKENTRY; }
<query_cover>{TOK_CONDITIONEDGE}   { return TOK_CONDITIONEDGE; }
<query_cover>{TOK_DECISIONEDGE}   { return TOK_DECISIONEDGE; }
<query_cover>{TOK_CONDITIONGRAPH}   { return TOK_CONDITIONGRAPH; }

<query_scope,query_cover,query_passing>{TOK_IDENTITY}   { return TOK_IDENTITY; }
<query_scope,query_cover,query_passing>{TOK_COMPLEMENT}   { return TOK_COMPLEMENT; }
<query_scope,query_cover,query_passing>{TOK_UNION}   { return TOK_UNION; }
<query_scope,query_cover,query_passing>{TOK_INTERSECT}   { return TOK_INTERSECT; }
<query_scope,query_cover,query_passing>{TOK_SETMINUS}   { return TOK_SETMINUS; }
<query_scope,query_cover,query_passing>{TOK_ENCLOSING_SCOPES}   { return TOK_ENCLOSING_SCOPES; }

<query_cover>{TOK_CFG}   { return TOK_CFG; }
<query_cover>{TOK_PREDICATE}   { return TOK_PREDICATE; }
<query_cover>{TOK_L_BRACE}   { return TOK_L_BRACE; }
<query_cover>{TOK_R_BRACE}   { return TOK_R_BRACE; }
<query_cover,query_passing>{TOK_GREATER_OR_EQ}   { return TOK_GREATER_OR_EQ; }
<query_cover>{TOK_GREATER}   { return TOK_GREATER; }
<query_cover>{TOK_EQ}   { return TOK_EQ; }
<query_cover,query_passing>{TOK_LESS_OR_EQ}   { return TOK_LESS_OR_EQ; }
<query_cover>{TOK_LESS}   { return TOK_LESS; }
<query_cover>{TOK_NEQ}   { return TOK_NEQ; }

<query_cover>{TOK_EDGECOV}   { return TOK_EDGECOV; }
<query_cover>{TOK_PATHCOV}   { return TOK_PATHCOV; }

<query_cover,query_passing>{TOK_NEXT}   { return TOK_NEXT; }
<query_cover,query_passing>{TOK_NEXT_START}   { return TOK_NEXT_START; }
<query_cover,query_passing>{TOK_NEXT_END}   { return TOK_NEXT_END; }
<query_cover,query_passing>{TOK_SUCH_THAT}   { return TOK_SUCH_THAT; }

<INITIAL>{TOK_IN}   { BEGIN query_scope; return TOK_IN; }
<INITIAL,query_scope>{TOK_COVER}   { BEGIN query_cover; return TOK_COVER; }
<query_cover>{TOK_PASSING}   { BEGIN query_passing; return TOK_PASSING; }

<query_scope,query_cover,query_passing>{TOK_C_IDENT}  { 
                                        yylval.STRING = strdup( yytext );
                                        FSHELL2_PROD_ASSERT1(::diagnostics::Parse_Error,
                                          yylval.STRING != 0, "Out of memory" );
                                        return TOK_C_IDENT; 
                                      }
<query_scope,query_cover,query_passing>{TOK_QUOTED_STRING}	{
                                        /* only return the string between the quotes */
                                        int len = strlen( yytext );
                                        yylval.STRING = static_cast<char*>(malloc((len - 1)*sizeof(char)));
                                        FSHELL2_PROD_ASSERT1(::diagnostics::Parse_Error,
                                          yylval.STRING != 0, "Out of memory" );
                                        strncpy( yylval.STRING, yytext + sizeof(char), (len - 2)*sizeof(char) ); 
                                        yylval.STRING[ len - 2 ] = '\0';
                                        return TOK_QUOTED_STRING; 
                                      }
<query_cover,query_passing>{TOK_NAT_NUMBER}				  { 
                                        yylval.NUMBER = strtol( yytext, 0, 10 );
                                        FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, 
                                          EINVAL != errno, "Invalid number" );
                                        FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, 
                                          ERANGE != errno, "Number out of range" );
                                        return TOK_NAT_NUMBER; 
                                      }

.                                     {
										for (::std::set< ::fshell2::fql::FQL_Node * >::iterator
											iter(intermediates.begin()); iter != intermediates.end(); ++iter)
										  if (*iter) (*iter)->destroy();
										intermediates.clear();
                                        FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, false,
                                          ::diagnostics::internal::to_string( "Unexpected character ", yytext, 
                                          " in line ", yylineno, "; type \"help\" to get usage information" ));
                                      }

%%

