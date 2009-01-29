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
 * $Id: model.hpp 6 2006-11-23 08:52:51Z tautschn $
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
