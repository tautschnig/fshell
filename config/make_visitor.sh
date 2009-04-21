#!/bin/bash

decls=""
func_decls=""
func_defs=""

for c in  \
FQL_Node \
Query \
Test_Goal_Sequence \
Test_Goal_Set \
Restriction_Automaton \
Abstraction \
Filter \
Predicate \
TGS_Union \
TGS_Intersection \
TGS_Setminus \
Edgecov \
Pathcov \
Filter_Complement \
Filter_Union \
Filter_Intersection \
Filter_Setminus \
Filter_Enclosing_Scopes \
Primitive_Filter
do
  decls="$decls
class $c;"
  func_decls="$func_decls

		/*! \{
		 * \brief Visit a @ref fshell2::fql::$c
		 * \param  n $c
		 */
		virtual void visit($c const* n);
		virtual void visit($c const* n) const;
		/*! \} */"
  func_defs="$func_defs

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit($c const* n)
{
}

template <typename INTERFACE>
void Standard_AST_Visitor_Aspect<INTERFACE>::visit($c const* n) const
{
}"
done

echo "$decls"
echo
echo "$func_decls"
echo
echo "$func_defs"

