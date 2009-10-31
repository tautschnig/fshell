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
 * @file fshell2/command/lexer.ll
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

/* in a command */
%start command
/* setting options */
%start options

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

#define yyparse CMDparse
#define yyerror CMDerror
#define yynerrs CMDnerrs
#define yylval CMDlval
#define yychar CMDchar

#include <fshell2/command/parser.h>

#include <fshell2/config/annotations.hpp>
#include <diagnostics/basic_exceptions/parse_error.hpp>

%}

%option c++
%option noyywrap
%option case-insensitive
%option prefix="CMD"

/* commands */
TOK_QUIT                QUIT
TOK_HELP                HELP
/* preparing the CFG */
TOK_ADD                 ADD
TOK_SOURCECODE          SOURCECODE
TOK_DEFINE              -D
TOK_EQ                  =
/* display the loaded sources */
TOK_SHOW                SHOW
TOK_ALL                 ALL
TOK_FILENAMES           FILENAMES
/* setting options */
TOK_SET                 SET
TOK_ENTRY               ENTRY
TOK_LIMIT               LIMIT
TOK_COUNT               COUNT
TOK_NO_ZERO_INIT        NO_ZERO_INIT  
/* C identifier */
TOK_C_IDENT             [_a-zA-Z][_a-zA-Z0-9]*
/* a quoted string (no newline) */
TOK_QUOTED_STRING       \"[^\"]*\"
/* a natural number */
TOK_NAT_NUMBER          [0-9]+

%%

[ \t]                                 /* NOOP */
\/\/.*                                /* NOOP */

<INITIAL>{TOK_QUIT}				      { return TOK_QUIT; }
<INITIAL>{TOK_HELP}				      { return TOK_HELP; }

<INITIAL>{TOK_ADD}   { BEGIN command; return TOK_ADD; }
<command>{TOK_SOURCECODE}   { return TOK_SOURCECODE; }
<command>{TOK_DEFINE}   { return TOK_DEFINE; }
<command>{TOK_EQ}   { return TOK_EQ; }

<INITIAL>{TOK_SHOW}   { BEGIN command; return TOK_SHOW; }
<command>{TOK_ALL}   { return TOK_ALL; }
<command>{TOK_FILENAMES}   { return TOK_FILENAMES; }

<INITIAL>{TOK_SET}   { BEGIN options; return TOK_SET; }
<options>{TOK_ENTRY}   { return TOK_ENTRY; }
<options>{TOK_LIMIT}   { return TOK_LIMIT; }
<options>{TOK_COUNT}   { return TOK_COUNT; }
<options>{TOK_NO_ZERO_INIT}   { return TOK_NO_ZERO_INIT; }

<command,options>{TOK_C_IDENT}  { 
                                        yylval.STRING = strdup( yytext );
                                        FSHELL2_PROD_ASSERT1(::diagnostics::Parse_Error,
                                          yylval.STRING != 0, "Out of memory" );
                                        return TOK_C_IDENT; 
                                      }
<command>{TOK_QUOTED_STRING}	{
                                        /* only return the string between the quotes */
                                        int len = strlen( yytext );
                                        yylval.STRING = static_cast<char*>(malloc((len - 1)*sizeof(char)));
                                        FSHELL2_PROD_ASSERT1(::diagnostics::Parse_Error,
                                          yylval.STRING != 0, "Out of memory" );
                                        strncpy( yylval.STRING, yytext + sizeof(char), (len - 2)*sizeof(char) ); 
                                        yylval.STRING[ len - 2 ] = '\0';
                                        return TOK_QUOTED_STRING; 
                                      }
<options>{TOK_NAT_NUMBER}				  { 
                                        yylval.NUMBER = strtol( yytext, 0, 10 );
                                        FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, 
                                          EINVAL != errno, "Invalid number" );
                                        FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, 
                                          ERANGE != errno, "Number out of range" );
                                        return TOK_NAT_NUMBER; 
                                      }

.                                     {
										return TOK_INVALID;
									  }

%%

