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
 * @file fshell2/command/parser.yy
 *
 * @brief YACC/bison based parser
 *
 *
 * $Id$
 *
 * @author Michael Tautschnig <tautschn@model.in.tum.de>
 * @date   Thu Feb 19 23:41:39 CET 2009
*/

%{
#include <iostream>
#include <cstring>

#undef yyFlexLexer
#define yyFlexLexer CMDFlexLexer
#include <FlexLexer.h>

#define yyparse CMDparse
#define yyerror CMDerror
#define yynerrs CMDnerrs
#define yylval CMDlval
#define yychar CMDchar

#include <fshell2/command/command_processing.hpp>

extern "C"
{
  void yyerror(CMDFlexLexer *, ::fshell2::command::Command_Processing::parsed_command_t &,
  	char **, int*, ::std::list< ::std::pair< char*, char* > > &, char const*);
}

#define yylex() lexer->yylex()

/* #define YYDEBUG 1 */

%}

%parse-param { CMDFlexLexer * lexer }
%parse-param { ::fshell2::command::Command_Processing::parsed_command_t & cmd }
%parse-param { char ** arg }
%parse-param { int * numeric_arg }
%parse-param { ::std::list< ::std::pair< char*, char* > > & defines }

%initial-action { *arg = 0; *numeric_arg = -1; }

%union
{
  int NUMBER;
  char* STRING;
}

/* commands */
%token TOK_QUIT
%token TOK_HELP
/* preparing the CFG */
%token TOK_ADD
%token TOK_SOURCECODE
%token TOK_DEFINE
%token TOK_EQ
/* display the loaded sources */
%token TOK_SHOW
%token TOK_ALL
%token TOK_FILENAMES
/* setting options */
%token TOK_SET
%token TOK_ENTRY
%token TOK_LIMIT
%token TOK_COUNT
%token TOK_NO_ZERO_INIT
/* C identifier */
%token <STRING> TOK_C_IDENT
/* a quoted string (no newline) */
%token <STRING> TOK_QUOTED_STRING
/* a natural number */
%token <NUMBER> TOK_NAT_NUMBER
/* invalid characters */
%token TOK_INVALID

%%

Command: TOK_QUIT
       {
	     cmd = ::fshell2::command::Command_Processing::CMD_QUIT;
       }
       | TOK_HELP
       {
	   	 cmd = ::fshell2::command::Command_Processing::CMD_HELP;
       }
       | TOK_ADD TOK_SOURCECODE Defines TOK_QUOTED_STRING
       {
	     cmd = ::fshell2::command::Command_Processing::CMD_ADD_SOURCECODE;
		 *arg = $4;
       }
       | TOK_SHOW TOK_FILENAMES
       {
	     cmd = ::fshell2::command::Command_Processing::CMD_SHOW_FILENAMES;
       }
       | TOK_SHOW TOK_SOURCECODE TOK_QUOTED_STRING
       {
	     cmd = ::fshell2::command::Command_Processing::CMD_SHOW_SOURCECODE;
		 *arg = $3;
       }
       | TOK_SHOW TOK_SOURCECODE TOK_ALL
       {
	     cmd = ::fshell2::command::Command_Processing::CMD_SHOW_SOURCECODE_ALL;
       }
       | TOK_SET Options
	   ;

Defines: /* empty */
       | Defines TOK_DEFINE TOK_C_IDENT
       {
         defines.push_back( ::std::make_pair( $3, ::strdup( "1" ) ) );
       }
       | Defines TOK_DEFINE TOK_C_IDENT TOK_EQ TOK_QUOTED_STRING
       {
         defines.push_back( ::std::make_pair( $3, $5 ) );
       }

Options: TOK_ENTRY TOK_C_IDENT
       {
	     cmd = ::fshell2::command::Command_Processing::CMD_SET_ENTRY;
		 *arg = $2;
       }
	   | TOK_LIMIT TOK_COUNT TOK_NAT_NUMBER
	   {
	     cmd = ::fshell2::command::Command_Processing::CMD_SET_LIMIT_COUNT;
		 *numeric_arg = $3;
	   }
	   | TOK_NO_ZERO_INIT
	   {
	     cmd = ::fshell2::command::Command_Processing::CMD_SET_NO_ZERO_INIT;
	   }
       ;


%%

void yyerror(CMDFlexLexer *, ::fshell2::command::Command_Processing::parsed_command_t &,
	char **, int *, ::std::list< ::std::pair< char*, char* > > &, char const*)
{
	// no-op
}

