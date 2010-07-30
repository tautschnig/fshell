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
#include <diagnostics/basic_exceptions/violated_invariance.hpp>

#include <set>
#include <list>

#include <fshell2/fql/ast/cp_alternative.hpp>
#include <fshell2/fql/ast/cp_concat.hpp>
#include <fshell2/fql/ast/depcov.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/nodecov.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pp_alternative.hpp>
#include <fshell2/fql/ast/pp_concat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/quote.hpp>
#include <fshell2/fql/ast/repeat.hpp>
#include <fshell2/fql/ast/transform_pred.hpp>

#include <cbmc/src/ansi-c/ansi_c_parser.h>
#include <cbmc/src/ansi-c/ansi_c_parse_tree.h>

#define yyparse FQLparse
#define yyerror FQLerror
#define yynerrs FQLnerrs
#define yylval FQLlval
#define yychar FQLchar

#define MAKE_ID_STAR FQL_CREATE3(Repeat, FQL_CREATE1(Edgecov, FQL_CREATE_FF0(F_IDENTITY)), 0, -1)

extern "C"
{
  void yyerror(yyFlexLexer *, ::std::ostream *, ::fshell2::fql::Query **, char const*);
}

#define yylex() lexer->yylex()

/* #define YYDEBUG 1 */

::std::set< ::fshell2::fql::FQL_Node * > intermediates;

%}

%error-verbose

%parse-param { yyFlexLexer * lexer }
%parse-param { ::std::ostream * os }
%parse-param { ::fshell2::fql::Query ** query_ast }

%initial-action { *query_ast = 0; }

/* ( Filter ) may be parsed as Filter or ECP_Atom; shift is preferred over
 * reduce, makes Filter the first choice */
%expect 1

%union
{
  int NUMBER;
  char* STRING;
  
  ::fshell2::fql::Filter_Expr * FILTER_EXPR;
  ::fshell2::fql::Predicate * PREDICATE;
  ::fshell2::fql::Path_Pattern_Expr * PATH_PATTERN_EXPR;
  ::fshell2::fql::Coverage_Pattern_Expr * COVERAGE_PATTERN_EXPR;
  ::fshell2::fql::ECP_Atom * ECP_ATOM;

  ::std::set< ::fshell2::fql::Predicate *,
    ::fshell2::fql::FQL_Node_Lt_Compare > * PREDICATE_SET;
}

/* general */
%token TOK_L_PARENTHESIS
%token TOK_R_PARENTHESIS
%token TOK_COMMA
/* filter functions */
%token TOK_IDENTITY
%token TOK_FILE
%token TOK_LINE
%token <NUMBER> TOK_LINE_ABBREV
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
%token TOK_DEF
%token TOK_USE
%token TOK_STMTTYPE
%token TOK_STT_IF
%token TOK_STT_FOR
%token TOK_STT_WHILE
%token TOK_STT_SWITCH
%token TOK_STT_CONDOP
%token TOK_STT_ASSERT
/* CFA transformers */
%token TOK_NOT
%left TOK_TGUNION
%left TOK_TGINTERSECT
%token TOK_SETMINUS
%token TOK_COMPOSE
%token TOK_PRED
/* predicates */
%token <STRING> TOK_ARBITRARY_PRED
/* target graph interpreters */
%token TOK_NODECOV
%token TOK_EDGECOV
%token TOK_PATHCOV
%token TOK_DEPCOV
/* elementary coverage patterns */
%token TOK_NEXT
%token TOK_CONCAT
%token TOK_ALTERNATIVE
%token TOK_KLEENE
%token TOK_DBLQUOTE
%token TOK_GREATER_OR_EQ
%token TOK_EQ
%token TOK_LESS_OR_EQ
%token TOK_START
%token TOK_END
/* query */
%token TOK_IN
%token TOK_COVER
%token TOK_PASSING
/* C identifier */
%token <STRING> TOK_C_IDENT
/* a quoted string (no newline) */
%token <STRING> TOK_QUOTED_STRING
/* a natural number */
%token <NUMBER> TOK_NAT_NUMBER

%type <FILTER_EXPR> Scope Filter Filter_Function
%type <COVERAGE_PATTERN_EXPR> Cover Coverage_Pattern Coverage_Pattern_Term Coverage_Pattern_Factor
%type <PATH_PATTERN_EXPR> Passing Path_Pattern Path_Pattern_Term Path_Pattern_Factor Start_Anchor End_Anchor
%type <ECP_ATOM> ECP_Atom
%type <PREDICATE_SET> Predicates
%type <PREDICATE> Predicate
%type <NUMBER> Stmttype Stmttypes

%%

Query: Scope Cover Passing
	 {
	   *query_ast = FQL_CREATE3(Query, $1, $2, $3);
	   intermediates.clear();
	 }
	 ;

Scope: /* empty */
	 {
	   $$ = 0;
	 }
	 | TOK_IN Filter
	 {
	   $$ = $2;
	 }
	 ;

Cover: TOK_COVER Start_Anchor Coverage_Pattern End_Anchor
	 {
	   $$ = $3;
	   if ($4) {
		 $$ = FQL_CREATE2(CP_Concat, $$, FQL_CREATE1(Quote, $4));
		 intermediates.erase($4);
	   }
	   if ($2) {
	     $$ = FQL_CREATE2(CP_Concat, FQL_CREATE1(Quote, $2), $$);
		 intermediates.erase($2);
	   }
	   intermediates.erase($3);
	   intermediates.insert($$);
	 }
	 ;

Passing: /* empty */
	   {
		 $$ = MAKE_ID_STAR;
		 intermediates.insert($$);
	   }
	   | TOK_PASSING Start_Anchor Path_Pattern End_Anchor
	   {
	     $$ = $3;
	     if ($4) {
	       $$ = FQL_CREATE2(PP_Concat, $$, $4);
	       intermediates.erase($4);
	     }
	     if ($2) {
	       $$ = FQL_CREATE2(PP_Concat, $2, $$);
	       intermediates.erase($2);
	     }
	     intermediates.erase($3);
	     intermediates.insert($$);
	   }
	   ;

Start_Anchor: /* empty */
			{
			  $$ = MAKE_ID_STAR;
			  intermediates.insert($$);
			}
			| TOK_START
			{
			  $$ = 0;
			}
			;

End_Anchor: /* empty */
		  {
		    $$ = MAKE_ID_STAR;
			intermediates.insert($$);
		  }
		  | TOK_END
		  {
		    $$ = 0;
		  }
		  ;

Coverage_Pattern: Coverage_Pattern_Term
				{
				  $$ = $1;
				}
				| Coverage_Pattern TOK_ALTERNATIVE Coverage_Pattern_Term
				{
		      	  $$ = FQL_CREATE2(CP_Alternative, $1, $3);
				  intermediates.erase($1);
				  intermediates.erase($3);
			   	  intermediates.insert($$);
				}
				;
				
Coverage_Pattern_Term: Coverage_Pattern_Factor
			         {
				       $$ = $1;
				     }
				     | Coverage_Pattern_Term TOK_CONCAT Coverage_Pattern_Factor
				     {
		      	       $$ = FQL_CREATE2(CP_Concat, $1, $3);
				       intermediates.erase($1);
				       intermediates.erase($3);
			   	       intermediates.insert($$);
				     }
				     | Coverage_Pattern_Term TOK_NEXT Coverage_Pattern_Factor
				     {
		      	       $$ = FQL_CREATE2(CP_Concat, $1, FQL_CREATE2(CP_Concat,
				       		 FQL_CREATE1(Quote, MAKE_ID_STAR), $3));
				       intermediates.erase($1);
				       intermediates.erase($3);
			           intermediates.insert($$);
				     }
				     ;

Coverage_Pattern_Factor: ECP_Atom
					   {
					     $$ = $1;
					   }
					   | TOK_L_PARENTHESIS Coverage_Pattern TOK_R_PARENTHESIS
					   {
					     $$ = $2;
					   }
					   | TOK_DBLQUOTE Path_Pattern TOK_DBLQUOTE
					   {
					     $$ = FQL_CREATE1(Quote, $2);
						 intermediates.erase($2);
						 intermediates.insert($$);
					   }
					   ;

Path_Pattern: Path_Pattern_Term
		    {
		      $$ = $1;
		    }
		    | Path_Pattern TOK_ALTERNATIVE Path_Pattern_Term
		    {
		      $$ = FQL_CREATE2(PP_Alternative, $1, $3);
			  intermediates.erase($1);
			  intermediates.erase($3);
			  intermediates.insert($$);
		    }
		    ;

Path_Pattern_Term: Path_Pattern_Factor
			     {
				   $$ = $1;
				 }
				 | Path_Pattern_Term TOK_CONCAT Path_Pattern_Factor
				 {
		      	   $$ = FQL_CREATE2(PP_Concat, $1, $3);
				   intermediates.erase($1);
				   intermediates.erase($3);
			   	   intermediates.insert($$);
				 }
				 | Path_Pattern_Term TOK_NEXT Path_Pattern_Factor
				 {
		      	  $$ = FQL_CREATE2(PP_Concat, $1, FQL_CREATE2(PP_Concat,
				  		 MAKE_ID_STAR, $3));
				   intermediates.erase($1);
				   intermediates.erase($3);
			       intermediates.insert($$);
				 }
				 ;

Path_Pattern_Factor: ECP_Atom
				   {
				     $$ = $1;
				   }
				   | TOK_L_PARENTHESIS Path_Pattern TOK_R_PARENTHESIS
				   {
				     $$ = $2;
				   }
				   | Path_Pattern_Factor TOK_KLEENE
				   {
		      	   	 $$ = FQL_CREATE3(Repeat, $1, 0, -1);
				     intermediates.erase($1);
			   		 intermediates.insert($$);
				   }
				   | Path_Pattern_Factor TOK_GREATER_OR_EQ TOK_NAT_NUMBER
				   {
		      	   	 $$ = FQL_CREATE3(Repeat, $1, $3, -1);
				     intermediates.erase($1);
			   		 intermediates.insert($$);
				   }
				   | Path_Pattern_Factor TOK_EQ TOK_NAT_NUMBER
				   {
		      	   	 $$ = FQL_CREATE3(Repeat, $1, $3, $3);
				     intermediates.erase($1);
			   		 intermediates.insert($$);
				   }
				   | Path_Pattern_Factor TOK_LESS_OR_EQ TOK_NAT_NUMBER
				   {
		      	   	 $$ = FQL_CREATE3(Repeat, $1, 0, $3);
				     intermediates.erase($1);
			   		 intermediates.insert($$);
				   }
				   ;

ECP_Atom: Predicate
		{
		  $$ = $1;
		}
		| TOK_NODECOV TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
	    {
		  $$ = FQL_CREATE1(Nodecov, $3);
	      intermediates.erase($3);
	      intermediates.insert($$);
	    }
		| TOK_EDGECOV TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
 	    {
		  $$ = FQL_CREATE1(Edgecov, $3);
 	      intermediates.erase($3);
 	      intermediates.insert($$);
 	    }
		| Filter
		{
		  $$ = FQL_CREATE1(Edgecov, $1);
 	      intermediates.erase($1);
 	      intermediates.insert($$);
		}
		| TOK_PATHCOV TOK_L_PARENTHESIS Filter TOK_COMMA TOK_NAT_NUMBER TOK_R_PARENTHESIS
	    {
		  $$ = FQL_CREATE2(Pathcov, $3, $5);
	      intermediates.erase($3);
 	      intermediates.insert($$);
	    }
		| TOK_DEPCOV TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	    {
		  $$ = FQL_CREATE3(Depcov, $3, $5, $7);
	      intermediates.erase($3);
	      intermediates.erase($5);
	      intermediates.erase($7);
 	      intermediates.insert($$);
	    }
		;

Predicate: TOK_ARBITRARY_PRED
		 {
		   ::stream_message_handlert cbmc_msg_handler(*os);
		   ::std::ostringstream pr;
		   pr << "void __fn() { PRED = (_Bool)(" << $1 << ");}" << ::std::endl;
		   // put the string into an istream
		   ::std::istringstream is(pr.str());
		   // run the CBMC C parser
		   ansi_c_parser.clear();
		   // the first line is artificial
		   ansi_c_parser.line_no = 0;
		   ansi_c_parser.filename = "<predicate>";
		   ansi_c_parser.in = &is;
		   ansi_c_parser.set_message_handler(cbmc_msg_handler);
		   ansi_c_parser.grammar = ansi_c_parsert::LANGUAGE;
		   ansi_c_scanner_init();
		   bool result(ansi_c_parser.parse());
		   FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, !result,
		     ::diagnostics::internal::to_string("Failed to parse predicate ", $1));
		   FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
		     1 == ansi_c_parser.parse_tree.items.size());
		   FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
		     1 == (static_cast<const exprt&>(ansi_c_parser.parse_tree.items.front().find(ID_value))).operands().size());
		   FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
		     (static_cast<const exprt&>(ansi_c_parser.parse_tree.items.front().find(ID_value))).operands().front().op0().op0().get("identifier") == "PRED");
		   (static_cast<exprt &>(ansi_c_parser.parse_tree.items.front().add(ID_value))).operands().front().op0().op0().set("identifier", "!PRED!");
		   $$ = ::fshell2::fql::Predicate::Factory::get_instance().create(new
		     ::exprt((static_cast<const exprt&>(ansi_c_parser.parse_tree.items.front().find(ID_value))).operands().front()));
		   intermediates.insert($$);
		   // clean up the things we don't need anymore
		   ansi_c_parser.clear();
		 }
		 ;

Predicates: Predicate
		  {
		  	$$ = new ::std::set< ::fshell2::fql::Predicate *, ::fshell2::fql::FQL_Node_Lt_Compare >;
			$$->insert($1);
		  }
		  | Predicates TOK_COMMA Predicate
		  {
		    $$ = $1;
			$$->insert($3);
		  }
		  ;

Filter: Filter_Function
	  {
	    $$ = $1;
	  }
      | TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
	  {
	    $$ = $2;
	  }
	  | TOK_NOT TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
	  {
        $$ = FQL_CREATE2(Filter_Setminus, FQL_CREATE_FF0(F_IDENTITY), $3);
	 	intermediates.erase($3);
	    intermediates.insert($$);
	  }
	  | Filter TOK_TGUNION Filter
	  {
	    $$ = FQL_CREATE2(Filter_Union, $1, $3);
	 	intermediates.erase($1);
	 	intermediates.erase($3);
	    intermediates.insert($$);
	  }
      | Filter TOK_TGINTERSECT Filter
	  {
	    $$ = FQL_CREATE2(Filter_Intersection, $1, $3);
	 	intermediates.erase($1);
	 	intermediates.erase($3);
	    intermediates.insert($$);
	  }
      | TOK_SETMINUS TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  {
	    $$ = FQL_CREATE2(Filter_Setminus, $3, $5);
	 	intermediates.erase($3);
	 	intermediates.erase($5);
	    intermediates.insert($$);
	  }
	  | TOK_COMPOSE TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  {
	    $$ = FQL_CREATE2(Filter_Compose, $3, $5);
	 	intermediates.erase($3);
	 	intermediates.erase($5);
	    intermediates.insert($$);
	  }
	  | TOK_PRED TOK_L_PARENTHESIS Filter TOK_COMMA Predicates TOK_R_PARENTHESIS
	  {
	    $$ = FQL_CREATE2(Transform_Pred, $3, $5);
	 	intermediates.erase($3);
	    intermediates.insert($$);
	  }
	  ;
	  
Filter_Function: TOK_IDENTITY
			   {
			     $$ = FQL_CREATE_FF0(F_IDENTITY);
	    	     intermediates.insert($$);
			   }
			   | TOK_FILE TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_FILE, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_LINE TOK_L_PARENTHESIS TOK_NAT_NUMBER TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_LINE, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_LINE_ABBREV
			   {
			     $$ = FQL_CREATE_FF1(F_LINE, $1);
	    	     intermediates.insert($$);
			   }
			   | TOK_COLUMN TOK_L_PARENTHESIS TOK_NAT_NUMBER TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_COLUMN, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_FUNC TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_FUNC, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_LABEL TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_LABEL, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_CALL TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_CALL, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_CALLS
			   {
			     $$ = FQL_CREATE_FF0(F_CALLS);
	    	     intermediates.insert($$);
			   }
			   | TOK_ENTRY TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_ENTRY, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_EXIT TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_EXIT, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_EXPR TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_EXPR, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_REGEXP TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_REGEXP, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_BASICBLOCKENTRY
			   {
			     $$ = FQL_CREATE_FF0(F_BASICBLOCKENTRY);
	    	     intermediates.insert($$);
			   }
			   | TOK_CONDITIONEDGE
			   {
			     $$ = FQL_CREATE_FF0(F_CONDITIONEDGE);
	    	     intermediates.insert($$);
			   }
			   | TOK_DECISIONEDGE
			   {
			     $$ = FQL_CREATE_FF0(F_DECISIONEDGE);
	    	     intermediates.insert($$);
			   }
			   | TOK_CONDITIONGRAPH
			   {
			     $$ = FQL_CREATE_FF0(F_CONDITIONGRAPH);
	    	     intermediates.insert($$);
			   }
			   | TOK_DEF TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_DEF, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_USE TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_USE, $3);
	    	     intermediates.insert($$);
			   }
			   | TOK_STMTTYPE TOK_L_PARENTHESIS Stmttypes TOK_R_PARENTHESIS
			   {
			     $$ = FQL_CREATE_FF1(F_STMTTYPE, $3);
	    	     intermediates.insert($$);
			   }
			   ;

Stmttype: TOK_STT_IF
		{
		  $$ = ::fshell2::fql::STT_IF;
		}
		| TOK_STT_FOR
		{
		  $$ = ::fshell2::fql::STT_FOR;
		}
		| TOK_STT_WHILE
		{
		  $$ = ::fshell2::fql::STT_WHILE;
		}
		| TOK_STT_SWITCH
		{
		  $$ = ::fshell2::fql::STT_SWITCH;
		}
		| TOK_STT_CONDOP
		{
		  $$ = ::fshell2::fql::STT_CONDOP;
		}
		| TOK_STT_ASSERT
		{
		  $$ = ::fshell2::fql::STT_ASSERT;
		}
		;

Stmttypes: Stmttype
		 {
		   $$ = $1;
		 }
		 | Stmttypes TOK_COMMA Stmttype
		 {
		   $$ = $1 | $3;
		 }
		 ;

%%

void yyerror(yyFlexLexer *, ::std::ostream *, ::fshell2::fql::Query **, char const* errstr)
{
  for (::std::set< ::fshell2::fql::FQL_Node * >::iterator
  	iter(intermediates.begin()); iter != intermediates.end(); ++iter)
    if (*iter) (*iter)->destroy();
  intermediates.clear();
  FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, false,
  ::diagnostics::internal::to_string( 
    errstr, "; type \"help\" to get usage information" ));
}

