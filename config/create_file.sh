#!/bin/bash

if [ $# -ne 3 ] ; then
  echo "Usage: $0 <filename> <namespace> <classname>" 1>&2
  exit 1
fi

fname="$1"
namespace="`echo $2 | sed 's/^:://'`"
classname="$3"
case `id -un` in
  michael) author="Michael Tautschnig <tautschnig@forsyte.de>" ;;
  holzera) author="Andreas Holzer <holzer@forsyte.de>" ;;
  *) echo "Please put your full name corresponding to your id `id -un` in here" 1>&2 ; exit 1 ;;
esac

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
  *.t.cpp)
    mkdir -p `dirname "$fname"`
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


#include <diagnostics/unittest.hpp>
#include <fshell2/config/config.hpp>
#include <fshell2/config/annotations.hpp>

#include <`echo $fname | sed 's/.t.cpp$/.hpp/'`>

#define TEST_COMPONENT_NAME $classname
#define TEST_COMPONENT_NAMESPACE $namespace

$begin_ns
/** @cond */
TEST_NAMESPACE_BEGIN;
TEST_COMPONENT_TEST_NAMESPACE_BEGIN;
/** @endcond */
using namespace ::diagnostics::unittest;

////////////////////////////////////////////////////////////////////////////////
/**
 * @test A test of $classname
 *
 */
void test( Test_Data & data )
{
}

/** @cond */
TEST_COMPONENT_TEST_NAMESPACE_END;
TEST_NAMESPACE_END;
/** @endcond */

$end_ns
TEST_SUITE_BEGIN;
TEST_NORMAL_CASE( &test, LEVEL_PROD );
TEST_SUITE_END;

STREAM_TEST_SYSTEM_MAIN;
EOF
    ;;
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

$begin_ns
/*! \brief TODO
*/
class $classname
{
	/*! \copydoc doc_self
	*/
	typedef $classname Self;

	public:

	private:
	/*! \copydoc copy_constructor
	*/
	$classname( Self const& rhs );

	/*! \copydoc assignment_op
	 */
	Self& operator=( Self const& rhs );
};

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

$begin_ns

$end_ns
EOF
    ;;
  *)
    echo "Filename must end in either .hpp, .cpp, or .t.cpp" 1>&2
    exit 1
    ;;
esac



