#!/bin/bash


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
  for t in hpp cpp ; do
    fname=`echo $c | awk '{ print tolower( $1 ) }'`
    fname="fshell2/fql/ast/$fname.$t"
    namespace="fshell2::fql"
    classname="$c"
    author="Michael Tautschnig <tautschnig@forsyte.de>"

    pre_ns=""
    begin_ns=""
    end_ns=""

    for comp in `echo $namespace | sed 's/::/\n/g'` ; do
      ns="`echo $comp | awk '{ print toupper( $1 ) }'`_"
      begin_ns="$begin_ns$pre_ns${ns}NAMESPACE_BEGIN;
"
      end_ns="$pre_ns${ns}NAMESPACE_END;
$end_ns"
      pre_ns="$pre_ns$ns"
    done

    if [ -e $fname ] ; then
      echo "$fname already exists" 1>&2
      exit 3
    fi

    case "$fname" in
      *.hpp)
        mkdir -p `dirname "$fname"`
        pattern="`echo $fname | awk '{ print toupper( $1 ) }'`"
        pattern="`echo $pattern | sed -e 's#/#__#g' -e 's#\.#_#g'`"
        cat > $fname <<EOF
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

#ifndef $pattern
#define $pattern

/*! \file $fname
 * \brief TODO
 *
 * \$Id$
 * \author $author
 * \date   `date` 
*/

#include <fshell2/config/config.hpp>
#include <fshell2/fql/ast/fql_node.hpp>
#include <fshell2/fql/ast/fql_node_factory.hpp>

$begin_ns
/*! \brief TODO
*/
class $classname : public FQL_Node
{
	/*! \copydoc doc_self
	*/
	typedef $classname Self;

	public:
  typedef FQL_Node_Factory<Self> Factory;

  /*! \{
   * \brief Accept a visitor 
   * \param  v Visitor
   */
  virtual void accept(AST_Visitor * v) const;
  virtual void accept(AST_Visitor const * v) const;
  /*! \} */
		
  virtual bool destroy();	

	private:
	friend Self * FQL_Node_Factory<Self>::create();
	friend FQL_Node_Factory<Self>::~FQL_Node_Factory<Self>();

  /*! Constructor
  */
  $classname();

	/*! \copydoc copy_constructor
	*/
	$classname( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
		
  /*! \brief Destructor
  */
  virtual ~$classname();
};

template <>
inline $classname * FQL_Node_Factory<$classname>::create() {
}

$end_ns
#endif /* $pattern */
EOF
        ;;
      *.cpp)
        cat > $fname <<EOF
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

/*! \file $fname
 * \brief TODO
 *
 * \$Id$
 * \author $author
 * \date   `date` 
*/

#include <`echo $fname | sed 's/.cpp$/.hpp/'`>

#include <fshell2/fql/ast/ast_visitor.hpp>

$begin_ns

$classname::$classname() {
}

void $classname::accept(AST_Visitor * v) const {
    v->visit(this);
}

void $classname::accept(AST_Visitor const * v) const {
    v->visit(this);
}

bool $classname::destroy() {
    if (this->m_ref_count) return false;
    Factory::get_instance().destroy(this);
    return true;
}

$classname::~$classname() {
}

$end_ns
EOF
        ;;
      *)
        echo "Filename must end in either .hpp, .cpp, or .t.cpp" 1>&2
        exit 1
        ;;
    esac
  done
done

