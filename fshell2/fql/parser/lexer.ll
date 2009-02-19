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
 * $Id: lexer.ll 1134 2008-09-02 06:46:37Z tautschn $
 *
 * @author Michael Tautschnig <tautschnig@forsyte.de>
 * @date   Thu Feb 19 23:41:27 CET 2009
*/


%{
#ifndef yyFlexLexer
#  include <FlexLexer.h>
#endif

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

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
FSHELL2_FQL_PARSER_NAMESPACE_BEGIN;

extern ::std::ostream * parser_err_stream;

FSHELL2_FQL_PARSER_NAMESPACE_END;
FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

#include <fshell2/fql/parser/parser.h>

%}

%option c++
%option noyywrap
%option case-insensitive


%%

\n                                    { BEGIN INITIAL; return TOK_EOL; }
[ \t]                                 /* NOOP */
#.*                                   /* NOOP */
<INITIAL>{TOK_QUIT}				            { return TOK_QUIT; }

.                                     {
                                        FSHELL2_PROD_CHECK1( ::fshell2::Parse_Error, false,
                                          ::fshell2::to_string( "Unexpected character ", yytext, 
                                          " in line ", yylineno, "; type \"help\" to get usage information" ));
                                        /* *(::fortas::query::parser::parser_err_stream) << 
                                          "Unexpected character " << yytext << " in line " << yylineno << 
                                            "; type \"help\" to get usage information" << ::std::endl; */
                                      }

%%

