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

#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter_complement.hpp>
#include <fshell2/fql/ast/filter_compose.hpp>
#include <fshell2/fql/ast/filter_enclosing_scopes.hpp>
#include <fshell2/fql/ast/filter_function.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/pm_alternative.hpp>
#include <fshell2/fql/ast/pm_concat.hpp>
#include <fshell2/fql/ast/pm_filter_adapter.hpp>
#include <fshell2/fql/ast/pm_next.hpp>
#include <fshell2/fql/ast/pm_repeat.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/statecov.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/tgs_intersection.hpp>
#include <fshell2/fql/ast/tgs_setminus.hpp>
#include <fshell2/fql/ast/tgs_union.hpp>

#include <cbmc/src/ansi-c/ansi_c_parser.h>
#include <cbmc/src/ansi-c/ansi_c_parse_tree.h>

#define yyparse FQLparse
#define yyerror FQLerror
#define yynerrs FQLnerrs
#define yylval FQLlval
#define yychar FQLchar

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

%union
{
  int NUMBER;
  char* STRING;
  
  ::fshell2::fql::Filter * FILTER;
  ::fshell2::fql::Predicate * PREDICATE;
  ::fshell2::fql::Path_Monitor * PATH_MONITOR;
  ::fshell2::fql::Test_Goal_Sequence * TEST_GOAL_SEQUENCE;
  ::fshell2::fql::Test_Goal_Set * TEST_GOAL_SET;

  ::std::set< ::fshell2::fql::Predicate *,
    ::fshell2::fql::FQL_Node_Lt_Compare > * PREDICATE_SET;
  ::std::list< ::std::pair< ::fshell2::fql::Path_Monitor *,
    ::fshell2::fql::Test_Goal_Set * > > * TGS_LIST;

  ::exprt * EXPRT;
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
/* operations on target graphs */
%token TOK_COMPLEMENT
%token TOK_UNION
%token TOK_INTERSECT
%token TOK_SETMINUS
%token TOK_ENCLOSING_SCOPES
%token TOK_COMPOSE
/* abstraction/predicates */
%token TOK_L_BRACE
%token TOK_R_BRACE
%token TOK_GREATER_OR_EQ
%token TOK_GREATER
%token TOK_EQ
%token TOK_LESS_OR_EQ
%token TOK_LESS
%token TOK_NEQ
/* coverage specification */
%token TOK_STATECOV
%token TOK_EDGECOV
%token TOK_PATHCOV
%token TOK_L_SEQ
%token TOK_R_SEQ
/* path monitors */
%token TOK_NEXT
%token TOK_CONCAT
%token TOK_ALTERNATIVE
%token TOK_KLEENE
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

%type <FILTER> Scope Filter Filter_Function
%type <TEST_GOAL_SEQUENCE> Cover
%type <PATH_MONITOR> Passing Path_Monitor Path_Monitor_Term Path_Monitor_Factor
%type <PATH_MONITOR> Path_Monitor_Symbol
%type <TGS_LIST> Test_Goal_Sequence
%type <TEST_GOAL_SET> Test_Goal_Set
%type <PREDICATE_SET> Predicates Preconditions
%type <PREDICATE> Predicate
%type <EXPRT> Comparison c_LHS

%%

Query: Scope Cover Passing
	 {
	   *query_ast = ::fshell2::fql::Query::Factory::get_instance().create($1, $2, $3);
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

Cover: TOK_COVER Test_Goal_Sequence
	 {
	   $$ = ::fshell2::fql::Test_Goal_Sequence::Factory::get_instance().create(*$2, 0);
	   delete $2;
	   intermediates.insert($$);
	 }
	 ;

Passing: /* empty */
	   {
	     $$ = 0;
	   }
	   | TOK_PASSING Path_Monitor
	   {
	   	 $$ = $2;
	   }
	   ;

Test_Goal_Sequence: Test_Goal_Set
				  {
					$$ = new ::fshell2::fql::Test_Goal_Sequence::seq_t;
					$$->push_back(::std::make_pair< ::fshell2::fql::Path_Monitor *,
					  ::fshell2::fql::Test_Goal_Set * >(0, $1));
				  }
				  | Test_Goal_Sequence TOK_L_SEQ Path_Monitor TOK_R_SEQ Test_Goal_Set
				  {
				    $$ = $1;
					$$->push_back(::std::make_pair($3, $5));
				  }
				  | Test_Goal_Sequence TOK_NEXT Test_Goal_Set
				  {
				    $$ = $1;
					$1->push_back(::std::make_pair< ::fshell2::fql::Path_Monitor *,
					  ::fshell2::fql::Test_Goal_Set * >(0, $3));
				  }
				  ;

Path_Monitor: Path_Monitor_Term
		    {
		      $$ = $1;
		    }
		    | Path_Monitor TOK_ALTERNATIVE Path_Monitor_Term
		    {
		      $$ = ::fshell2::fql::PM_Alternative::Factory::get_instance().create($1, $3);
			  intermediates.erase($1);
			  intermediates.erase($3);
			  intermediates.insert($$);
		    }
		    ;

Path_Monitor_Term: Path_Monitor_Factor
			     {
				   $$ = $1;
				 }
				 | Path_Monitor_Term TOK_CONCAT Path_Monitor_Factor
				 {
		      	   $$ = ::fshell2::fql::PM_Concat::Factory::get_instance().create($1, $3);
				   intermediates.erase($1);
				   intermediates.erase($3);
			   	   intermediates.insert($$);
				 }
				 | Path_Monitor_Term TOK_NEXT Path_Monitor_Factor
				 {
		      	   $$ = ::fshell2::fql::PM_Next::Factory::get_instance().create($1, $3);
				   intermediates.erase($1);
				   intermediates.erase($3);
			       intermediates.insert($$);
				 }
				 ;

Path_Monitor_Factor: Preconditions Path_Monitor_Symbol
				   {
				     $$ = $2;
					 if ($1 != 0) {
					   for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
					     iter != $1->end(); ++iter)
						 $$->add_precond(*iter);
					   delete $1;
					 }
				   }
				   | Preconditions TOK_L_PARENTHESIS Path_Monitor TOK_R_PARENTHESIS
				   {
				     $$ = $3;
					 if ($1 != 0) {
					   for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
					     iter != $1->end(); ++iter)
						 $$->add_precond(*iter);
					   delete $1;
					 }
				   }
				   | Path_Monitor_Factor Predicate
				   {
				     $$ = $1;
					 $$->add_postcond($2);
				   }
				   | Path_Monitor_Factor TOK_KLEENE
				   {
		      	   	 $$ = ::fshell2::fql::PM_Repeat::Factory::get_instance().create($1, 0, -1);
				     intermediates.erase($1);
			   		 intermediates.insert($$);
				   }
				   | Path_Monitor_Factor TOK_LESS_OR_EQ TOK_NAT_NUMBER
				   {
		      	   	 $$ = ::fshell2::fql::PM_Repeat::Factory::get_instance().create($1, 0, $3);
				     intermediates.erase($1);
			   		 intermediates.insert($$);
				   }
				   | Path_Monitor_Factor TOK_GREATER_OR_EQ TOK_NAT_NUMBER
				   {
		      	   	 $$ = ::fshell2::fql::PM_Repeat::Factory::get_instance().create($1, $3, -1);
				     intermediates.erase($1);
			   		 intermediates.insert($$);
				   }
				   ;

Path_Monitor_Symbol: Filter
				   {
				     $$ = ::fshell2::fql::PM_Filter_Adapter::Factory::get_instance().create($1);
				     intermediates.erase($1);
			   		 intermediates.insert($$);
				   }
				   ;

Test_Goal_Set: Preconditions TOK_L_PARENTHESIS Test_Goal_Set TOK_R_PARENTHESIS
			 {
			   $$ = $3;
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			 }
			 | Test_Goal_Set Predicate
			 {
			   $$ = $1;
			   $$->add_postcond($2);
			 }
			 | Preconditions TOK_UNION TOK_L_PARENTHESIS Test_Goal_Set TOK_COMMA Test_Goal_Set TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::TGS_Union::Factory::get_instance().create($4, $6);
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			   intermediates.erase($4);
			   intermediates.erase($6);
			   intermediates.insert($$);
			 }
			 | Preconditions TOK_INTERSECT TOK_L_PARENTHESIS Test_Goal_Set TOK_COMMA Test_Goal_Set TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::TGS_Intersection::Factory::get_instance().create($4, $6);
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			   intermediates.erase($4);
			   intermediates.erase($6);
			   intermediates.insert($$);
			 }
			 | Preconditions TOK_SETMINUS TOK_L_PARENTHESIS Test_Goal_Set TOK_COMMA Test_Goal_Set TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::TGS_Setminus::Factory::get_instance().create($4, $6);
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			   intermediates.erase($4);
			   intermediates.erase($6);
			   intermediates.insert($$);
			 }
			 | Preconditions TOK_STATECOV TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::Statecov::Factory::get_instance().create($4,
			     static_cast< ::fshell2::fql::Predicate::preds_t * >(0));
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			   intermediates.erase($4);
			   intermediates.insert($$);
			 }
			 | Preconditions TOK_STATECOV TOK_L_PARENTHESIS Filter TOK_COMMA Predicates TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::Statecov::Factory::get_instance().create($4, $6);
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			   intermediates.erase($4);
			   intermediates.insert($$);
			 }
			 | Preconditions TOK_EDGECOV TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::Edgecov::Factory::get_instance().create($4,
			     static_cast< ::fshell2::fql::Predicate::preds_t * >(0));
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			   intermediates.erase($4);
			   intermediates.insert($$);
			 }
			 | Preconditions TOK_EDGECOV TOK_L_PARENTHESIS Filter TOK_COMMA Predicates TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::Edgecov::Factory::get_instance().create($4, $6);
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			   intermediates.erase($4);
			   intermediates.insert($$);
			 }
			 | Preconditions TOK_PATHCOV TOK_L_PARENTHESIS Filter TOK_COMMA TOK_NAT_NUMBER TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::Pathcov::Factory::get_instance().create($4, $6, 0);
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			   intermediates.erase($4);
			   intermediates.insert($$);
			 }
			 | Preconditions TOK_PATHCOV TOK_L_PARENTHESIS Filter TOK_COMMA TOK_NAT_NUMBER TOK_COMMA Predicates TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::Pathcov::Factory::get_instance().create($4, $6, $8);
			   if ($1 != 0) {
			     for (::fshell2::fql::Predicate::preds_t::const_iterator iter($1->begin());
			       iter != $1->end(); ++iter)
			       $$->add_precond(*iter);
			     delete $1;
			   }
			   intermediates.erase($4);
			   intermediates.insert($$);
			 }
			 ;

Preconditions: /* empty */
			 {
			   $$ = 0;
			 }
			 | Preconditions Predicate
			 {
			   $$ = $1;
			   if (0 == $$)
				 $$ = new ::std::set< ::fshell2::fql::Predicate *,
				   ::fshell2::fql::FQL_Node_Lt_Compare >;
			   $$->insert($2);
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

Predicate: TOK_L_BRACE c_LHS Comparison c_LHS TOK_R_BRACE
		 {
		   $3->reserve_operands(2);
		   $3->move_to_operands(*$2);
		   delete $2;
		   $3->move_to_operands(*$4);
		   delete $4;
		   $$ = ::fshell2::fql::Predicate::Factory::get_instance().create($3);
		   intermediates.insert($$);
		 }
		 | TOK_L_BRACE TOK_QUOTED_STRING TOK_R_BRACE
		 {
		   ::stream_message_handlert cbmc_msg_handler(*os);
		   ::std::ostringstream pr;
		   pr << "void __fn() {" << $2 << ";}" << ::std::endl;
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
		     ::diagnostics::internal::to_string("Failed to parse predicate ", $2));
		   FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
		     1 == ansi_c_parser.parse_tree.declarations.size());
		   FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
		     1 == ansi_c_parser.parse_tree.declarations.front().value().operands().size());
		   $$ = ::fshell2::fql::Predicate::Factory::get_instance().create(new
		     ::exprt(ansi_c_parser.parse_tree.declarations.front().value().operands().front()));
		   intermediates.insert($$);
		   // clean up the things we don't need anymore
		   ansi_c_parser.clear();
		 }
		 ;

// extend
c_LHS: TOK_C_IDENT
	 {
	   $$ = new ::exprt("symbol");
	   $$->set("#base_name", $1);
	   $$->set("#identifier", $1);
	 }
	 | TOK_NAT_NUMBER
	 {
	   $$ = new ::exprt("constant", ::typet("integer"));
	   $$->set("value", $1);
	 }
	 ;

Comparison: TOK_GREATER_OR_EQ
		  {
		    $$ = new ::exprt("code", ::typet("code"));
			$$->id(">=");
		  }
		  | TOK_GREATER
		  {
		    $$ = new ::exprt("code", ::typet("code"));
			$$->id(">");
		  }
		  | TOK_EQ
		  {
		    $$ = new ::exprt("code", ::typet("code"));
			$$->id("=");
		  }
		  | TOK_LESS_OR_EQ
		  {
		    $$ = new ::exprt("code", ::typet("code"));
			$$->id("<=");
		  }
		  | TOK_LESS
		  {
		    $$ = new ::exprt("code", ::typet("code"));
			$$->id("<");
		  }
		  | TOK_NEQ
		  {
		    $$ = new ::exprt("code", ::typet("code"));
			$$->id("notequal");
		  }
		  ;

Filter: Filter_Function
	  {
	    $$ = $1;
	  }
	  | TOK_COMPLEMENT TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Complement::Factory::get_instance().create($3);
	 	intermediates.erase($3);
	    intermediates.insert($$);
	  }
	  | TOK_UNION TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Union::Factory::get_instance().create($3, $5);
	 	intermediates.erase($3);
	 	intermediates.erase($5);
	    intermediates.insert($$);
	  }
      | TOK_INTERSECT TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Intersection::Factory::get_instance().create($3, $5);
	 	intermediates.erase($3);
	 	intermediates.erase($5);
	    intermediates.insert($$);
	  }
      | TOK_SETMINUS TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Setminus::Factory::get_instance().create($3, $5);
	 	intermediates.erase($3);
	 	intermediates.erase($5);
	    intermediates.insert($$);
	  }
	  | TOK_ENCLOSING_SCOPES TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Enclosing_Scopes::Factory::get_instance().create($3);
	 	intermediates.erase($3);
	    intermediates.insert($$);
	  }
	  | TOK_COMPOSE TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Compose::Factory::get_instance().create($3, $5);
	 	intermediates.erase($3);
	 	intermediates.erase($5);
	    intermediates.insert($$);
	  }
	  ;

Filter_Function: TOK_IDENTITY
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_IDENTITY>();
	    	     intermediates.insert($$);
			   }
			   | TOK_FILE TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_FILE>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_LINE TOK_L_PARENTHESIS TOK_NAT_NUMBER TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_LINE>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_LINE_ABBREV
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_LINE>($1);
	    	     intermediates.insert($$);
			   }
			   | TOK_COLUMN TOK_L_PARENTHESIS TOK_NAT_NUMBER TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_COLUMN>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_FUNC TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_FUNC>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_LABEL TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_LABEL>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_CALL TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_CALL>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_CALLS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_CALLS>();
	    	     intermediates.insert($$);
			   }
			   | TOK_ENTRY TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_ENTRY>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_EXIT TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_EXIT>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_EXPR TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_EXPR>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_REGEXP TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_REGEXP>($3);
	    	     intermediates.insert($$);
			   }
			   | TOK_BASICBLOCKENTRY
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_BASICBLOCKENTRY>();
	    	     intermediates.insert($$);
			   }
			   | TOK_CONDITIONEDGE
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_CONDITIONEDGE>();
	    	     intermediates.insert($$);
			   }
			   | TOK_DECISIONEDGE
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_DECISIONEDGE>();
	    	     intermediates.insert($$);
			   }
			   | TOK_CONDITIONGRAPH
			   {
			     $$ = ::fshell2::fql::Filter_Function::Factory::get_instance().create<
                   ::fshell2::fql::F_CONDITIONGRAPH>();
	    	     intermediates.insert($$);
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

