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
 * PACKAGE, PACKAGE_DESCRIPTION
 * Copyright (C) YEAR  AUTHORS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *******************************************************************************/

/*! \file $fname
 * \brief TODO
 *
 * \$Id$
 * \author $author
 * \date   `date` 
*/


#include <diagnostics/unittest.hpp>
#include <SOURCE/config/config.hpp>
#include <SOURCE/config/annotations.hpp>

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
 * PACKAGE, PACKAGE_DESCRIPTION
 * Copyright (C) YEAR  AUTHORS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

#include <SOURCE/config/config.hpp>

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
 * PACKAGE, PACKAGE_DESCRIPTION
 * Copyright (C) YEAR  AUTHORS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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



