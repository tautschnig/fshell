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

////////////////////////////////////////////////////////////////////////////////
/**
 * @file fshell2/fql/parser/parser.yy
 *
 * @brief YACC/bison based parser
 *
 *
 * $Id: parser.yy 1384 2009-02-18 12:33:41Z michaelz $
 *
 * @author Michael Tautschnig <tautschn@model.in.tum.de>
 * @date   Thu Feb 19 23:41:39 CET 2009
*/

%{
#include <iostream>

#ifndef yyFlexLexer
#  include <FlexLexer.h>
#endif

extern "C"
{
  void yyerror( yyFlexLexer *, ::std::ostream *, 
    ::fortas::query::parser::Command **, ::fortas::query::parser::QL_AST_Node **, char* );
}

#define yylex() lexer->yylex()

/* #define YYDEBUG 1 */

FSHELL2_NAMESPACE_BEGIN;
FSHELL2_FQL_NAMESPACE_BEGIN;
FSHELL2_FQL_PARSER_NAMESPACE_BEGIN;

::std::ostream * parser_err_stream;

FSHELL2_FQL_PARSER_NAMESPACE_END;
FSHELL2_FQL_NAMESPACE_END;
FSHELL2_NAMESPACE_END;

using namespace ::fshell2::fql::parser;

%}

%error-verbose

%parse-param { yyFlexLexer * lexer }
%parse-param { ::std::ostream * os }
%parse-param { ::fortas::query::parser::Command ** cmd }
%parse-param { ::fortas::query::parser::QL_AST_Node ** ast_root }

%initial-action { FORTAS_AUDIT_PROCEDURE_GUARD( "" ); parser_err_stream = os; }

%%

Statement: /* empty */

%%

void yyerror( yyFlexLexer *, ::std::ostream *, 
  ::fortas::query::parser::Command **, ::fortas::query::parser::QL_AST_Node **,
  char* errstr )
{
  FSHELL2_PROD_CHECK1( ::fshell2::Parse_Error, false, ::fshell2::to_string( 
    errstr, "; type \"help\" to get usage information" ) );
  /* *(::fortas::query::parser::parser_err_stream) << "Error: " << errstr << 
    "; type \"help\" to get usage information" << ::std::endl; */
}

