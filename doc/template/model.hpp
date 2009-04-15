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


#ifndef DOC__TEMPLATE__MODEL_HPP
#define DOC__TEMPLATE__MODEL_HPP

////////////////////////////////////////////////////////////////////////////////
/**
 * @file doc/template/model.hpp
 *
 * @brief An example of a .hpp file
 *
 * This template shows some standard ways of writing proper code within the
 * project
 *
 *
 * $Id$
 *
 * @author Michael Tautschnig  <tautschn@model.in.tum.de>
 * @date   Thu Nov 09 07:13:48 CET 2006
*/

#include <shared/config/config.hpp>

PACKAGE_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
/**
 * @brief No real class
 *
 * This class only serves as an example to show some coding standards. It must
 * not be used in any implementation.
 *
 * Furthermore some doxygen headers are shown and explained here.
 *
 */
class No_Such_Class
{
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * @copydoc doc_self
	 *
	 * This typedef allows the use of Self instead of the real class name within
	 * the class, which is very handy once lots of template parameters are used.
	 *
	 * The copydoc construct stops the tedious writing of the same comment again
	 * and again. doc_self is defined in @ref shared/config/config.hpp.
	 *
	 */
	typedef No_Such_Class Self;

	public:
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * @brief This method returns false
	 *
	 * The implementation of this function is found in @ref doc/template/sat7.cpp
	 *
	 * @param x An integer
	 *
	 * @return Truth value, always false
	 */
	bool no_such_method( const int x );

	private:
	////////////////////////////////////////////////////////////////////////////////
	/**
	 * @copydoc copy_constructor
	 *
	 * The copy constructor is private and not implemented. This ensures that it
	 * is not used implictly.
	 *
	 */
	No_Such_Class( const Self& rhs );

	////////////////////////////////////////////////////////////////////////////////
	/**
	 * @copydoc assignment_op
	 *
	 * The assignment operator is private and not implemented. This ensures that it
	 * is not used implictly.
	 *
	 */
	Self& operator=( const Self& rhs );

	////////////////////////////////////////////////////////////////////////////////
	/**
	 * @brief A data member
	 *
	 */
	int m_data;
};

PACKAGE_NAMESPACE_END;

#endif /* DOC__TEMPLATE__MODEL_HPP */
