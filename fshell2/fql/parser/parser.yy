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
 * $Id$
 *
 * @author Michael Tautschnig <tautschn@model.in.tum.de>
 * @date   Thu Feb 19 23:41:39 CET 2009
*/

%{
#include <iostream>
#undef yyFlexLexer
#define yyFlexLexer FQLFlexLexer
#include <FlexLexer.h>

#include <fshell2/config/annotations.hpp>
#include <diagnostics/basic_exceptions/parse_error.hpp>

#define yyparse FQLparse
#define yyerror FQLerror
#define yynerrs FQLnerrs
#define yylval FQLlval
#define yychar FQLchar

extern "C"
{
  void yyerror(yyFlexLexer *, ::std::ostream *, char const*);
}

#define yylex() lexer->yylex()

/* #define YYDEBUG 1 */

%}

%error-verbose

%parse-param { yyFlexLexer * lexer }
%parse-param { ::std::ostream * os }

%union
{
  int NUMBER;
  char* STRING;
}

/* general */
%token TOK_L_PARENTHESIS
%token TOK_R_PARENTHESIS
%token TOK_COMMA
/* predicate generators */
%token TOK_FILE
%token TOK_LINE
%token TOK_COLUMN
%token TOK_FUNC
%token TOK_LABEL
%token TOK_CALL
%token TOK_CALLS
%token TOK_ENTRY
%token TOK_EXIT
%token TOK_EXPR
%token TOK_REGEXP
%token TOK_BASICBLOCKENTRY
%token TOK_CONDITIONEDGE
%token TOK_DECISIONEDGE
%token TOK_CONDITIONGRAPH
/* operations on pred generators */
%token TOK_IDENTITY
%token TOK_COMPLEMENT
%token TOK_UNION
%token TOK_INTERSECT
%token TOK_SETMINUS
%token TOK_ENCLOSING_SCOPES
/* abstraction builders */
%token TOK_CFG
%token TOK_PREDICATE
%token TOK_L_BRACE
%token TOK_R_BRACE
%token TOK_GREATER_OR_EQ
%token TOK_GREATER
%token TOK_EQ
%token TOK_LESS_OR_EQ
%token TOK_LESS
%token TOK_NEQ
/* coverage specification */
%token TOK_EDGECOV
%token TOK_PATHCOV
/* automaton construction */
%token TOK_NEXT
%token TOK_NEXT_START
%token TOK_NEXT_END
%token TOK_SUCH_THAT
/* query */
%token TOK_IN
%token TOK_COVER
%token TOK_PASSING
/* C identifier */
%token <STRING> TOK_C_IDENT
/* a quoted string */
%token <STRING> TOK_QUOTED_STRING
/* a natural number */
%token <NUMBER> TOK_NAT_NUMBER

%%

Query: Scope Cover Passing
	 ;

Scope: /* empty */
	 | TOK_IN Filter
	 ;

Cover: TOK_COVER Test_Goal_Sequence
	 ;

Passing: /* empty */
	   | TOK_PASSING Automaton
	   ;

Test_Goal_Sequence: Test_Goal_Set
				  | Test_Goal_Sequence Automaton Test_Goal_Set
				  ;

Automaton: TOK_L_PARENTHESIS Automaton TOK_R_PARENTHESIS
		 | Aut_Step
		 | Automaton TOK_NEXT Aut_Step
		 | Automaton TOK_NEXT_START Aut_Step TOK_NEXT_END Aut_Step
		 | Automaton TOK_LESS_OR_EQ TOK_NAT_NUMBER
		 | Automaton TOK_GREATER_OR_EQ TOK_NAT_NUMBER
		 ;

Aut_Step: Filter
		| Predicate
		| Filter TOK_SUCH_THAT Predicate
		;

Test_Goal_Set: TOK_L_PARENTHESIS Test_Goal_Set TOK_R_PARENTHESIS
			 | TOK_UNION TOK_L_PARENTHESIS Test_Goal_Set TOK_COMMA Test_Goal_Set TOK_R_PARENTHESIS
			 | TOK_INTERSECT TOK_L_PARENTHESIS Test_Goal_Set TOK_COMMA Test_Goal_Set TOK_R_PARENTHESIS
			 | TOK_SETMINUS TOK_L_PARENTHESIS Test_Goal_Set TOK_COMMA Test_Goal_Set TOK_R_PARENTHESIS
			 | TOK_EDGECOV TOK_L_PARENTHESIS Abstraction TOK_COMMA Filter TOK_R_PARENTHESIS
			 | TOK_PATHCOV TOK_L_PARENTHESIS Abstraction TOK_COMMA Filter TOK_COMMA TOK_NAT_NUMBER TOK_R_PARENTHESIS
			 ;

Abstraction: TOK_CFG
		   | TOK_CFG TOK_L_PARENTHESIS Predicates TOK_R_PARENTHESIS
		   ;

Predicates: Predicate
		  | Predicates TOK_COMMA Predicate
		  ;


Predicate: TOK_L_BRACE c_LHS Comparison c_LHS TOK_R_BRACE
		 | TOK_PREDICATE TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
		 ;

// extend
c_LHS: TOK_C_IDENT
	 | TOK_NAT_NUMBER
	 ;

Comparison: TOK_GREATER_OR_EQ
		  | TOK_GREATER
		  | TOK_EQ
		  | TOK_LESS_OR_EQ
		  | TOK_LESS
		  | TOK_NEQ
		  ;

Filter: Primitive_Filter
	  | TOK_IDENTITY
	  | TOK_COMPLEMENT TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
	  | TOK_UNION TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
      | TOK_INTERSECT TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
      | TOK_SETMINUS TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  | TOK_ENCLOSING_SCOPES TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
	  ;

Primitive_Filter: TOK_FILE TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
				| TOK_LINE TOK_L_PARENTHESIS TOK_NAT_NUMBER TOK_R_PARENTHESIS
				| TOK_COLUMN TOK_L_PARENTHESIS TOK_NAT_NUMBER TOK_R_PARENTHESIS
				| TOK_FUNC TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				| TOK_LABEL TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				| TOK_CALL TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				| TOK_CALLS
				| TOK_ENTRY TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				| TOK_EXIT TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				| TOK_EXPR TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
				| TOK_REGEXP TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
				| TOK_BASICBLOCKENTRY
				| TOK_CONDITIONEDGE
				| TOK_DECISIONEDGE
				| TOK_CONDITIONGRAPH
				;

%%

void yyerror(yyFlexLexer *, ::std::ostream *, char const* errstr)
{
  FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, false,
  ::diagnostics::internal::to_string( 
    errstr, "; type \"help\" to get usage information" ));
}

