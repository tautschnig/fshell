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
%x stmttype

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

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;

class Filter_Expr;
class Predicate;
class Path_Pattern_Expr;
class Coverage_Pattern_Expr;
class ECP_Atom;

FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#include <set>
#include <list>

#include <fshell2/fql/parser/parser.h>

#include <fshell2/config/annotations.hpp>
#include <diagnostics/basic_exceptions/parse_error.hpp>

extern ::std::set< ::fshell2::fql::FQL_Node * > intermediates;

yy_state_type backup_state;

%}

%option c++
%option noyywrap
%option case-insensitive
%option prefix="FQL"

/* general */
TOK_L_PARENTHESIS       \(
TOK_R_PARENTHESIS       \)
TOK_COMMA               ,
/* filter functions */
TOK_IDENTITY            ID
TOK_FILE                @FILE
TOK_LINE                @LINE
TOK_LINE_ABBREV         @[0-9]+
TOK_COLUMN              @COLUMN
TOK_FUNC                @FUNC
TOK_LABEL               @LABEL
TOK_CALL                @CALL
TOK_CALLS               @CALLS
TOK_ENTRY               @ENTRY
TOK_EXIT                @EXIT
TOK_EXPR                @EXPR
TOK_REGEXP              @REGEXP
TOK_BASICBLOCKENTRY     @BASICBLOCKENTRY
TOK_CONDITIONEDGE       @CONDITIONEDGE
TOK_DECISIONEDGE        @DECISIONEDGE
TOK_CONDITIONGRAPH      @CONDITIONGRAPH
TOK_DEF                 @DEF
TOK_USE                 @USE
TOK_STMTTYPE            @STMTTYPE
TOK_STT_IF              IF
TOK_STT_FOR             FOR
TOK_STT_WHILE           WHILE
TOK_STT_SWITCH          SWITCH
TOK_STT_CONDOP          \?:
TOK_STT_ASSERT          ASSERT
/* CFA transformers */
TOK_NOT                 NOT
TOK_TGUNION             \|
TOK_TGINTERSECT         &
TOK_SETMINUS            SETMINUS
TOK_COMPOSE             COMPOSE
TOK_PRED                PRED
/* predicates */
TOK_ARBITRARY_PRED      \{[^\{\}]+\}
/* target graph interpreters */
TOK_NODECOV             NODES
TOK_EDGECOV             EDGES
TOK_PATHCOV             PATHS
TOK_DEPCOV              DEPS
/* elementary coverage patterns */
TOK_NEXT                ->
TOK_CONCAT              \.
TOK_ALTERNATIVE         \+
TOK_KLEENE              \*
TOK_DBLQUOTE            \" 
TOK_GREATER_OR_EQ       >=
TOK_EQ                  ==
TOK_LESS_OR_EQ          <=
TOK_START               \^
TOK_END                 \$
/* query */
TOK_IN                  IN
TOK_COVER               COVER
TOK_PASSING             PASSING
/* C identifier */
TOK_C_IDENT             [_a-zA-Z][_a-zA-Z0-9]*
/* a quoted string (no newline); see
 * http://dinosaur.compilertools.net/flex/flex_11.html for a more powerful
 * quoted-string lexer including support for escape sequences */
TOK_QUOTED_STRING       '[^']*'
/* a natural number */
TOK_NAT_NUMBER          [0-9]+

%%

[ \t\n]                               /* NOOP */
\/\/.*                                /* NOOP */
<*>{TOK_L_PARENTHESIS}   { return TOK_L_PARENTHESIS; }
{TOK_R_PARENTHESIS}   { return TOK_R_PARENTHESIS; }
<stmttype>{TOK_R_PARENTHESIS}   { BEGIN backup_state; return TOK_R_PARENTHESIS; }
<*>{TOK_COMMA}   { return TOK_COMMA; }

<query_cover,query_passing>{TOK_IDENTITY}   { return TOK_IDENTITY; }
<query_scope,query_cover,query_passing>{TOK_FILE}   { return TOK_FILE; }
<query_cover,query_passing>{TOK_LINE}   { return TOK_LINE; }
<query_cover,query_passing>{TOK_LINE_ABBREV}   { 
                                        yylval.NUMBER = strtol( yytext+1, 0, 10 );
                                        FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, 
                                          EINVAL != errno, "Invalid number" );
                                        FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, 
                                          ERANGE != errno, "Number out of range" );
                                        return TOK_LINE_ABBREV; 
                                      }
<query_cover,query_passing>{TOK_COLUMN}   { return TOK_COLUMN; }
<query_scope,query_cover,query_passing>{TOK_FUNC}   { return TOK_FUNC; }
<query_cover,query_passing>{TOK_LABEL}   { return TOK_LABEL; }
<query_cover,query_passing>{TOK_CALL}   { return TOK_CALL; }
<query_cover,query_passing>{TOK_CALLS}   { return TOK_CALLS; }
<query_cover,query_passing>{TOK_ENTRY}   { return TOK_ENTRY; }
<query_cover,query_passing>{TOK_EXIT}   { return TOK_EXIT; }
<query_cover,query_passing>{TOK_EXPR}   { return TOK_EXPR; }
<query_cover,query_passing>{TOK_REGEXP}   { return TOK_REGEXP; }
<query_cover,query_passing>{TOK_BASICBLOCKENTRY}   { return TOK_BASICBLOCKENTRY; }
<query_cover,query_passing>{TOK_CONDITIONEDGE}   { return TOK_CONDITIONEDGE; }
<query_cover,query_passing>{TOK_DECISIONEDGE}   { return TOK_DECISIONEDGE; }
<query_cover,query_passing>{TOK_CONDITIONGRAPH}   { return TOK_CONDITIONGRAPH; }
<query_cover,query_passing>{TOK_DEF}   { return TOK_DEF; }
<query_cover,query_passing>{TOK_USE}   { return TOK_USE; }
<query_cover,query_passing>{TOK_STMTTYPE}   { backup_state = YY_START; BEGIN stmttype; return TOK_STMTTYPE; }

<stmttype>{TOK_STT_IF}   { return TOK_STT_IF; }
<stmttype>{TOK_STT_FOR}   { return TOK_STT_FOR; }
<stmttype>{TOK_STT_WHILE}   { return TOK_STT_WHILE; }
<stmttype>{TOK_STT_SWITCH}   { return TOK_STT_SWITCH; }
<stmttype>{TOK_STT_CONDOP}   { return TOK_STT_CONDOP; }
<stmttype>{TOK_STT_ASSERT}   { return TOK_STT_ASSERT; }

<query_scope,query_cover,query_passing>{TOK_NOT}   { return TOK_NOT; }
<query_scope,query_cover,query_passing>{TOK_TGUNION}   { return TOK_TGUNION; }
<query_scope,query_cover,query_passing>{TOK_TGINTERSECT}   { return TOK_TGINTERSECT; }
<query_scope,query_cover,query_passing>{TOK_SETMINUS}   { return TOK_SETMINUS; }
<query_scope,query_cover,query_passing>{TOK_COMPOSE}   { return TOK_COMPOSE; }
<query_scope,query_cover,query_passing>{TOK_PRED}   { return TOK_PRED; }

<query_scope,query_cover,query_passing>{TOK_ARBITRARY_PRED}	{
                                        /* only return the string between { } */
                                        int len = strlen( yytext );
                                        yylval.STRING = static_cast<char*>(malloc((len - 1)*sizeof(char)));
                                        FSHELL2_PROD_ASSERT1(::diagnostics::Parse_Error,
                                          yylval.STRING != 0, "Out of memory" );
                                        strncpy( yylval.STRING, yytext + sizeof(char), (len - 2)*sizeof(char) ); 
                                        yylval.STRING[ len - 2 ] = '\0';
                                        return TOK_ARBITRARY_PRED; 
                                      }

<query_cover>{TOK_NODECOV}   { return TOK_NODECOV; }
<query_cover>{TOK_EDGECOV}   { return TOK_EDGECOV; }
<query_cover>{TOK_PATHCOV}   { return TOK_PATHCOV; }
<query_cover>{TOK_DEPCOV}   { return TOK_DEPCOV; }

<query_cover,query_passing>{TOK_NEXT}   { return TOK_NEXT; }
<query_cover,query_passing>{TOK_CONCAT}   { return TOK_CONCAT; }
<query_cover,query_passing>{TOK_ALTERNATIVE}   { return TOK_ALTERNATIVE; }
<query_cover,query_passing>{TOK_KLEENE}   { return TOK_KLEENE; }
<query_cover,query_passing>{TOK_DBLQUOTE}   { return TOK_DBLQUOTE; }
<query_cover,query_passing>{TOK_GREATER_OR_EQ}   { return TOK_GREATER_OR_EQ; }
<query_cover,query_passing>{TOK_EQ}   { return TOK_EQ; }
<query_cover,query_passing>{TOK_LESS_OR_EQ}   { return TOK_LESS_OR_EQ; }
<query_cover,query_passing>{TOK_START}   { return TOK_START; }
<query_cover,query_passing>{TOK_END}   { return TOK_END; }

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

<*>.                                  {
										for (::std::set< ::fshell2::fql::FQL_Node * >::iterator
											iter(intermediates.begin()); iter != intermediates.end(); ++iter)
										  if (*iter) (*iter)->destroy();
										intermediates.clear();
                                        FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, false,
                                          ::diagnostics::internal::to_string( "Unexpected character ", yytext, 
                                          " in line ", yylineno, "; type \"help\" to get usage information" ));
                                      }

%%

