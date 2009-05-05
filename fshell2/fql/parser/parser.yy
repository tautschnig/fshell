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

#include <fshell2/fql/ast/abstraction.hpp>
#include <fshell2/fql/ast/edgecov.hpp>
#include <fshell2/fql/ast/filter.hpp>
#include <fshell2/fql/ast/filter_complement.hpp>
#include <fshell2/fql/ast/filter_enclosing_scopes.hpp>
#include <fshell2/fql/ast/filter_intersection.hpp>
#include <fshell2/fql/ast/filter_setminus.hpp>
#include <fshell2/fql/ast/filter_union.hpp>
#include <fshell2/fql/ast/pathcov.hpp>
#include <fshell2/fql/ast/predicate.hpp>
#include <fshell2/fql/ast/primitive_filter.hpp>
#include <fshell2/fql/ast/query.hpp>
#include <fshell2/fql/ast/restriction_automaton.hpp>
#include <fshell2/fql/ast/test_goal_sequence.hpp>
#include <fshell2/fql/ast/test_goal_set.hpp>
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
  
  ::fshell2::fql::Abstraction * ABSTRACTION;
  ::fshell2::fql::Filter * FILTER;
  ::fshell2::fql::Predicate * PREDICATE;
  ::fshell2::fql::Restriction_Automaton * RESTRICTION_AUTOMATON;
  ::fshell2::fql::Test_Goal_Sequence * TEST_GOAL_SEQUENCE;
  ::fshell2::fql::Test_Goal_Set * TEST_GOAL_SET;

  ::std::set< ::fshell2::fql::Predicate *,
    ::fshell2::fql::FQL_Node_Lt_Compare > * PREDICATE_SET;
  ::std::list< ::std::pair< ::fshell2::fql::Restriction_Automaton *,
    ::fshell2::fql::Test_Goal_Set * > > * TGS_LIST;
  ::std::pair< ::fshell2::fql::Filter *,
    ::fshell2::fql::Predicate * > * AUT_STEP_PAIR;

  ::exprt * EXPRT;
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

%type <FILTER> Scope Filter Primitive_Filter
%type <TEST_GOAL_SEQUENCE> Cover
%type <RESTRICTION_AUTOMATON> Passing Automaton Step_Or_Nested_Aut
%type <TGS_LIST> Test_Goal_Sequence
%type <TEST_GOAL_SET> Test_Goal_Set
%type <ABSTRACTION> Abstraction
%type <PREDICATE_SET> Predicates
%type <PREDICATE> Predicate
%type <AUT_STEP_PAIR> Aut_Step
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
	   | TOK_PASSING Automaton
	   {
	   	 $$ = $2;
	   }
	   ;

Test_Goal_Sequence: Test_Goal_Set
				  {
					$$ = new ::fshell2::fql::Test_Goal_Sequence::seq_t;
					$$->push_back(::std::make_pair< ::fshell2::fql::Restriction_Automaton *,
					  ::fshell2::fql::Test_Goal_Set * >(0, $1));
				  }
				  | Test_Goal_Sequence Automaton Test_Goal_Set
				  {
				    $$ = $1;
					$$->push_back(::std::make_pair($2, $3));
				  }
				  ;

Automaton: TOK_L_PARENTHESIS Automaton TOK_R_PARENTHESIS
		 {
		   $$ = $2;
		 }
		 | Aut_Step
		 {
		   $$ = 0;
		 }
		 | Step_Or_Nested_Aut TOK_COMMA Step_Or_Nested_Aut
		 {
		   $$ = 0;
		 }
		 | Automaton TOK_NEXT Step_Or_Nested_Aut
		 {
		   $$ = 0;
		 }
		 | Automaton TOK_NEXT_START Aut_Step TOK_NEXT_END Step_Or_Nested_Aut
		 {
		   $$ = 0;
		 }
		 | Step_Or_Nested_Aut TOK_LESS_OR_EQ TOK_NAT_NUMBER
		 {
		   $$ = 0;
		 }
		 | Step_Or_Nested_Aut TOK_GREATER_OR_EQ TOK_NAT_NUMBER
		 {
		   $$ = 0;
		 }
		 ;

Step_Or_Nested_Aut: TOK_L_PARENTHESIS Automaton TOK_R_PARENTHESIS
				  {
					$$ = $2;
				  }
				  | Aut_Step
				  {
					$$ = 0;
				  }

Aut_Step: Filter
		{
		  $$ = new ::std::pair< ::fshell2::fql::Filter *, ::fshell2::fql::Predicate * >($1, 0);
		}
		| Predicate
		{
		  $$ = new ::std::pair< ::fshell2::fql::Filter *, ::fshell2::fql::Predicate * >(0, $1);
		}
		| Filter TOK_SUCH_THAT Predicate
		{
		  $$ = new ::std::pair< ::fshell2::fql::Filter *, ::fshell2::fql::Predicate * >($1, $3);
		}
		;

Test_Goal_Set: TOK_L_PARENTHESIS Test_Goal_Set TOK_R_PARENTHESIS
			 {
			   $$ = $2;
			 }
			 | TOK_UNION TOK_L_PARENTHESIS Test_Goal_Set TOK_COMMA Test_Goal_Set TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::TGS_Union::Factory::get_instance().create($3, $5);
			   intermediates.insert($$);
			 }
			 | TOK_INTERSECT TOK_L_PARENTHESIS Test_Goal_Set TOK_COMMA Test_Goal_Set TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::TGS_Intersection::Factory::get_instance().create($3, $5);
			   intermediates.insert($$);
			 }
			 | TOK_SETMINUS TOK_L_PARENTHESIS Test_Goal_Set TOK_COMMA Test_Goal_Set TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::TGS_Setminus::Factory::get_instance().create($3, $5);
			   intermediates.insert($$);
			 }
			 | TOK_EDGECOV TOK_L_PARENTHESIS Abstraction TOK_COMMA Filter TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::Edgecov::Factory::get_instance().create($3, $5);
			   intermediates.insert($$);
			 }
			 | TOK_PATHCOV TOK_L_PARENTHESIS Abstraction TOK_COMMA Filter TOK_COMMA TOK_NAT_NUMBER TOK_R_PARENTHESIS
			 {
			   $$ = ::fshell2::fql::Pathcov::Factory::get_instance().create($3, $5, $7);
			   intermediates.insert($$);
			 }
			 ;

Abstraction: TOK_CFG
		   {
		     ::std::set< ::fshell2::fql::Predicate *, ::fshell2::fql::FQL_Node_Lt_Compare > empty;
			 $$ = ::fshell2::fql::Abstraction::Factory::get_instance().create(empty);
			 intermediates.insert($$);
		   }
		   | TOK_CFG TOK_L_PARENTHESIS Predicates TOK_R_PARENTHESIS
		   {
		     $$ = ::fshell2::fql::Abstraction::Factory::get_instance().create(*$3);
			 delete $3;
			 intermediates.insert($$);
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
		   $$ = 0; //::fshell2::fql::Predicate::Factory::get_instance().create($3);
		   //intermediates.insert($$);
		 }
		 | TOK_PREDICATE TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
		 {
		   ::stream_message_handlert cbmc_msg_handler(*os);
		   ::std::ostringstream pr;
		   pr << "void __fn() {" << $3 << ";}" << ::std::endl;
		   // put the string into an istream
		   ::std::istringstream is(pr.str());
		   // run the CBMC C parser
		   ansi_c_parser.clear();
		   // the first line is artificial
		   ansi_c_parser.line_no = 0;
		   ansi_c_parser.filename = "<predicate>";
		   ansi_c_parser.in = &is;
		   ansi_c_parser.set_message_handler(&cbmc_msg_handler);
		   ansi_c_parser.grammar = ansi_c_parsert::LANGUAGE;
		   ansi_c_scanner_init();
		   bool result(ansi_c_parser.parse());
		   FSHELL2_PROD_CHECK1(::diagnostics::Parse_Error, !result,
		     ::diagnostics::internal::to_string("Failed to parse predicate ", $3));
		   FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
		     1 == ansi_c_parser.parse_tree.declarations.size());
		   FSHELL2_AUDIT_ASSERT(::diagnostics::Violated_Invariance,
		     1 == ansi_c_parser.parse_tree.declarations.front().value().operands().size());
		   $$ = 0; /*::fshell2::fql::Predicate::Factory::get_instance().create(new
		     ::exprt(ansi_c_parser.parse_tree.declarations.front().value().operands().front()));
		   intermediates.insert($$);*/
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

Filter: Primitive_Filter
	  {
	    $$ = $1;
	  }
	  | TOK_IDENTITY
	  {
	    $$ = ::fshell2::fql::Filter_Identity::Factory::get_instance().create();
	    intermediates.insert($$);
	  }
	  | TOK_COMPLEMENT TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Complement::Factory::get_instance().create($3);
	    intermediates.insert($$);
	  }
	  | TOK_UNION TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Union::Factory::get_instance().create($3, $5);
	    intermediates.insert($$);
	  }
      | TOK_INTERSECT TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Intersection::Factory::get_instance().create($3, $5);
	    intermediates.insert($$);
	  }
      | TOK_SETMINUS TOK_L_PARENTHESIS Filter TOK_COMMA Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Setminus::Factory::get_instance().create($3, $5);
	    intermediates.insert($$);
	  }
	  | TOK_ENCLOSING_SCOPES TOK_L_PARENTHESIS Filter TOK_R_PARENTHESIS
	  {
	    $$ = ::fshell2::fql::Filter_Enclosing_Scopes::Factory::get_instance().create($3);
	    intermediates.insert($$);
	  }
	  ;

Primitive_Filter: TOK_FILE TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_FILE>($3);
	    	      intermediates.insert($$);
				}
				| TOK_LINE TOK_L_PARENTHESIS TOK_NAT_NUMBER TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_LINE>($3);
	    	      intermediates.insert($$);
				}
				| TOK_COLUMN TOK_L_PARENTHESIS TOK_NAT_NUMBER TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_COLUMN>($3);
	    	      intermediates.insert($$);
				}
				| TOK_FUNC TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_FUNC>($3);
	    	      intermediates.insert($$);
				}
				| TOK_LABEL TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_LABEL>($3);
	    	      intermediates.insert($$);
				}
				| TOK_CALL TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_CALL>($3);
	    	      intermediates.insert($$);
				}
				| TOK_CALLS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_CALLS>();
	    	      intermediates.insert($$);
				}
				| TOK_ENTRY TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_ENTRY>($3);
	    	      intermediates.insert($$);
				}
				| TOK_EXIT TOK_L_PARENTHESIS TOK_C_IDENT TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_EXIT>($3);
	    	      intermediates.insert($$);
				}
				| TOK_EXPR TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_EXPR>($3);
	    	      intermediates.insert($$);
				}
				| TOK_REGEXP TOK_L_PARENTHESIS TOK_QUOTED_STRING TOK_R_PARENTHESIS
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_REGEXP>($3);
	    	      intermediates.insert($$);
				}
				| TOK_BASICBLOCKENTRY
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_BASICBLOCKENTRY>();
	    	      intermediates.insert($$);
				}
				| TOK_CONDITIONEDGE
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_CONDITIONEDGE>();
	    	      intermediates.insert($$);
				}
				| TOK_DECISIONEDGE
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
                    ::fshell2::fql::F_DECISIONEDGE>();
	    	      intermediates.insert($$);
				}
				| TOK_CONDITIONGRAPH
				{
				  $$ = ::fshell2::fql::Primitive_Filter::Factory::get_instance().create<
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

